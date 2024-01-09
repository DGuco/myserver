#include "tcp_server.h"
#include "my_assert.h"
#include "log.h"

CTCPServer::CTCPServer(eTcpServerModule module, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: m_nRunModule(module)
{
#ifdef __LINUX__
	m_pEpollEventList = NULL;
	m_nEpollFd = -1;
#endif
}

CTCPServer::~CTCPServer()
{
#ifdef __LINUX__
	ClearEpoll();
#endif
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
#ifdef __LINUX__
	if (m_nRunModule == eTcpSelect)
	{
		return InitSelect(m_IPAddr.c_str(), m_nPort);
	}
	else
	{
		return InitEpoll(m_IPAddr.c_str(), m_nPort);
	}
#else
	ASSERT_EX(m_nRunModule == eTcpSelect, return, "Windows platform only run in select module");
	return InitSelect(m_IPAddr.c_str(), m_nPort);
#endif
}

bool CTCPServer::Run()
{
#ifdef __LINUX__
	if (m_nRunModule == eTcpSelect)
	{
		return SelectTick();
	}
	else
	{
		return EpollTick();
	}
#else
	return SelectTick();
#endif
}

int CTCPServer::InitSelect(const char* ip, int port)
{
	if (!m_ListenSocket.IsValid())
	{
		if (!m_ListenSocket.Open())
		{
			return 0;
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

int CTCPServer::SelectTick()
{
	FD_ZERO(&m_fdsRead);  // ����˿ڼ�
	FD_ZERO(&m_fdsWrite);  // ����˿ڼ�

	// ÿ��100���볬ʱ
	timeval tvListen;
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;

	int nListenFD = m_ListenSocket.GetSocket();
	FD_SET(nListenFD, &m_fdsRead);  // ��listen�˿ڼ���˿ڼ�
	int iMaxSocketFD = nListenFD;
	struct sockaddr_in stConnAddr;
	int iAddrLength = sizeof(stConnAddr);

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			SOCKET tmFd = it->second->GetSocketFD();
			FD_SET(tmFd, &m_fdsRead);  // ��listen�˿ڼ���˿ڼ�
			FD_SET(tmFd, &m_fdsWrite);  // ��listen�˿ڼ���˿ڼ�
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
			FD_SET(tmFd, &m_fdsRead);  // ��listen�˿ڼ���˿ڼ�
			FD_SET(tmFd, &m_fdsWrite);  // ��listen�˿ڼ���˿ڼ�
			if (it->second->GetSocketFD() > iMaxSocketFD)
			{
				iMaxSocketFD = it->second->GetSocketFD();
			}
		}
	}

	// �ȴ���ȡ
	int iTmp = select(iMaxSocketFD + 1, &m_fdsRead, &m_fdsWrite, NULL, &tvListen);

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
	if (FD_ISSET(nListenFD, &m_fdsRead))
	{
		//�µ�����
		CSocket newSocket = m_ListenSocket.Accept();
		if (newSocket.IsValid())
		{
			SafePointer<CTCPClient> pClient = CreateTcpClient(newSocket);
			m_ClientMap.insert(std::make_pair(pClient->GetSocketFD(), pClient));
			LOG_DEBUG("default", "Accept new socket fd = {} ,host = {},port = {}",newSocket.GetSocket(),newSocket.GetHost().c_str(),newSocket.GetPort());
		}
		else
		{
			LOG_ERROR("default", "Accept new socket error,listenfd = {},erromsg =  %s.", strerror(errno));
		}
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				//����tcp����
				it->second->RecvData();
				//��������
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//�ѻ��������ݷ��ͳ�ȥ
				it->second->Flush();
				//������ɺ��߼�
				it->second->DoWriteLogic();
			}
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				//����tcp����
				it->second->RecvData();
				//��������
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//�ѻ��������ݷ��ͳ�ȥ
				it->second->Flush();
				//������ɺ��߼�
				it->second->DoWriteLogic();
			}
		}
	}
	return;
}

