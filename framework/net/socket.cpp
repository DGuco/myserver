#include "my_assert.h"
#include "socket.h"
#include "log.h"

CSocket::CSocket()	: m_nSocket(INVALID_SOCKET)
{}

CSocket::~CSocket()
{}

//----------------------------------------------------------------
bool CSocket::Open(int nProtocolFamily, int nType, int nProtocol)
{
	if (m_nSocket != INVALID_SOCKET)
	{
		Close();
	}
	m_nSocket = socket(nProtocolFamily, nType, nProtocol);
	if (m_nSocket == INVALID_SOCKET)
	{
		CACHE_LOG(TCP_ERROR, "CreateSocket failed with error : {},errormsg : {} \n", errno, strerror(errno));
		return false;
	}
	memset(&m_SocketAddr, 0, sizeof(m_SocketAddr));
	m_SocketAddr.sin_family = AF_INET;
	return true;
}

//-----------------------------------------------------------------
void CSocket::Close()
{
	if (m_nSocket != INVALID_SOCKET)
	{
		close(m_nSocket);
		m_nSocket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::Shutdown()
{
	if (m_nSocket != INVALID_SOCKET)
	{
		shutdown(m_nSocket, 2);
		close(m_nSocket);
		m_nSocket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownRead()
{
	if (m_nSocket != INVALID_SOCKET)
	{
		shutdown(m_nSocket, 0);
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownWrite()
{
	if (m_nSocket != INVALID_SOCKET)
	{
		shutdown(m_nSocket, 1);
	}
}

bool CSocket::Bind(int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	saiAddress.sin_port = htons(port);
	saiAddress.sin_family = AF_INET;

	if (::bind(m_nSocket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) 
	{
		Close();
		m_nSocket = INVALID_SOCKET;
		CACHE_LOG(TCP_ERROR, "bind failed with error : {},errormsg : {} \n", errno, strerror(errno));
		return false;
	}
	return true;
}

bool CSocket::Bind(const char* ipaddr, int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = inet_addr(ipaddr);
	saiAddress.sin_port = htons(port);
	saiAddress.sin_family = AF_INET;

	if (::bind(m_nSocket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) 
	{
		Close();
		m_nSocket = INVALID_SOCKET;
		CACHE_LOG(TCP_ERROR, "bind failed error : {},errormsg : {} \n", errno, strerror(errno));
		return false;
	}
	return 0;
}
int CSocket::Conn(const char* ipaddr, int port, bool block)
{
	memset(&m_SocketAddr, 0, sizeof(m_SocketAddr));
	m_SocketAddr.sin_addr.s_addr = inet_addr(ipaddr);
	m_SocketAddr.sin_port = htons(port);
	m_SocketAddr.sin_family = AF_INET;

	m_Host = ipaddr;
	m_nPort = port;
	if (connect(m_nSocket, reinterpret_cast<const sockaddr*>(&m_SocketAddr), sizeof(sockaddr)) == SOCKET_ERROR)
	{
		if (!block && errno == SOCKET_CONNECTING)
		{
			return 1;
		}

		Close();
		m_nSocket = INVALID_SOCKET;
		CACHE_LOG(TCP_ERROR, "connect {}:{} failed error : {},errormsg : {} \n", ipaddr, port, errno, strerror(errno));
		return SOCKET_ERROR;
	}
	return 0;
}

bool CSocket::Listen()
{
	if (listen(m_nSocket, TCP_BACK_LOG) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "Listen failed error : {},errormsg : {} \n", errno, strerror(errno));
		Close();
		return false;
	}
	return true;
}

CSocket CSocket::Accept()
{
	if (!IsValid())
	{
		return CSocket();
	}
	CSocket newSocket;
	int iAddrLength = sizeof(sockaddr_in);
	SOCKET iNewSocketFD = accept(m_nSocket, (struct sockaddr*)&(newSocket.m_SocketAddr), &iAddrLength);
	newSocket.m_nSocket = iNewSocketFD;
	if (newSocket.IsValid())
	{
		newSocket.m_nPort = (ntohs(newSocket.m_SocketAddr.sin_port));
		newSocket.m_Host = (inet_ntoa(newSocket.m_SocketAddr.sin_addr));
	}
	return newSocket;
}

int CSocket::Read(char* data, int len)
{
	/**
	int iRecvBytes;
	while (1)
	{
		iRecvBytes = read(iSocket, pBuf, iLen);
		if (0 < iRecvBytes)
		{
			return iRecvBytes;
		}
		else
		{
			if (0 > iRecvBytes && errno == EINTR)
			{
				continue;
			}
			return iRecvBytes;
		}
	}
	 */
	int iRecvedBytes = recv(m_nSocket,data,len,0);
	if (iRecvedBytes == 0)
	{
		CNetAddr tmAddr;
		GetRemoteAddress(tmAddr);
		CACHE_LOG(TCP_ERROR, "Socket recved 0 from {}:{} , fd = {}, errno : {},errormsg :{}.", tmAddr.m_szAddr.c_str(),
			tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
		return ERR_SOCKE_REMOTE_CLOSED;
	}
	else if (iRecvedBytes < 0)
	{
		if (errno != OPT_WOULD_BLOCK)
		{
			CNetAddr tmAddr;
			GetRemoteAddress(tmAddr);
			CACHE_LOG(TCP_ERROR, "recv error! from {}:{} , fd = {}, errno : {},errormsg :{}.", tmAddr.m_szAddr.c_str(),
				tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
			return ERR_SOCKE_WOULD_BLOCK;
		}
		else
		{
			return ERR_SOCKE_OTHER_ERROR;
		}
	}
	return iRecvedBytes;
}

int CSocket::Write(char* data, int len)
{
	/*
	int iRecvBytes;
	while(1)
	{
		iSendBytes = write(iSocket, pBuf, iPackLen);
		if (iSendBytes == iPackLen)
		{
			return iSendBytes;
		}
		else
		{
			if (0 >= iSendBytes && EINTR == errno)
			{
				continue;
			}

			return iSendBytes;
		}
	}
	在socket编程中，write()和send()都是用来向套接字发送数据的函数，二者最主要的区别在于:
	1.write()函数只是一个基本的系统调用，不包含任何协议的选项，而send()函数包含了更多的参数和选项，可以通过它来设置TCP协议的
	些选项，比如MSG MORE，可以告诉TCP协议在发送数据时不要立即发送掉。
	2.write()函数调用可能会被信号中断(EINTR)，而send()函数则具有更好的可控性，可以通过设置非阻塞模式及使用select/pol来避免被
	中断的情况。总的来说，send()函数相比较于write()函数在更高级别上提供了更多的控制和选项，使其能够更加灵活地满足各种通信需求。
	但在一些简单的应用场景下，write()函数也是一种更为简便、高效的选择,
	3.write函数在写入数据时，如果数据量较大，可能会被分成多次写入，需要多次调用wrte函数。而send函数可以一次性发送所有数据，也
	可以分多次发送。
	无论是使用write()函数还是send()函数，都需要根据具体应用场景来进行选择.在Socket编程中，一般使用send()函数进行数据发送，因为
	它提供了更多的控制方式，并且更适合Socket编程的需求。
	*/
#if defined(__WINDOWS__)
	uint32 flag = MSG_DONTROUTE;
#elif defined(__LINUX__)
	uint32 flag = MSG_NOSIGNAL;
#endif
	int iBytesSent = send(m_nSocket, (const char*)data, len, flag);
	if (iBytesSent < 0)
	{
		if (errno == OPT_WOULD_BLOCK)
		{
			return ERR_SOCKE_WOULD_BLOCK;
		}
		else
		{
			CNetAddr tmAddr;
			GetRemoteAddress(tmAddr);
			CACHE_LOG(TCP_ERROR, "send error! to {}:{} , fd = {}, errno : {},errormsg :{}.", tmAddr.m_szAddr.c_str(),
				tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
			return ERR_SOCKE_OTHER_ERROR;
		}
	}
	return iBytesSent;
}

//-----------------------------------------------------------------
bool CSocket::GetRemoteAddress(CNetAddr & addr) const
{
	if (INVALID_SOCKET == m_nSocket)
		return false;
	sockaddr_in asiAddress;
	int nSize = sizeof(asiAddress);
	memset(&asiAddress, 0, sizeof(asiAddress));
	if (getpeername(m_nSocket, reinterpret_cast<sockaddr *>(&asiAddress), &nSize)) 
	{
		return false;
	}
	addr.m_uPort = (ntohs(asiAddress.sin_port));
	addr.m_szAddr = (inet_ntoa(asiAddress.sin_addr));
	return true;
}

bool CSocket::SetSendBufSize(int size)
{
	if (SetSocketOpt(SOL_SOCKET, SO_SNDBUF,&size, sizeof(size)) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "SetSendBufSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
}

int CSocket::GetSendBuffSize()
{
	int nBuffLen = 0;
	int nSize = sizeof(nBuffLen);
	if (GetSocketOpt(SOL_SOCKET, SO_SNDBUF, &nBuffLen, &nSize) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "GetSendBuffSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return -1;
	}
	return nBuffLen;
}

bool CSocket::SetRecvBufSize(int size)
{
	if (SetSocketOpt(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "SetRecvBufSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
}

int CSocket::GetRecvBuffSize()
{
	int nBuffLen = 0;
	int nSize = sizeof(nBuffLen);
	if (GetSocketOpt(SOL_SOCKET, SO_RCVBUF, &nBuffLen, &nSize) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "GetRecvBuffSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return -1;
	}
	return nBuffLen;
}

SOCKET CSocket::GetSocket() const
{
	return m_nSocket;
}

int CSocket::SetSocketOpt(int sol, int type, const void* value, int size)
{
#ifdef __LINUX__
	return setsockopt(m_nSocket, sol, type,value, size);
#else
	return setsockopt(m_nSocket, sol, type, (const char*)value, size);
#endif
}

int CSocket::GetSocketOpt(int sol, int type,void* value, int* size)
{
#ifdef __LINUX__
	return getsockopt(m_nSocket, sol, type, value, (socklen_t*)size);
#else
	return getsockopt(m_nSocket, sol, type, (char*)value, (int*)size);
#endif
}

bool CSocket::SetSocketNoBlock()
{
	if (m_nSocket == -1)
	{
		return 0;
	}
#ifdef __LINUX__
	int flags;
	if (ioctl(m_nSocket FIONBIO, &flags)
		&& ((flags = fcntl(m_nSocket, F_GETFL, 0)) < 0
		|| fcntl(m_nSocket, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		CACHE_LOG(TCP_ERROR, "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return 0;
	}
#else
	unsigned long cmd = 1;
	if (ioctlsocket(m_nSocket, FIONBIO, &cmd) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
#endif
}

bool CSocket::SetReuseAddr()
{
	int iReusePortFlag = 1;
	if (SetSocketOpt(SOL_SOCKET, SO_REUSEADDR, &iReusePortFlag, sizeof(iReusePortFlag)) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "SetReuseAddr error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
}

bool CSocket::IsReuseAddr()
{
	int resuaddr = 0;
	INT nSize = sizeof(resuaddr);
	GetSocketOpt(SOL_SOCKET, SO_REUSEADDR, &resuaddr, &(nSize));
	return resuaddr == 1;
}

bool CSocket::SetLinger(int lingertime)
{
	struct linger ling = { 0, 0 };
	if (lingertime > 0)
	{
		ling.l_onoff = 1;
		ling.l_linger = lingertime;
	}
	if (SetSocketOpt(SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == INVALID_SOCKET)
	{
		CACHE_LOG(TCP_ERROR, "SetLinger error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;

}

int CSocket::GetLinger()
{
	struct linger ling = { 0, 0 };
	int len = sizeof(ling);
	GetSocketOpt(SOL_SOCKET, SO_LINGER, &ling, &(len));
	return ling.l_linger;
}

bool CSocket::SetKeepAlive()
{
	int nKeepAlive = 1;
	if (SetSocketOpt(SOL_SOCKET, SO_KEEPALIVE, &nKeepAlive, sizeof(nKeepAlive)) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "SetKeepAlive error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
}

bool CSocket::IsKeepAlive()
{
	int nKeepAlive = 0;
	int nSize = sizeof(nKeepAlive);
	GetSocketOpt(SOL_SOCKET, SO_KEEPALIVE, &nKeepAlive, &(nSize));
	return nKeepAlive == 1;
}

bool CSocket::SetTcpNoDelay()
{
	int nNoDelay = 1;
	if (SetSocketOpt(IPPROTO_TCP, TCP_NODELAY, &nNoDelay, sizeof(nNoDelay)) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "SetTcpNoDelay error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	return true;
}

bool CSocket::IsTcpNoDelay()
{
	int nNoDelay = 0;
	int nSize = sizeof(nNoDelay);
	GetSocketOpt(IPPROTO_TCP, TCP_NODELAY, &nNoDelay, &(nSize));
	return nNoDelay == 1;
}

unsigned int  CSocket::CanReadLen()
{
	if (m_nSocket == -1)
	{
		return 0;
	}
#ifdef __LINUX__
	int available = 0;
	if (ioctl(m_nSocket FIONREAD, &available) < 0)
	{
		CACHE_LOG(TCP_ERROR, "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
	}
	return available;
#else
	unsigned long available;
	if (ioctlsocket(m_nSocket, FIONREAD, &available) == SOCKET_ERROR)
	{
		CACHE_LOG(TCP_ERROR, "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
	}
	return available;
#endif
}

bool CSocket::IsValid()
{
	return m_nSocket != INVALID_SOCKET;
}

bool CSocket::IsSocketError()
{
	int nError = 0;
	int nSize = sizeof(nError);
	if (GetSocketOpt(SOL_SOCKET, SO_ERROR, &nError, &(nSize)) == SOCKET_ERROR )
	{
		return true;
	}
	return nError == 1;
}

const CString<ADDR_LENGTH>& CSocket::GetHost() const
{
	return m_Host;
}

int	CSocket::GetPort() const
{
	return m_nPort;
}