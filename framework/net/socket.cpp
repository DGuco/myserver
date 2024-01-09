#include "my_assert.h"
#include "socket.h"
#include "log.h"

CSocket::CSocket()	: m_nSocket(INVALID_SOCKET)
{}

CSocket::~CSocket()
{
	Close();
}

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
		LOG_ERROR("default", "CreateSocket failed with error : {},errormsg : {} \n", errno, strerror(errno));
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

	if (::bind(m_nSocket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) {
		Close();
		m_nSocket = INVALID_SOCKET;
		LOG_ERROR("default", "bind failed with error : {},errormsg : {} \n", errno, strerror(errno));
		return false;
	}
	return 0;
}

bool CSocket::Bind(const char* ipaddr, int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = inet_addr(ipaddr);
	saiAddress.sin_port = htons(port);
	saiAddress.sin_family = AF_INET;

	if (::bind(m_nSocket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) {
		Close();
		m_nSocket = INVALID_SOCKET;
		LOG_ERROR("default", "bind failed error : {},errormsg : {} \n", errno, strerror(errno));
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
		LOG_ERROR("default", "connect {}:{} failed error : {},errormsg : {} \n", ipaddr, port, errno, strerror(errno));
		return SOCKET_ERROR;
	}
	return 0;
}

bool CSocket::Listen()
{
	if (listen(m_nSocket, TCP_BACK_LOG) == SOCKET_ERROR)
	{
		LOG_ERROR("default", "Listen failed error : {},errormsg : {} \n", errno, strerror(errno));
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
}
int CSocket::Read(char* data, int len)
{
	int iRecvedBytes = recv(m_nSocket,data,len,0);
	if (iRecvedBytes == 0)
	{
		CNetAddr tmAddr;
		GetRemoteAddress(tmAddr);
		LOG_ERROR("default", "Socket recved 0 from {}:{} , fd = {}, errno : {},errormsg :{}.", tmAddr.m_szAddr.c_str(),
			tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
		Close();
		return iRecvedBytes;
	}
	else if (errno != OPT_WOULD_BLOCK)
	{
		CNetAddr tmAddr;
		GetRemoteAddress(tmAddr);
		LOG_ERROR("default", "recv error! from {}:{} , fd = {}, errno : {},errormsg :{}.",tmAddr.m_szAddr.c_str(),
			tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
		Close();
		return SOCKET_ERROR;
	}
	return iRecvedBytes;
}

int CSocket::Write(char* data, int len)
{
	int iBytesSent = send(m_nSocket, (const char*)data, len, 0);
	if (iBytesSent < 0 && errno != OPT_WOULD_BLOCK)
	{
		CNetAddr tmAddr;
		GetRemoteAddress(tmAddr);
		LOG_ERROR("default", "send error! to {}:{} , fd = {}, errno : {},errormsg :{}.", tmAddr.m_szAddr.c_str(),
			tmAddr.m_uPort, m_nSocket, errno, strerror(errno));
		Close();
		return SOCKET_ERROR;
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
		LOG_ERROR("default", "SetSendBufSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "GetSendBuffSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return -1;
	}
	return nBuffLen;
}

bool CSocket::SetRecvBufSize(int size)
{
	if (SetSocketOpt(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == SOCKET_ERROR)
	{
		LOG_ERROR("default", "SetRecvBufSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "GetRecvBuffSize error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
#ifdef __LINUX__
	int flags;
	if (m_nSocket == -1)
	{
		return 0;
	}
	if (ioctl(m_nSocket FIONBIO, &flags)
		&& ((flags = fcntl(m_nSocket, F_GETFL, 0)) < 0
		|| fcntl(m_nSocket, F_SETFL, flags | O_NONBLOCK) < 0))
	{
		return 0;
	}
#else
	unsigned long cmd = 1;
	if (ioctlsocket(m_nSocket, FIONBIO, &cmd) == SOCKET_ERROR)
	{
		LOG_ERROR("default", "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "SetReuseAddr error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "SetLinger error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "SetKeepAlive error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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
		LOG_ERROR("default", "SetTcpNoDelay error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
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