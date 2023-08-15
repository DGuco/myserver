/*****************************************************************
* FileName:tcp_server.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "tcp_socket.h"

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
class CTCPServer
{
public:
	CTCPServer();
	~CTCPServer();
	int Initialize(short nEntityType, short nEntityID, u_long ulIPAddr, u_short unPort);
	int ConnectTo(char* szLocalAddr = NULL, bool bblock);
	int CreateServer();
	void Run();
	int GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr);
	u_long GetConnAddr();
	u_short GetConnPort();
	short  GetEntityType();
	short  GetEntityID();
	CTCPSocket<uiRecvBufLen, uiSendBufLen>& GetSocket();

private:
	short   m_nEntityType;
	short   m_nEntityID;
	CTCPSocket<uiRecvBufLen, uiSendBufLen> m_stSocket;
	u_long m_ulIPAddr;
	u_short m_unPort;
};


template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
CTCPServer<uiRecvBufLen, uiSendBufLen>::CTCPServer()
{
	m_ulIPAddr = 0;
	m_nEntityID = -1;
	m_nEntityType = -1;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
CTCPServer<uiRecvBufLen, uiSendBufLen>::~CTCPServer()
{
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
int CTCPServer<uiRecvBufLen, uiSendBufLen>::Initialize(short nEntityType, short nEntityID, u_long ulIPAddr, u_short unPort)
{
	m_ulIPAddr = ulIPAddr;
	m_unPort = unPort;
	m_nEntityID = nEntityID;
	m_nEntityType = nEntityType;

	return 0;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
int CTCPServer<uiRecvBufLen, uiSendBufLen>::ConnectToServer(char* szLocalAddr, eLinkMode emBlock)
{
	return m_stSocket.ConnectTo(m_ulIPAddr, m_unPort, emBlock);
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
int CTCPServer<uiRecvBufLen, uiSendBufLen>::CreateServer()
{
	struct in_addr in;
	if (m_nEntityID <= 0)
	{
		return -1;
	}

	in.s_addr = m_ulIPAddr;
	if (m_stSocket.InitTcpServer(m_unPort, inet_ntoa(in)/*将in转变成xx.xx.xx.xx的形式*/))
	{
		LOG_ERROR("default", "InitTcpServer Listen Server  %d failed.", m_nEntityID);
		return -1;
	}
	return 0;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
CTCPSocket<uiRecvBufLen, uiSendBufLen>& CTCPServer<uiRecvBufLen, uiSendBufLen>::Run()
{
	fd_set fds_read;
	timeval tvListen;
	int i = 0, iTmp;
	int iListenSocketFD = -1;
	int iNewSocketFD = -1;
	struct sockaddr_in stConnAddr;
	CString<32> szConnAddr;
	int iMaxSocketFD = -1;

	memset(&stConnAddr, 0, sizeof(stConnAddr));
	socklen_t iAddrLength = sizeof(stConnAddr);

	// 每隔100毫秒超时
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;
	FD_ZERO(&fds_read);  // 清除端口集
	iListenSocketFD = m_stSocket.GetSocketFD();

	FD_SET(iListenSocketFD, &fds_read);  // 将listen端口加入端口集
	iMaxSocketFD = iListenSocketFD;

	for (i = 0; i < m_iCurrentUnRegisterNum; i++)
	{
		if (m_astUnRegisterInfo[i].m_iRegisted == 0)
		{
			FD_SET(m_astUnRegisterInfo[i].m_iSocketFD, &fds_read);
			if (m_astUnRegisterInfo[i].m_iSocketFD > iMaxSocketFD)
			{
				iMaxSocketFD = m_astUnRegisterInfo[i].m_iSocketFD;
			}
		}
	}

	// 等待读取
	iTmp = select(iMaxSocketFD + 1, &fds_read, NULL, NULL, &tvListen);

	// 如果没有可读或者出错则返回
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default", "Select error, %s.", strerror(errno));
		}
		return iTmp;
	}

	// 如果iListenSocketFD在fds_read中
	if (FD_ISSET(iListenSocketFD, &fds_read))
	{
		iNewSocketFD = accept(iListenSocketFD, (struct sockaddr*)&stConnAddr, &iAddrLength);
		SockAddrToString(&stConnAddr, (char*)szConnAddr);
		LOG_INFO("default", "Get a conn request from %s socket fd %d.", szConnAddr, iNewSocketFD);

		if (m_iCurrentUnRegisterNum >= MAX_UNREGISTER_NUM)
		{
			LOG_ERROR("default", "Error there is no empty space(cur num: %d) to record.", m_iCurrentUnRegisterNum);
			close(iNewSocketFD);
		}
		else
		{
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_iSocketFD = iNewSocketFD;
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_ulIPAddr = stConnAddr.sin_addr.s_addr;
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_tAcceptTime = time(NULL);
			m_iCurrentUnRegisterNum++;

			int iOptLen = sizeof(socklen_t);
			int iOptVal = SENDBUFSIZE;
			//				if (setsockopt(iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptVal, iOptLen))  // 设置发送缓冲区的大小
			//				{
			//					LOG_ERROR( "default", "Socket(%d) Set send buffer size to %d failed!", iNewSocketFD, iOptVal);
			//				}
			//				else
			if (getsockopt(iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (void*)&iOptVal, (socklen_t*)&iOptLen) == 0)  // 查看是否设置成功
			{
				LOG_INFO("default", "Socket(%d) Set Send buf of socket is %d.", iNewSocketFD, iOptVal);
			}
		}
		//}
	}

	// 注意这里是从后向前扫描，因为在此过程中
	// 有可能会删除当前元素并把最后一个填充到此处
	for (i = m_iCurrentUnRegisterNum - 1; i >= 0; i--) {
		if (FD_ISSET(m_astUnRegisterInfo[i].m_iSocketFD, &fds_read)) {
			ReceiveAndProcessRegister(i);
		}
	}

	//	sleep(0);

	return 1;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
CTCPSocket<uiRecvBufLen, uiSendBufLen>& CTCPServer<uiRecvBufLen, uiSendBufLen>::GetSocket()
{
	return m_stSocket;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
int CTCPServer<uiRecvBufLen, uiSendBufLen>::GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr)
{
	if (!pnEntityType || !pnEntityID || !pulIpAddr)
	{
		return -1;
	}

	*pnEntityType = m_nEntityType;
	*pnEntityID = m_nEntityID;
	*pulIpAddr = m_ulIPAddr;

	return 0;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
u_long CTCPServer<uiRecvBufLen, uiSendBufLen>::GetConnAddr()
{
	return m_ulIPAddr;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
u_short CTCPServer<uiRecvBufLen, uiSendBufLen>::GetConnPort()
{
	return m_unPort;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
short CTCPServer<uiRecvBufLen, uiSendBufLen>::GetEntityType()
{
	return m_nEntityType;
}

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
short CTCPServer<uiRecvBufLen, uiSendBufLen>::GetEntityID()
{
	return m_nEntityID;
}


#endif //__TCP_SERVER_H__
