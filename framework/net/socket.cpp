#include "my_assert.h"
#include "socket.h"
#include "log.h"

CSocket::CSocket()	: m_Socket(INVALID_SOCKET)
{}

CSocket::~CSocket()
{
	Close();
}

//----------------------------------------------------------------
bool CSocket::Open(int nProtocolFamily, int nType, int nProtocol)
{
	m_Socket = socket(nProtocolFamily, nType, nProtocol);
	if (m_Socket == INVALID_SOCKET)
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
	if (m_Socket != INVALID_SOCKET)
	{
		close(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::Shutdown()
{
	if (m_Socket != INVALID_SOCKET)
	{
		shutdown(m_Socket, 2);
		close(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownRead()
{
	if (m_Socket != INVALID_SOCKET)
	{
		shutdown(m_Socket, 0);
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownWrite()
{
	if (m_Socket != INVALID_SOCKET)
	{
		shutdown(m_Socket, 1);
	}
}

bool CSocket::Bind(int port)
{
	sockaddr_in saiAddress;
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	saiAddress.sin_port = htons(port);
	saiAddress.sin_family = AF_INET;

	if (::bind(m_Socket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) {
		Close();
		m_Socket = INVALID_SOCKET;
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

	if (::bind(m_Socket, reinterpret_cast<const sockaddr*>(&saiAddress), sizeof(sockaddr)) == SOCKET_ERROR) {
		Close();
		m_Socket = INVALID_SOCKET;
		LOG_ERROR("default", "bind failed error : {},errormsg : {} \n", errno, strerror(errno));
		return false;
	}
	return 0;
}

bool CSocket::Listen()
{
	if (listen(m_Socket, TCP_BACK_LOG) == SOCKET_ERROR)
	{
		LOG_ERROR("default", "Listen failed error : {},errormsg : {} \n", errno, strerror(errno));
		Close();
		return false;
	}
	return true;
}

//-----------------------------------------------------------------
bool CSocket::GetRemoteAddress(CNetAddr & addr) const
{
	if (INVALID_SOCKET == m_Socket)
		return false;
	sockaddr_in asiAddress;
	int nSize = sizeof(asiAddress);
	memset(&asiAddress, 0, sizeof(asiAddress));
	if (getpeername(m_Socket, reinterpret_cast<sockaddr *>(&asiAddress), &nSize)) 
	{
		return false;
	}
	addr.m_uPort = (ntohs(asiAddress.sin_port));
	addr.m_szAddr = (inet_ntoa(asiAddress.sin_addr));
	return true;
}

SOCKET CSocket::GetSocket() const
{
	return m_Socket;
}