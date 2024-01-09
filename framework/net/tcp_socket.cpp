#include "tcp_socket.h"
#include "base.h"
#include "log.h"

CTCPSocket::CTCPSocket(unsigned int RecvBufLen_, unsigned int SendBufLen_)
{
	m_nStatus = eTcpClosed;
	m_nRecvBuffLen = RecvBufLen_;
	m_nSendBuffLen = SendBufLen_;
	m_pReadBuff = new CByteBuff(RecvBufLen_);
	m_pWriteBuff = new CByteBuff(SendBufLen_);
}

CTCPSocket::~CTCPSocket()
{
	m_pReadBuff.ForceFree();
	m_pWriteBuff.ForceFree();
}

int CTCPSocket::ConnectTo(const char* szIPAddr, u_short unPort, bool block)
{
	if (!m_Socket.IsValid())
	{
		if (!m_Socket.Open())
		{
			return -1;
		}
	}
	m_nStatus = eTcpCreated;
	if (!m_Socket.SetRecvBufSize(m_nRecvBuffLen))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(m_nSendBuffLen))
	{
		Close();
		return -1;
	}

	if (!block)
	{
		m_Socket.SetSocketNoBlock();
	}

	int nConRet = m_Socket.Conn(szIPAddr,unPort,block);
	if (nConRet == SOCKET_ERROR)
	{
		Close();
		return -2;
	}
	else if (nConRet == 1)
	{
		m_nStatus = eTcpConnecting;
		//非阻塞连接中，检查是否连接成功
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_nStatus = eTcpConnected;
	}
	return 0;
}

int CTCPSocket::CheckConnectedOk()
{
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000; //10毫秒

	fd_set readfds, writefds, execpfds;
	SOCKET nSocketFd = m_Socket.GetSocket();

	FD_ZERO(&writefds);
	FD_SET(m_Socket.GetSocket(), &writefds);
	readfds = writefds;
	execpfds = writefds;
	int n = select(nSocketFd + 1, &readfds, &writefds, &execpfds, &tv);
	if (n < 0)
	{
		LOG_ERROR("default", "select error({}:{})", errno, strerror(errno));
		Close();
		return -2;
	}

	if (n == 0)
	{
		return 0;
	}

	if (!FD_ISSET(nSocketFd, &readfds) && !FD_ISSET(nSocketFd, &writefds))
	{
		LOG_ERROR("default", "socket {} can't read and write", nSocketFd);
		Close();
		return -3;
	}

	if (m_Socket.IsSocketError())
	{
		LOG_ERROR("default", "connect failure on SOCKET {} error:{} msg:{}.", m_Socket.GetSocket(), errno, strerror(errno));
		Close();
		return -4;
	}
	m_nStatus = eTcpConnected;
	return 0;
}

int CTCPSocket::Close()
{
	m_Socket.Close();
	m_nStatus = eTcpClosed;
	return 0;
}

SOCKET CTCPSocket::GetSocketFD()
{
	return m_Socket.GetSocket();
}

CSocket& CTCPSocket::GetSocket()
{
	return m_Socket;
}

eTcpStatus CTCPSocket::GetStatus()
{
	return (eTcpStatus)m_nStatus;
}

int CTCPSocket::RecvData()
{
	int nRetCode = m_pReadBuff->Recv(m_Socket);
	if (nRetCode == ERR_RECV_WOULD_BLOCK)
	{
		nRetCode = ERR_RECV_OK;
	}
	return nRetCode;
}

int CTCPSocket::Write(BYTE* pCode, msize_t nCodeLength)
{
	if (!m_Socket.IsValid())
	{
		return ERR_SEND_NOSOCK;
	}

	int retCode = ERR_SEND_OK;
	//有残留数据待发送
	if (m_pWriteBuff->CanReadLen() > 0)
	{
		retCode = m_pWriteBuff->Send(m_Socket);
		if (retCode < 0)
		{
			return retCode;
		}
	}

	//没有数据了，索引都归0吧
	if (m_pWriteBuff->CanReadLen() == 0)
	{
		m_pWriteBuff->Clear();
	}

	if (pCode != NULL && nCodeLength > 0)
	{
		if (m_pWriteBuff->WriteBytes(pCode, nCodeLength) == -1)
		{
			return ERR_SEND_NOBUFF;
		}

		if (retCode != ERR_SEND_WOULD_BLOCK)
		{
			//再次尝试发送本次写入缓冲区的数据
			if (m_pWriteBuff->CanReadLen() > 0)
			{
				retCode = m_pWriteBuff->Send(m_Socket);
				if (retCode < 0)
				{
					return retCode;
				}
			}

			//没有数据了，索引都归0吧
			if (m_pWriteBuff->CanReadLen() == 0)
			{
				m_pWriteBuff->Clear();
			}
		}
	}
	return ERR_SEND_OK;
}

int CTCPSocket::Flush()
{
	return Write(NULL,0);
}

int CTCPSocket::GetOneCode(unsigned short& nCodeLength, BYTE* pCode)
{

}

bool CTCPSocket::AddToFDSet(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_nStatus == eTcpConnected)
	{
		FD_SET(nSocket, &tmFdSet);
		return true;
	}
	else if (m_Socket.IsValid())
	{
		Close();
		return 0;
	}
	return true;
}

bool CTCPSocket::IsFDSetted(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_nStatus == eTcpConnected)
	{
		return FD_ISSET(nSocket, &tmFdSet);
	}
	return false;
}