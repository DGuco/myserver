#include "tcp_server.h"
#include "my_assert.h"
#include "log.h"

CTCPServer::CTCPServer(eTcpServerModule module, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: m_nRunModule(module)
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
	return ConnectTo(szLocalAddr, port, bblock);
}

int CTCPServer::CreateServer()
{
	if (InitTcpServer(m_IPAddr.c_str(),m_nPort))
	{
		LOG_ERROR("default", "InitTcpServer Listen Server  {}: {} failed.", m_IPAddr.c_str(),m_nPort);
		return -1;
	}
	return 0;
}

bool CTCPServer::Run()
{
#ifdef __WINDOWS__
	ASSERT_EX(m_nRunModule == eTcpSelect, return, "Windows platform only run in select module");
#endif
	ASSERT_EX(m_ListenSocket.IsValid(), return, "Listen socket is not valid");
	if (m_nRunModule == eTcpSelect)
	{
		return SelectTick();
	}
	else
	{
		return EpollTick();
	}
}

bool CTCPServer::InitTcpServer(const char* ip, int port)
{
	if (!m_ListenSocket.IsValid())
	{
		if (!m_ListenSocket.Open())
		{
			return -1;
		}
	}

	//�����׽ӿں�һ������ʹ���еĵ�ַ����
	if (!m_ListenSocket.SetReuseAddr())  return -1;

	bool bRet = 0;
	if (ip != NULL)
	{
		bRet = m_ListenSocket.Bind(port);
	}
	else
	{
		bRet = m_ListenSocket.Bind(ip, port);
	}

	if (!bRet) return -1;

	if (!m_ListenSocket.Listen()) return -1;

	m_ListenSocket.SetSocketNoBlock();
	return 0;
}

bool CTCPServer::SelectTick()
{
	fd_set fds_read;
	fd_set fds_write;
	FD_ZERO(&fds_read);  // ����˿ڼ�
	FD_ZERO(&fds_write);  // ����˿ڼ�

	// ÿ��100���볬ʱ
	timeval tvListen;
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;

	int iListenSocketFD = m_ListenSocket.GetSocket();
	FD_SET(iListenSocketFD, &fds_read);  // ��listen�˿ڼ���˿ڼ�
	int iMaxSocketFD = iListenSocketFD;
	struct sockaddr_in stConnAddr;
	int iAddrLength = sizeof(stConnAddr);

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			SOCKET tmFd = it->second->GetSocketFD();
			FD_SET(tmFd, &fds_read);  // ��listen�˿ڼ���˿ڼ�
			FD_SET(tmFd, &fds_write);  // ��listen�˿ڼ���˿ڼ�
			if (it->second->GetSocketFD() > iMaxSocketFD)
			{
				iMaxSocketFD = it->second->GetSocketFD();
			}
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			SOCKET tmFd = it->second->GetSocketFD();
			FD_SET(tmFd, &fds_read);  // ��listen�˿ڼ���˿ڼ�
			FD_SET(tmFd, &fds_write);  // ��listen�˿ڼ���˿ڼ�
			if (it->second->GetSocketFD() > iMaxSocketFD)
			{
				iMaxSocketFD = it->second->GetSocketFD();
			}
		}
	}

	// �ȴ���ȡ
	int iTmp = select(iMaxSocketFD + 1, &fds_read, NULL, NULL, &tvListen);

	// ���û�пɶ����߳����򷵻�
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default", "Select error, %s.", strerror(errno));
		}
		return;
	}

	// ���iListenSocketFD��fds_read��
	if (FD_ISSET(iListenSocketFD, &fds_read))
	{
		SOCKET iNewSocketFD = accept(iListenSocketFD, (struct sockaddr*)&stConnAddr, &iAddrLength);
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &fds_read))
			{
				it->second->RecvData();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &fds_write))
			{
				it->second->RecvData();
			}
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &fds_read))
			{
				it->second->Write();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &fds_write))
			{
				it->second->Write();
			}
		}
	}
	return;
}

bool CTCPServer::EpollTick()
{
#ifdef __LINUX__

#else
	return 0;
#endif

}
