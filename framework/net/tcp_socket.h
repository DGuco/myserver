/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include "safe_pointer.h"
#include "byte_buff.h"
#include "socket.h"

enum eTcpStatus
{
	eTcpClosed = 0,
	eTcpCreated = 1,
	eTcpConnecting = 2,
	eTcpConnected = 3,
	eTcpRegisting = 4,
	eTcpRegistered = 5,
	eTcpListened = 6,
};

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
class CTCPSocket
{
public:
	//
	CTCPSocket();
	//
	~CTCPSocket();
	//获取socketid
	SOCKET GetSocketFD();
	//获取连接状态
	eTcpStatus GetStatus();
	//连接
	int ConnectTo(char* szIPAddr, u_short unPort,bool block = true);
	//连接
	int ConnectTo(u_long ulIPNetAddr, u_short unPort,bool block = true);
	//检查非阻塞连接是否连接成功
	int CheckConnectedOk();
	//读取数据
	int RecvData();
	//把数据写到缓冲区准备发送
	int Write(BYTE* pCode, msize_t nCodeLength);
	//获取读缓冲区中的一段信息
	int GetOneCode(unsigned short& nCodeLength, BYTE* pCode);
	//添加socket到fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//是否添加到fdset中
	bool IsFDSetted(fd_set& pCheckSet);
	//是否连接成功
	int CheckConnectedOk();
	//Init tcp Server
	int InitTcpServer(const char* ip,int port);
	//关闭
	int Close();
public:
	//是否可读写
	virtual CanReadWrite() { return 0};
protected:
	CSocket					m_Socket;	     //Socket 描述符
	int						m_iStatus;	     //连接状态
	SafePointer<CByteBuff>	m_pReadBuff;     //读缓冲
	SafePointer<CByteBuff>	m_pWriteBuff;    //写缓冲
};


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPSocket<RecvBufLen_, SendBufLen_>::CTCPSocket()
{
	m_iStatus = eTcpClosed;
	m_pReadBuff = new CByteBuff(RecvBufLen_);
	m_pWriteBuff = new CByteBuff(SendBufLen_);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPSocket<RecvBufLen_, SendBufLen_>::~CTCPSocket()
{
	DELETE(m_pReadBuff);
	DELETE(m_pWriteBuff);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::ConnectTo(char* szIPAddr, u_short unPort, bool block)
{
	m_Socket.Open();
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	m_iStatus = eTcpCreated;
	if (!m_Socket.SetRecvBufSize(RecvBufLen_))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(SendBufLen_))
	{
		Close();
		return -1;
	}

	if (!block)
	{
		m_Socket.SetSocketNoBlock();
	}

	sockaddr_in stTempAddr;
	memset((void*)&stTempAddr, 0, sizeof(sockaddr_in));
	stTempAddr.sin_family = AF_INET;
	stTempAddr.sin_port = htons(unPort);
	stTempAddr.sin_addr.s_addr = inet_addr(szIPAddr);
	int nConRet = m_Socket.Conn(stTempAddr, sizeof(stTempAddr), block);
	if (nConRet == SOCKET_ERROR)
	{
		Close();
		return -2;
	}
	else if (nConRet == 1)
	{
		m_iStatus = eTcpConnecting;
		//非阻塞连接中，检查是否连接成功
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::ConnectTo(u_long ulIPNetAddr, u_short unPort, bool block)
{
	m_Socket.Open();
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	m_iStatus = eTcpCreated;
	if (!m_Socket.SetRecvBufSize(RecvBufLen_))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(SendBufLen_))
	{
		Close();
		return -1;
	}

	if (!block)
	{
		m_Socket.SetSocketNoBlock();
	}

	sockaddr_in stTempAddr;
	memset((void*)&stTempAddr, 0, sizeof(sockaddr_in));
	stTempAddr.sin_family = AF_INET;
	stTempAddr.sin_port = htons(unPort);
	stTempAddr.sin_addr.s_addr = ulIPNetAddr;
	int nConRet = m_Socket.Conn(stTempAddr, sizeof(stTempAddr), block);
	if (nConRet == SOCKET_ERROR)
	{
		Close();
		return -2;
	}
	else if(nConRet == 1)
	{
		m_iStatus = eTcpConnecting;
		//非阻塞连接中，检查是否连接成功
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::CheckConnectedOk()
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
		LOG_ERROR("default", "connect failure on SOCKET {} error:{} msg:{}.", m_nSocket, errno, strerror(errno))
		Close();
		return -4;
	}
	m_iStatus = eTcpConnected;
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::InitTcpServer(const char* ip, int port)
{
	m_Socket.Open();
	if (!m_Socket.IsValid()) return -1;
	m_iStatus = eTcpCreated;

	//允许套接口和一个已在使用中的地址捆绑
	if (!m_Socket.SetReuseAddr())  return -1;

	bool bRet = 0;
	if (ip != NULL)
	{
		bRet = m_Socket.Bind(port);
	}
	else
	{
		bRet = m_Socket.Bind(ip,port);
	}

	if (!bRet) return -1;

	if (!m_Socket.Listen()) return -1;

	m_Socket.SetSocketNoBlock();
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::Close()
{
	m_Socket.Close();
	m_iStatus = eTcpClosed;
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
SOCKET CTCPSocket<RecvBufLen_, SendBufLen_>::GetSocketFD()
{
	return m_Socket.GetSocket();
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
eTcpStatus CTCPSocket<RecvBufLen_, SendBufLen_>::GetStatus()
{
	return m_iStatus;
}

// 返回值：-1 ：Socket状态错误；-2 ：接收缓冲区已满；-3 ：对端断开连接；-4 ：接收错误
template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::RecvData()
{
	if (!CanReadWrite())
	{
		return ERR_RECV_NOT_READY;
	}
	int nRetCode =  m_pReadBuff->Recv(m_Socket);
	if (nRetCode == ERR_RECV_WOULD_BLOCK)
	{
		nRetCode = ERR_RECV_OK;
	}
	return nRetCode;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::Write(BYTE* pCode, msize_t nCodeLength)
{
	if (!m_Socket.IsValid())
	{
		return ERR_SEND_NOSOCK;
	}

	if (!pCode)
	{
		return ERR_SEND_NODATA;
	}

	if (!CanReadWrite())
	{
		return ERR_SEND_NOT_READY;
	}

	int retCode = ERR_SEND_OK;
	//有残留数据待发送
	if (m_pWriteBuff->CanReadLen() > 0)
	{
		int retCode = m_pWriteBuff->Send(m_Socket);
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

	//把本次的数据写入缓冲区
	SOCKET nTmSocket = m_Socket.GetSocket();
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
	return ERR_SEND_OK;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::GetOneCode(unsigned short& nCodeLength, BYTE* pCode, eByteMode emByte)
{

}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
bool CTCPSocket<RecvBufLen_, SendBufLen_>::AddToFDSet(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_iStatus == eTcpConnected)
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

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
bool CTCPSocket<RecvBufLen_, SendBufLen_>::IsFDSetted(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_iStatus == eTcpConnected)
	{
		return  = FD_ISSET(nSocket, tmFdSet);
	}
	return false;
}

#endif //__TCP_CLIENT_H__
