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
	// 	int iVal = 1;
	// 	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&iVal), sizeof(iVal))) 
	// 	{
	// 		LOG_ERROR("default", "setsockopt failed with error : {},errormsg : {} \n", errno, strerror(errno));
	// 	}
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

bool CSocket::Bind(std::string host, int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = inet_addr(host.c_str());
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
bool CSocket::Conn(std::string host, int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = inet_addr(host.c_str());
	saiAddress.sin_port = htons(port);
	saiAddress.sin_family = AF_INET;

	if (connect(m_nSocket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR)
	{
		if (m_bBlock && errno == SOCKET_CONNECTING)
		{
			return true;
		}

		Close();
		m_nSocket = INVALID_SOCKET;
		LOG_ERROR("default", "connect {}:{} failed error : {},errormsg : {} \n", host, port, errno, strerror(errno));
		return false;
	}
	return true;
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
	int iFlags;
	iFlags = fcntl(m_nSocket, F_GETFL, 0);
	iFlags |= O_NONBLOCK;
	iFlags |= O_NDELAY;
	fcntl(m_nSocket, F_SETFL, iFlags);
	m_bBlock = true;
	return true;
#else
	unsigned long cmd = 1;
	if (ioctlsocket(m_nSocket, FIONBIO, &cmd) == SOCKET_ERROR)
	{
		LOG_ERROR("default", "ioctlsocket error , fd = {}, errno : {},errormsg :{}.", m_nSocket, errno, strerror(errno));
		return false;
	}
	m_bBlock = true;
	return true;
#endif
}

bool CSocket::IsValid()
{
	return m_nSocket != INVALID_SOCKET;
}