#ifdef __LINUX__
int CTCPServer::InitEpoll(const char* ip, int port)
{
	int iTempRet;
	socklen_t iOptVal = 0;
	int iOptLen = sizeof(int);

	int flags = 1;
	struct linger ling = { 0, 0 };
	struct sockaddr_in addr;

	if (m_pEpollEventList == NULL)
	{
		if (!(m_pEpollEventList = (struct epoll_event*)malloc(MAX_SOCKET_NUM * sizeof(struct epoll_event))))
		{
			LOG_ERROR("default", "malloc() error!");
			return -1;
		}
		if ((m_nEpollFd = epoll_create(MAX_SOCKET_NUM)) < 0)
		{
			LOG_ERROR("default", "Error : {}", strerror(errno));
			LOG_ERROR("default", "epoll_create error!");
			return -1;
		}

		return 0;
	}

	m_ListenSocket.Open();
	if (!m_ListenSocket.IsValid())
	{
		ClearEpoll();
		return -2;
	}
	m_ListenSocket.SetSocketNoBlock();
	m_ListenSocket.SetReuseAddr();
	m_ListenSocket.SetKeepAlive();
	m_ListenSocket.SetLinger(0);
	m_ListenSocket.SetTcpNoDelay();

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bool bRet = 0;
	if (ip != NULL)
	{
		bRet = m_ListenSocket.Bind(port);
	}
	else
	{
		bRet = m_ListenSocket.Bind(ip, port);
	}

	if (!bRet)
	{
		ClearEpoll();
		m_ListenSocket.Close();
		return -5;
	}

	m_ListenSocket.SetSendBufSize(1024 * 1024);
	m_ListenSocket.SetRecvBufSize(1024 * 1024);

	// ���ý��ն��д�С*/
	bRet = m_ListenSocket.Listen();
	if (!bRet)
	{
		ClearEpoll();
		m_ListenSocket.Close();
		LOG_ERROR("default", "listen fd = {} connection error!", mEpollSocket);
		return -6;
	}

	if (epoll_ctl(m_ListenSocket.GetSocket(), EPOLL_CTL_ADD, sfd, &m_stEpollEvent) < 0)
	{
		ClearEpoll();
		m_ListenSocket.Close();
		return -7;
	}
	return 0;
}

