#include "tcp_socket.h"
#include "common_def.h"
#include "base.h"
#include "log.h"

CTCPSocket::CTCPSocket(unsigned int RecvBufLen_, unsigned int SendBufLen_)
{
	m_nStatus = eSocketClosed;
	m_pReadBuff = new CByteBuff(RecvBufLen_);
	m_pWriteBuff = new CByteBuff(SendBufLen_);
}

CTCPSocket::CTCPSocket(CSocket socket,unsigned int RecvBufLen_, unsigned int SendBufLen_)
{
	m_Socket = socket;
	if (m_Socket.IsValid())
	{
		m_nStatus = eSocketOpen;
	}
	else
	{
		m_nStatus = eSocketClosed;
	}
	m_pReadBuff = new CByteBuff(RecvBufLen_);
	m_pWriteBuff = new CByteBuff(SendBufLen_);
}

CTCPSocket::~CTCPSocket()
{
	m_pReadBuff.Free();
	m_pWriteBuff.Free();
	m_Socket.Close();
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
	m_nStatus = eSocketOpen;
	if (!m_Socket.SetRecvBufSize(TCP_SERVER_RECV_BUFF_LEN))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(TCP_SERVER_SEND_BUFF_LEN))
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
		m_nStatus = eSocketConnecting;
		//非阻塞连接中，检查是否连接成功
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_nStatus = eSocketConnected;
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
		DISK_LOG(ERROR_DISK, "select error({}:{})", errno, strerror(errno));
		Close();
		return -2;
	}

	if (n == 0)
	{
		return 0;
	}

	if (!FD_ISSET(nSocketFd, &readfds) && !FD_ISSET(nSocketFd, &writefds))
	{
		DISK_LOG(ERROR_DISK, "socket {} can't read and write", nSocketFd);
		Close();
		return -3;
	}

	if (m_Socket.IsSocketError())
	{
		DISK_LOG(ERROR_DISK, "connect failure on SOCKET {} error:{} msg:{}.", m_Socket.GetSocket(), errno, strerror(errno));
		Close();
		return -4;
	}
	m_nStatus = eSocketConnected;
	return 0;
}

int CTCPSocket::Close(bool now)
{
	if (now)
	{
		m_Socket.Close();
		m_nStatus = eSocketClosed;
	}
	else
	{
		m_nStatus = eSocketClosing;
	}
	return 0;
}

bool CTCPSocket::IsValid()
{
	return m_Socket.IsValid();
}

SOCKET CTCPSocket::GetSocketFD()
{
	return m_Socket.GetSocket();
}

CSocket& CTCPSocket::GetSocket()
{
	return m_Socket;
}

u_short CTCPSocket::GetStatus()
{
	return (u_short)m_nStatus;
}

int CTCPSocket::Recv()
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
	}
	return ERR_SEND_OK;
}

int CTCPSocket::Flush()
{
	if (!m_Socket.IsValid())
	{
		return ERR_SEND_NOSOCK;
	}

	int retCode = ERR_SEND_OK;
	if (m_pWriteBuff->CanReadLen() > 0)
	{
		retCode = m_pWriteBuff->Send(m_Socket);
	}

	//没有数据了，索引都归0吧
	if (m_pWriteBuff->CanReadLen() == 0)
	{
		m_pWriteBuff->Clear();
	}

	if (retCode == ERR_RECV_WOULD_BLOCK)
	{
		retCode = ERR_RECV_OK;
	}
	return retCode;
}

bool CTCPSocket::AddToFDSet(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (m_Socket.IsValid())
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
	if (m_Socket.IsValid())
	{
		return FD_ISSET(nSocket, &tmFdSet);
	}
	return false;
}

//
CSafePtr<CByteBuff> CTCPSocket::GetReadBuff()
{
	return m_pReadBuff;
}

//
CSafePtr<CByteBuff> CTCPSocket::GetSendBuff()
{
	return m_pWriteBuff;
}