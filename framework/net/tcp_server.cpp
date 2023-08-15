#include "tcp_server.h"
#include "my_assert.h"
#include "log.h"

CTCPServer::CTCPServer(eTcpServerModule module, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	:	m_Socket(RecvBufLen_, SendBufLen_),
		m_nRunModule(module)
{

}

CTCPServer::~CTCPServer()
{

}

int CTCPServer::InitServer(const char* ipAddr, u_short unPort)
{
	m_IPAddr = ipAddr;
	m_nPort = unPort;
	return 0;
}

int CTCPServer::ConnectTo(const char* szLocalAddr,int port,bool bblock)
{
	return m_Socket.ConnectTo(szLocalAddr, port, bblock);
}

int CTCPServer::CreateServer()
{
	if (m_Socket.InitTcpServer(m_IPAddr.c_str(),m_nPort))
	{
		LOG_ERROR("default", "InitTcpServer Listen Server  {}: {} failed.", m_IPAddr.c_str(),m_nPort);
		return -1;
	}
	return 0;
}

void CTCPServer::Run()
{
#ifdef __LINUX__
#else
	ASSERT_EX(m_nRunModule == eTcpSelect,return,"Windows platform only run in select module");
#endif
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

	// ÿ��100���볬ʱ
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;
	FD_ZERO(&fds_read);  // ����˿ڼ�
	iListenSocketFD = m_Socket.GetSocketFD();

	FD_SET(iListenSocketFD, &fds_read);  // ��listen�˿ڼ���˿ڼ�
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

	// �ȴ���ȡ
	iTmp = select(iMaxSocketFD + 1, &fds_read, NULL, NULL, &tvListen);

	// ���û�пɶ����߳����򷵻�
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default", "Select error, %s.", strerror(errno));
		}
		return iTmp;
	}

	// ���iListenSocketFD��fds_read��
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
			//				if (setsockopt(iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptVal, iOptLen))  // ���÷��ͻ������Ĵ�С
			//				{
			//					LOG_ERROR( "default", "Socket(%d) Set send buffer size to %d failed!", iNewSocketFD, iOptVal);
			//				}
			//				else
			if (getsockopt(iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (void*)&iOptVal, (socklen_t*)&iOptLen) == 0)  // �鿴�Ƿ����óɹ�
			{
				LOG_INFO("default", "Socket(%d) Set Send buf of socket is %d.", iNewSocketFD, iOptVal);
			}
		}
		//}
	}

	// ע�������ǴӺ���ǰɨ�裬��Ϊ�ڴ˹�����
	// �п��ܻ�ɾ����ǰԪ�ز������һ����䵽�˴�
	for (i = m_iCurrentUnRegisterNum - 1; i >= 0; i--) {
		if (FD_ISSET(m_astUnRegisterInfo[i].m_iSocketFD, &fds_read)) {
			ReceiveAndProcessRegister(i);
		}
	}

	//	sleep(0);

	return 1;
}

CTCPSocket & CTCPServer::GetSocket()
{
	return m_Socket;
}

int CTCPServer::GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr)
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

u_long CTCPServer::GetConnAddr()
{
	return m_ulIPAddr;
}

u_short CTCPServer::GetConnPort()
{
	return m_nPort;
}

short CTCPServer::GetEntityType()
{
	return m_nEntityType;
}

short CTCPServer::GetEntityID()
{
	return m_nEntityID;
}