int CTCPServer::EpollTick()
{
	int       	iTempRet;
	int       	i;
	int       	j;
	socklen_t 	iSockAddrSize;
	int    	  	iNewSocket;
	int       	nfds;
	int       	flags;
	struct 		epoll_event* cevents;
	struct		sockaddr_in  m_stSockAddr;                    	// �����ַ
	iSockAddrSize = sizeof(m_stSockAddr);

	nfds = epoll_wait(m_nEpollFd, m_pEpollEventList, MAX_SOCKET_NUM, 100);
	if (-1 == nfds)
	{
		LOG_ERROR("default", "epoll_wait return -1!");
	}

	for (i = 0, cevents = m_pEpollEventList; i < nfds; i++, cevents++)
	{
		SOCKET nTmFd = cevents->data.fd;
		if (0 >= nTmFd)
		{
			LOG_ERROR("default", "Error cevents->data.fd = {}!", cevents->data.fd);
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			LOG_ERROR("default", "cevents->events generate error event!");
			continue;
		}
		
		if (0 == (EPOLLIN & cevents->events))
		{
			LOG_ERROR("default","cevents->events is not input event!");
			continue;
        }

		//����������
		if (nTmFd == m_ListenSocket.GetSocket())
		{
			CSocket tmConnSocket = m_ListenSocket.Accept();
			if (!tmConnSocket.IsValid())
			{
				// �ͻ������������Ժ��������ر���
				LOG_ERROR("default", "client connected port {} and disconnected! iNewSocket = {}, errno({} : {})",
					m_pSocketInfo->m_iConnectedPort, iNewSocket, errno, strerror(errno));
				continue;
			}

			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				LOG_ERROR("default", "error: socket id is so big {}", iNewSocket);
				close(iNewSocket);
				continue;
			}

			m_stEpollEvent.data.fd = iNewSocket;
			if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, sfd, &m_stEpollEvent) < 0)
			{
				return -1;
			}
			return 0;
		}
		for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
		{
			if (it->second->GetSocket().IsValid())
			{
				SOCKET tmFd = it->second->GetSocketFD();
				FD_SET(tmFd, &m_fdsRead);  // ��listen�˿ڼ���˿ڼ�
				FD_SET(tmFd, &m_fdsWrite);  // ��listen�˿ڼ���˿ڼ�
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
				FD_SET(tmFd, &m_fdsRead);  // ��listen�˿ڼ���˿ڼ�
				FD_SET(tmFd, &m_fdsWrite);  // ��listen�˿ڼ���˿ڼ�
				if (it->second->GetSocketFD() > iMaxSocketFD)
				{
					iMaxSocketFD = it->second->GetSocketFD();
				}
			}
		}


		m_pSocketInfo = &m_pstSocketInfo[iFDTemp];
		if (0 >= iFDTemp)
		{
			LOG_ERROR("default", "Error cevents->data.fd = {}!", cevents->data.fd);
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			LOG_ERROR("default", "cevents->events generate error event!");
			m_pSocketInfo = &m_pstSocketInfo[iFDTemp];
			ClearSocketInfo(Err_ClientClose);
			continue;
		}

		if (0 == (EPOLLIN & cevents->events))
		{
			LOG_ERROR("default", "cevents->events does not generate input event!");
			continue;
		}

		if (LISTEN_SOCKET == m_pSocketInfo->m_iSocketType)
		{
			LOG_NOTICE("default", "recv  events:{} fd:{}", nfds, iFDTemp);

			iNewSocket = accept(iFDTemp, (struct sockaddr*)&m_stSockAddr, (socklen_t*)&iSockAddrSize);
			if (0 >= iNewSocket)
			{
				// �ͻ������������Ժ��������ر���
				LOG_NOTICE("default", "client connected port %d and disconnected! iNewSocket = {}, errno({} : {})",
					m_pSocketInfo->m_iConnectedPort, iNewSocket, errno, strerror(errno));

				continue;
			}

			m_stTcpStat.m_iConnIncoming++;
			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				LOG_ERROR("default", "error: socket id is so big {}", iNewSocket);
				closesocket(iNewSocket);
				continue;
			}

			if (ioctl(iNewSocket, FIONBIO, &flags) &&
				((flags = fcntl(iNewSocket, F_GETFL, 0)) < 0 ||
					fcntl(iNewSocket, F_SETFL, flags | O_NONBLOCK) < 0))
			{
				LOG_ERROR("default", "operate on socket %d error connect port {}!", iNewSocket, m_pSocketInfo->m_iConnectedPort);
				closesocket(iNewSocket);
				continue;
			}

			iTempRet = eph_new_conn(iNewSocket);
			if (0 != iTempRet)
			{
				LOG_ERROR("default", "add to epoll failed [socket {} connect port {}]!", iNewSocket, m_pSocketInfo->m_iConnectedPort);
				closesocket(iNewSocket);
				continue;
			}

			// ���ĵ�ǰ������socket
			if (iNewSocket > maxfds)
			{
				maxfds = iNewSocket;
			}
			// ����������1
			m_stTcpStat.m_iConnTotal++;

			// ����һ�����е�socket�ṹ
			j = iNewSocket;
			char* pTempIP = inet_ntoa(m_stSockAddr.sin_addr);
			m_pstSocketInfo[j].m_iSrcIP = m_stSockAddr.sin_addr.s_addr;
			m_pstSocketInfo[j].m_nSrcPort = m_stSockAddr.sin_port;
			strncpy(m_pstSocketInfo[j].m_szClientIP, pTempIP, sizeof(m_pstSocketInfo[j].m_szClientIP) - 1);
			time(&(m_pstSocketInfo[j].m_tCreateTime)); // ��¼��socket������ʱ��
			m_pstSocketInfo[j].m_tStamp = m_pstSocketInfo[j].m_tCreateTime;
			m_pstSocketInfo[j].m_iSocketType = CONNECT_SOCKET;
			m_pstSocketInfo[j].m_iSocket = iNewSocket;
			m_pstSocketInfo[j].m_iSocketFlag = RECV_DATA;  // ����socket��״̬�Ǵ���������
			m_pstSocketInfo[j].m_iConnectedPort = m_pSocketInfo->m_iConnectedPort;

			m_pstSocketInfo[j].m_iUin = 0;

			LOG_NOTICE("default", "{} connected port {}, socket id = {}.", pTempIP, m_pSocketInfo->m_iConnectedPort, iNewSocket);
		}
		else
		{
			m_pSocketInfo = &m_pstSocketInfo[iFDTemp];
			RecvClientData(iFDTemp);
		}
	}

	return 0;
}

int CTCPServer::EpollDelSocket(CTCPSocket* pSocket)
{
	int iTempRet;
	// �ر�Socket����socket�����Ϣ���
	if (pSocket != NULL && pSocket->GetSocket().IsValid())
	{
		epoll_event tmEvent;
		tmEvent.data.fd = pSocket->GetSocketFD();
		if (epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, pSocket->GetSocketFD(), &tmEvent) < 0)
		{
			LOG_ERROR("default", "epoll remove socket error: fd = {} ", pSocket->GetSocketFD());
		}

		pSocket->Close();
	}
	return 0;
}

//
int CTCPServer::ClearEpoll()
{
	if (m_pEpollEventList != NULL)
	{
		free(m_pEpollEventList);
		m_pEpollEventList = NULL;
	}
	if (m_nEpollFd != -1)
	{
		close(m_nEpollFd);
		m_nEpollFd = -1;
	}
	return 0;
}
#endif
