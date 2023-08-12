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
};

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
class CTCPClient
{
public:
	//
	CTCPClient();
	//
	~CTCPClient();
	//获取socketid
	SOCKET GetSocketFD();
	//获取连接状态
	eTcpStatus GetStatus();
	//连接
	int ConnectTo(char* szIPAddr, u_short unPort,bool block = true);
	//连接
	int ConnectTo(u_long ulIPNetAddr, u_short unPort,bool block = true);
	//检查非阻塞连接是否连接成功
	int CheckNoblockConnecting();
	//读取数据
	int RecvData();
	//
	int GetOneCode(unsigned short& nCodeLength, BYTE* pCode, eByteMode emByte = use_host_byte);
	//
	int GetOneHttpCode(int& nCodeLength, BYTE* pCode);
	//
	int GetOneCode32(int& iCodeLength, BYTE* pCode);
	//
	int SendOneCode(unsigned short nCodeLength, BYTE* pCode);
	//
	int SendOneCode32(int nCodeLength, BYTE* pCode);
	//
	bool AddToFDSet(fd_set& pCheckSet);
	//
	bool IsFDSetted(fd_set& pCheckSet);
	//
	int HasReserveData();
	//
	int CleanReserveData();
	//
	int PrintfSocketStat();
	//
	int CheckNoblockConnecting();
	int Close();
protected:
	CSocket					m_Socket; //Socket 描述符
	int						m_iStatus;	 //连接状态
	SafePointer<CByteBuff>	m_ReadBuff;  //读缓冲
	SafePointer<CByteBuff>	m_WriteBuff; //写缓冲
};


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPClient<RecvBufLen_, SendBufLen_>::CTCPClient()
{
	m_iStatus = eTcpClosed;
	m_ReadBuff = new CByteBuff(RecvBufLen_);
	m_WriteBuff = new CByteBuff(SendBufLen_);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPClient<RecvBufLen_, SendBufLen_>::~CTCPClient()
{
	DELETE(m_ReadBuff);
	DELETE(m_WriteBuff);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::ConnectTo(char* szIPAddr, u_short unPort, bool block)
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
		CheckNoblockConnecting();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::ConnectTo(u_long ulIPNetAddr, u_short unPort, bool block)
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
		CheckNoblockConnecting();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::CheckNoblockConnecting()
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
int CTCPClient<RecvBufLen_, SendBufLen_>::Close()
{
	m_Socket.Close();
	m_iStatus = eTcpClosed;
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
SOCKET CTCPClient<RecvBufLen_, SendBufLen_>::GetSocketFD()
{
	return m_Socket.GetSocket();
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
eTcpStatus CTCPClient<RecvBufLen_, SendBufLen_>::GetStatus()
{
	return m_iStatus;
}

// 返回值：-1 ：Socket状态错误；-2 ：接收缓冲区已满；-3 ：对端断开连接；-4 ：接收错误
template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::RecvData()
{
	int iRecvedBytes = 0;
	int iTempRet = 0;

	char szPeerAddr[32];
	sockaddr_in stPeerAddr;
	memset(&stPeerAddr, 0, sizeof(sockaddr_in));
	socklen_t iPeerAddrLen = sizeof(stPeerAddr);

	if (m_iSocketFD < 0 || m_iStatus != tcs_connected)
	{

		LOG_ERROR("default", "RecvData Failed : m_iSocketFD(%d), m_iStatus(%d).",
			m_iSocketFD, m_iStatus);
		return ERR_RECV_NOSOCK;
	}

	if (m_iReadEnd == m_iReadBegin)
	{
		m_iReadBegin = 0;
		m_iReadEnd = 0;
	}

	do
	{

		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			LOG_ERROR("default", "The recv buffer is full now(%d, %d), stop recv data, fd = %d.", m_iReadBegin, m_iReadEnd, m_iSocketFD);
			iTempRet = ERR_RECV_NOBUFF;
			break;
		}

		iRecvedBytes = recv(m_iSocketFD, &m_abyRecvBuffer[m_iReadEnd],
			sizeof(m_abyRecvBuffer) - m_iReadEnd, 0);
		if (iRecvedBytes > 0)
		{
			m_iReadEnd += iRecvedBytes;
		}
		else if (iRecvedBytes == 0)
		{
			getpeername(m_iSocketFD, (struct sockaddr*)&stPeerAddr, &iPeerAddrLen);
			SockAddrToString(&stPeerAddr, szPeerAddr);
			LOG_ERROR("default", "recv error! RecvedBytes(%d) from %s , fd = %d, errno = %d.", iRecvedBytes, szPeerAddr, m_iSocketFD, errno);
			Close();
			iTempRet = ERR_RECV_REMOTE;
			break;
		}
		else if (errno != EAGAIN)
		{
			getpeername(m_iSocketFD, (struct sockaddr*)&stPeerAddr, &iPeerAddrLen);
			SockAddrToString(&stPeerAddr, szPeerAddr);
			LOG_ERROR("default", "Error in read, %s, socket fd = %d, remote site %s.", strerror(errno), m_iSocketFD, szPeerAddr);
			Close();
			iTempRet = ERR_RECV_FALIED;
			break;
		}
	} while (iRecvedBytes > 0);

	return iTempRet;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::GetOneCode(unsigned short& nCodeLength, BYTE* pCode, eByteMode emByte)
{
	unsigned short shMaxBufferLen = nCodeLength;
	int iDataLength = 0;
	unsigned short nTempLength;

	if (!pCode)
	{
		LOG_ERROR("default", "GetOneCode Failed : pCode is NULL.");
		return -1;
	}

	//RecvData();

	iDataLength = m_iReadEnd - m_iReadBegin;

	if (iDataLength <= 0)
	{
		//LOG_ERROR("default", "GetOneCode Failed : iDataLength(%d) <= 0.", iDataLength);
		return 0;
	}

	if (iDataLength < (int)sizeof(short))
	{
		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			//memcpy((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			memmove((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		LOG_INFO("default", "GetOneCode Failed : iDataLength(%d) < sizeof(short).", iDataLength);
		return 0;
	}

	if (emByte == use_network_byte)
	{
		nTempLength = ntohs(*((unsigned short*)&m_abyRecvBuffer[m_iReadBegin]));
	}
	else
	{
		nTempLength = /*ntohs*/(*((unsigned short*)&m_abyRecvBuffer[m_iReadBegin]));
	}

	if (nTempLength == 0 || nTempLength + sizeof(short) > sizeof(m_abyRecvBuffer))
	{
		m_iReadBegin = m_iReadEnd = 0;
		Close();
		LOG_ERROR("default", "GetOneCode Failed : nTempLength(%d), m_abyRecvBuffer length(%d).",
			nTempLength, sizeof(m_abyRecvBuffer));
		return -2;
	}

	if (iDataLength < (int)nTempLength)
	{
		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			//memcpy((void *)&m_abyRecvBuffer[0], (const void *)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			memmove((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		//LOG_ERROR("default", "GetOneCode Failed : iDataLength(%d) < nTempLength(%d).",
		//	iDataLength, nTempLength);
		return 0;
	}


	int iTempRet = 1;
	nCodeLength = nTempLength;

	if (nCodeLength < shMaxBufferLen)
	{
		memcpy((void*)pCode, (const void*)&m_abyRecvBuffer[m_iReadBegin], nCodeLength);
	}
	else
	{
		iTempRet = -2;
		Close();
		LOG_ERROR("default", "GetOneCode Failed : nCodeLength(%d) < shMaxBufferLen(%d).",
			nCodeLength, shMaxBufferLen);
		return iTempRet;
	}

	m_iReadBegin += nTempLength;

	if (m_iReadBegin == m_iReadEnd)
	{
		m_iReadBegin = m_iReadEnd = 0;
	}

	return iTempRet;
}


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::GetOneHttpCode(int& nCodeLength, BYTE* pCode)
{
	int shMaxBufferLen = nCodeLength;
	int iDataLength = 0;
	unsigned int nTempLength;

	if (!pCode)
	{
		return -1;
	}

	iDataLength = m_iReadEnd - m_iReadBegin;

	if (iDataLength <= 0)
	{
		return 0;
	}

	const char* LENTH_TOKEN = "Content-Length";
	const char* HEAD_BODY_SPLITTER = "\n\n";

	char* pSpliter = strstr((const char*)&m_abyRecvBuffer[m_iReadBegin], HEAD_BODY_SPLITTER);
	if (pSpliter != NULL)
	{
		char* pToken = strstr((const char*)&m_abyRecvBuffer[m_iReadBegin], LENTH_TOKEN);
		if (pToken != NULL)
		{
			char* pStart = strstr(pToken + strlen(LENTH_TOKEN), ":");
			char* pEnd = NULL;

			if (pStart != NULL) pEnd = strstr(pStart, "\n");

			if (pStart != NULL && pEnd != NULL && pStart < pSpliter && pEnd <= pSpliter)
			{
				nTempLength = atoi(pStart + 1) + (pSpliter - (char*)m_abyRecvBuffer) + strlen(HEAD_BODY_SPLITTER);
			}
			else
			{
				// 协议格式不正确
				m_iReadBegin = m_iReadEnd = 0;
				Close();
				return -1;
			}
		}
		else
		{
			// 协议格式不正确
			m_iReadBegin = m_iReadEnd = 0;
			Close();
			return -1;
		}
	}
	else
	{
		if (iDataLength > 4096)
		{
			// 协议格式不正确
			m_iReadBegin = m_iReadEnd = 0;
			Close();
			return -1;
		}

		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			memcpy((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		return 0;
	}

	if (nTempLength == 0 || nTempLength > sizeof(m_abyRecvBuffer))
	{
		m_iReadBegin = m_iReadEnd = 0;
		Close();
		return -2;
	}

	if (iDataLength < nTempLength)
	{
		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			memcpy((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		return 0;
	}


	int iTempRet = 1;
	nCodeLength = nTempLength;

	if (nCodeLength < shMaxBufferLen)
	{
		memcpy((void*)pCode, (const void*)&m_abyRecvBuffer[m_iReadBegin], nCodeLength);
	}
	else
	{
		iTempRet = -2;
		Close();
		return iTempRet;
	}

	m_iReadBegin += nTempLength;

	if (m_iReadBegin == m_iReadEnd)
	{
		m_iReadBegin = m_iReadEnd = 0;
	}

	return iTempRet;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::GetOneCode32(int& iCodeLength, BYTE* pCode)
{
	int iMaxBufferLen = iCodeLength;
	int iDataLength = 0;
	int iTempLength = 0;

	if (!pCode)
	{
		return -1;
	}

	//RecvData();

	iDataLength = m_iReadEnd - m_iReadBegin;

	if (iDataLength <= 0)
	{
		return 0;
	}

	if (iDataLength < sizeof(int))
	{
		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			memcpy((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		return 0;
	}

	iTempLength = (int)/*ntohl*/(*((int*)&m_abyRecvBuffer[m_iReadBegin]));

	if (iTempLength <= 0 || iTempLength + sizeof(int) > sizeof(m_abyRecvBuffer))
	{
		m_iReadBegin = m_iReadEnd = 0;
		Close();
		return -2;
	}

	if (iDataLength < iTempLength)
	{
		if (m_iReadEnd == sizeof(m_abyRecvBuffer))
		{
			memcpy((void*)&m_abyRecvBuffer[0], (const void*)&m_abyRecvBuffer[m_iReadBegin], iDataLength);
			m_iReadBegin = 0;
			m_iReadEnd = iDataLength;
		}

		return 0;
	}


	int iTempRet = 1;
	iCodeLength = iTempLength;

	if (iCodeLength < iMaxBufferLen)
	{
		memcpy((void*)pCode, (const void*)&m_abyRecvBuffer[m_iReadBegin], iCodeLength);
	}
	else
	{
		iTempRet = -2;
		Close();
		return iTempRet;
	}

	m_iReadBegin += iTempLength;

	if (m_iReadBegin == m_iReadEnd)
	{
		m_iReadBegin = m_iReadEnd = 0;
	}

	return iTempRet;
}


// 返回值说明：-1：参数错误或状态非法；-2：发送缓冲区满；-3：发送系统错误；
template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::SendOneCode(unsigned short nCodeLength, BYTE* pCode)
{
	int iBytesSent = 0;
	int iBytesLeft = nCodeLength;
	BYTE* pbyTemp = NULL;
	int iTempRet = 0;

	if (!pCode)
	{
		LOG_ERROR("default", "SendOneCode Failed : pCode is NULL.");
		return ERR_SEND_NOSOCK;
	}

	if (m_iSocketFD < 0 || m_iStatus != tcs_connected)
	{
		LOG_ERROR("default", "SendOneCode Failed : m_iSocketFD(%d), m_iStatus(%d).",
			m_iSocketFD, m_iStatus);
		return ERR_SEND_NOSOCK;
	}

	// 首先检查是否有滞留数据
	iBytesLeft = m_iPostEnd - m_iPostBegin;
	pbyTemp = &(m_abyPostBuffer[m_iPostBegin]);
	while (iBytesLeft > 0)
	{
		iBytesSent = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);

		if (iBytesSent > 0)
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
			m_iPostBegin += iBytesSent;
		}

		if (iBytesSent < 0 && errno != EAGAIN)

		{
			m_iStatus = tcs_error;
			iTempRet = ERR_SEND_FAILED;
			LOG_ERROR("default", "SendOneCode Failed : part1 : iBytesSent(%d), errno(%d : %s).",
				iBytesSent, errno, strerror(errno));
			break;
		}
	}

	if (iBytesLeft == 0)
	{
		// 滞留数据发送成功，则继续发送本次提交的数据
		m_iPostBegin = m_iPostEnd = 0;
	}
	else
	{
		// 否则，直接返回
		if (iBytesLeft < 0)
		{
			iTempRet = ERR_SEND_UNKOWN;
		}
		else
		{
			// Socket发送缓冲区满，则将剩余的数据放到缓存中
			// 为了效率考虑,只在m_iPostBegin大于1M时才做处理
			if (m_iPostBegin > (1024 * 1024))
			{
				memmove((void*)&(m_abyPostBuffer[0]), (const void*)pbyTemp, iBytesLeft);
				m_iPostBegin = 0;
				m_iPostEnd = iBytesLeft;
			}

			if ((m_iPostEnd + nCodeLength) <= (int)(sizeof(m_abyPostBuffer) - 1))
			{
				// 成功拷贝进缓冲区也算成功
				int iBytesLeftNow = nCodeLength;
				BYTE* pbyTempNow = pCode;
				memcpy((void*)&(m_abyPostBuffer[m_iPostEnd]), (const void*)pbyTempNow, iBytesLeftNow);
				m_iPostEnd += iBytesLeftNow;
			}
			else
			{
				// 数据丢失
				LOG_ERROR("default", "SendOneCode Failed : lost data ! m_iPostBegin(%d), m_iPostEnd(%d), iBytesLeft(%d), lostLen(%d)!",
					m_iPostBegin, m_iPostEnd, iBytesLeft, nCodeLength);
				iTempRet = ERR_SEND_NOBUFF;
			}
		}
		return iTempRet;
	}

	// 发送本次提交的数据
	iBytesLeft = nCodeLength;
	pbyTemp = pCode;

	while (iBytesLeft > 0)
	{
		iBytesSent = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);

		if (iBytesSent > 0)
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
		}


		if (iBytesSent < 0 && errno != EAGAIN)
		{
			m_iStatus = tcs_error;
			iTempRet = ERR_SEND_FAILED;
			LOG_ERROR("default", "SendOneCode Failed : part3 : iBytesSent(%d), iBytesLeft(%d), errno(%d : %s).",
				iBytesSent, iBytesLeft, errno, strerror(errno));
			break;
		}

		else if (iBytesSent < 0)
		{
			// Socket发送缓冲区满，则将剩余的数据放到缓存中
			memcpy((void*)&(m_abyPostBuffer[m_iPostEnd]), (const void*)pbyTemp, iBytesLeft);
			m_iPostEnd += iBytesLeft;
			iTempRet = ERR_SEND_NOBUFF;
			LOG_ERROR("default", "SendOneCode Failed : part4 : iBytesSent(%d), iBytesLeft(%d), sendtotallen(%d), errno(%d : %s).",
				iBytesSent, iBytesLeft, nCodeLength, errno, strerror(errno));
			break;
		}

	}

	return iTempRet;
}

// 返回值说明：-1：参数错误或状态非法；-2：发送缓冲区满；-3：发送系统错误；
template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::SendOneCode32(int iCodeLength, BYTE* pCode)
{
	int iBytesSent = 0;
	int iBytesLeft = iCodeLength;
	BYTE* pbyTemp = NULL;
	int iTempRet = 0;

	if (!pCode)
	{
		return ERR_SEND_NOSOCK;
	}

	if (m_iSocketFD < 0 || m_iStatus != tcs_connected)
	{
		return ERR_SEND_NOSOCK;
	}

	// 首先检查是否有滞留数据
	iBytesLeft = m_iPostEnd - m_iPostBegin;
	pbyTemp = &(m_abyPostBuffer[m_iPostBegin]);
	while (iBytesLeft > 0)
	{
		iBytesSent = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);

		if (iBytesSent > 0)
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
			m_iPostBegin += iBytesSent;
		}


		if (iBytesSent < 0 && errno != EAGAIN)

		{
			m_iStatus = tcs_error;
			iTempRet = ERR_SEND_FAILED;
			break;
		}
		else if (iBytesSent < 0)
		{
			iTempRet = ERR_SEND_NOBUFF;
			break;
		}
	}

	if (iBytesLeft == 0)
	{
		// 滞留数据发送成功，则继续发送本次提交的数据
		m_iPostBegin = m_iPostEnd = 0;
	}
	else
	{
		// 否则，直接返回
		return iTempRet;
	}

	//发送本次提交的数据
	iBytesLeft = iCodeLength;
	pbyTemp = pCode;

	while (iBytesLeft > 0)
	{
		iBytesSent = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);

		if (iBytesSent > 0)
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
		}


		if (iBytesSent < 0 && errno != EAGAIN)

		{
			m_iStatus = tcs_error;
			iTempRet = ERR_SEND_FAILED;
			break;
		}

		else if (iBytesSent < 0)
		{
			// Socket发送缓冲区满，则将剩余的数据放到缓存中
			memcpy((void*)&(m_abyPostBuffer[m_iPostEnd]), (const void*)pbyTemp, iBytesLeft);
			m_iPostEnd += iBytesLeft;
			iTempRet = ERR_SEND_NOBUFF;
			break;
		}

	}

	return iTempRet;
}


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
bool CTCPClient<RecvBufLen_, SendBufLen_>::AddToFDSet(fd_set& tmFdSet)
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
bool CTCPClient<RecvBufLen_, SendBufLen_>::IsFDSetted(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_iStatus == eTcpConnected)
	{
		return  = FD_ISSET(nSocket, tmFdSet);
	}
	return false;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
void CTCPClient<RecvBufLen_, SendBufLen_>::GetCriticalData(int& iReadBegin, int& iReadEnd, int& iPostBegin, int& iPostEnd)
{
	iReadBegin = m_iReadBegin;
	iReadEnd = m_iReadEnd;
	iPostBegin = m_iPostBegin;
	iPostEnd = m_iPostEnd;
}


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::HasReserveData()
{
	if (m_iPostEnd - m_iPostBegin > 0)
	{
		return True;
	}
	else
	{
		return False;
	}
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPClient<RecvBufLen_, SendBufLen_>::CleanReserveData()
{
	int iBytesSent = 0, iBytesLeft = 0, iBytesCleaned = 0, iTempRet = 0;
	BYTE* pbyTemp = NULL;

	if (m_iSocketFD < 0 || m_iStatus != tcs_connected)
	{
		return ERR_SEND_NOSOCK;
	}
	iBytesLeft = m_iPostEnd - m_iPostBegin;
	pbyTemp = &(m_abyPostBuffer[m_iPostBegin]);
	while (iBytesLeft > 0)
	{
		iBytesSent = send(m_iSocketFD, (const char*)pbyTemp, iBytesLeft, 0);
		if (iBytesSent > 0)
		{
			pbyTemp += iBytesSent;
			iBytesLeft -= iBytesSent;
			m_iPostBegin += iBytesSent;
			iBytesCleaned += iBytesSent;
		}
		if (iBytesSent < 0 && errno != EAGAIN)
		{
			m_iStatus = tcs_error;
			iTempRet = ERR_SEND_FAILED;
			break;
		}
		else if (iBytesSent < 0)
		{
			iTempRet = ERR_SEND_NOBUFF;
			LOG_ERROR("default", "CleanReserveData Failed : iBytesSent(%d), iBytesLeft(%d), errno(%d : %s).",
				iBytesSent, iBytesLeft, errno, strerror(errno));
			break;
		}
	}
	if (iBytesLeft == 0)
	{
		m_iPostBegin = m_iPostEnd = 0;
		iTempRet = 0;
	}
	//SAY("%d bytes is cleaned, left %d bytes.", iBytesCleaned, iBytesLeft);                                      
	return iTempRet;
}
#endif //__TCP_CLIENT_H__
