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
#ifdef __WINDOWS__
	ASSERT_EX(m_nRunModule == eTcpSelect, return, "Windows platform only run in select module");
#endif
	ASSERT_EX(m_ListenSocket.IsValid(), return, "Listen socket is not valid");

	if (m_nRunModule == eTcpSelect)
	{
		return InitSelect(m_IPAddr.c_str(), m_nPort);
	}
	else
	{
		return InitEpoll(m_IPAddr.c_str(), m_nPort);
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

bool CTCPServer::InitSelect(const char* ip, int port)
{
	if (!m_ListenSocket.IsValid())
	{
		if (!m_ListenSocket.Open())
		{
			return -1;
		}
	}

	//允许套接口和一个已在使用中的地址捆绑
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

bool CTCPServer::InitEpoll(const char* ip, int port)
{
#ifdef __LINUX__
	int iTempRet;
	socklen_t iOptVal = 0;
	int iOptLen = sizeof(int);

	int flags = 1;
	struct linger ling = { 0, 0 };
	struct sockaddr_in addr;

	if (m_pEpollEventList == NULL)
	{
		memset(&m_stEpollEvent, 0, sizeof(struct epoll_event));
		m_stEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		m_stEpollEvent.data.ptr = NULL;
		m_stEpollEvent.data.fd = -1;

		if (!(m_pEpollEventList = (struct epoll_event*)malloc(MAX_SOCKET_NUM * sizeof(struct epoll_event))))
		{
			LOG_ERROR("default", "malloc() error!");
			return -1;
		}
		if ((m_nEpollFd = epoll_create(MAX_SOCKET_NUM)) < 0)
		{
			LOG_ERROR("default", "Error : %s", strerror(errno));
			LOG_ERROR("default", "epoll_create error!");
			return -1;
		}

		return 0;
	}

	m_ListenSocket.Open();
	if (!m_ListenSocket.IsValid())
	{
		return -2;
	}

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
		m_ListenSocket.Close();
		free(m_pEpollEventList);
		return -5;
	}

	m_ListenSocket.SetSendBufSize(1024 * 1024);
	m_ListenSocket.SetRecvBufSize(1024 * 1024);

	// 设置接收队列大小*/
	bRet = m_ListenSocket.Listen();
	if (!bRet)
	{
		LOG_ERROR("default", "listen fd = {} connection error!", mEpollSocket);
		return 61;
	}

	m_stEpollEvent.data.fd = m_ListenSocket.GetSocket();
	if (epoll_ctl(m_ListenSocket.GetSocket(), EPOLL_CTL_ADD, sfd, &m_stEpollEvent) < 0)
	{
		return -1;
	}

	return 0;
#else

#endif
}

bool CTCPServer::SelectTick()
{
	FD_ZERO(&m_fdsRead);  // 清除端口集
	FD_ZERO(&m_fdsWrite);  // 清除端口集

	// 每隔100毫秒超时
	timeval tvListen;
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;

	int iListenSocketFD = m_ListenSocket.GetSocket();
	FD_SET(iListenSocketFD, &m_fdsRead);  // 将listen端口加入端口集
	int iMaxSocketFD = iListenSocketFD;
	struct sockaddr_in stConnAddr;
	int iAddrLength = sizeof(stConnAddr);

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			SOCKET tmFd = it->second->GetSocketFD();
			FD_SET(tmFd, &m_fdsRead);  // 将listen端口加入端口集
			FD_SET(tmFd, &m_fdsWrite);  // 将listen端口加入端口集
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
			FD_SET(tmFd, &m_fdsRead);  // 将listen端口加入端口集
			FD_SET(tmFd, &m_fdsWrite);  // 将listen端口加入端口集
			if (it->second->GetSocketFD() > iMaxSocketFD)
			{
				iMaxSocketFD = it->second->GetSocketFD();
			}
		}
	}

	// 等待读取
	int iTmp = select(iMaxSocketFD + 1, &m_fdsRead, NULL, NULL, &tvListen);

	// 如果没有可读或者出错则返回
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default", "Select error, %s.", strerror(errno));
		}
		return;
	}

	// 如果iListenSocketFD在fds_read中
	if (FD_ISSET(iListenSocketFD, &m_fdsRead))
	{
		SOCKET iNewSocketFD = accept(iListenSocketFD, (struct sockaddr*)&stConnAddr, &iAddrLength);
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				it->second->RecvData();
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				it->second->Flush();
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
				it->second->RecvData();
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				it->second->Flush();
				it->second->DoWriteLogic();
			}
		}
	}
	return;
}

bool CTCPServer::EpollTick()
{
#ifdef __LINUX__
	int       	iTempRet;
	int       	i;
	int       	j;
	socklen_t 	iSockAddrSize;
	int    	  	iNewSocket;
	int       	nfds;
	int       	flags;
	struct 		epoll_event* cevents;
	struct		sockaddr_in  m_stSockAddr;                    	// 网络地址
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
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			LOG_ERROR("default", "cevents->events generate error event!");
			continue;
		}
		        if (0 == (EPOLLIN & cevents->events))
		{
			LOG_ERROR("default","cevents->events does not generate input event!");
			continue;
        }

		//监听描述符
		if (nTmFd == m_ListenSocket.GetSocket())
		{
			SOCKET  iNewSocket  = accept(nTmFd, (struct sockaddr*)&m_stSockAddr, (socklen_t*)&iSockAddrSize);
			if (0 >= iNewSocket)
			{
				// 客户端连接上来以后又立即关闭了
				LOG_NOTICE("default", "client connected port %d and disconnected! iNewSocket = %d, errno(%d : %s)",
					m_pSocketInfo->m_iConnectedPort, iNewSocket, errno, strerror(errno));

				continue;
			}

			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				LOG_ERROR("default", "error: socket id is so big %d", iNewSocket);
				closesocket(iNewSocket);
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
				FD_SET(tmFd, &m_fdsRead);  // 将listen端口加入端口集
				FD_SET(tmFd, &m_fdsWrite);  // 将listen端口加入端口集
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
				FD_SET(tmFd, &m_fdsRead);  // 将listen端口加入端口集
				FD_SET(tmFd, &m_fdsWrite);  // 将listen端口加入端口集
				if (it->second->GetSocketFD() > iMaxSocketFD)
				{
					iMaxSocketFD = it->second->GetSocketFD();
				}
			}
		}


		m_pSocketInfo = &m_pstSocketInfo[iFDTemp];
		if (0 >= iFDTemp)
		{
			LOG_ERROR("default", "Error cevents->data.fd = %d!", cevents->data.fd);
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
			LOG_NOTICE("default", "recv  events:%d fd:%d", nfds, iFDTemp);

			iNewSocket = accept(iFDTemp, (struct sockaddr*)&m_stSockAddr, (socklen_t*)&iSockAddrSize);
			if (0 >= iNewSocket)
			{
				// 客户端连接上来以后又立即关闭了
				LOG_NOTICE("default", "client connected port %d and disconnected! iNewSocket = %d, errno(%d : %s)",
					m_pSocketInfo->m_iConnectedPort, iNewSocket, errno, strerror(errno));

				continue;
			}

			m_stTcpStat.m_iConnIncoming++;
			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				LOG_ERROR("default", "error: socket id is so big %d", iNewSocket);
				closesocket(iNewSocket);
				continue;
			}

			if (ioctl(iNewSocket, FIONBIO, &flags) &&
				((flags = fcntl(iNewSocket, F_GETFL, 0)) < 0 ||
					fcntl(iNewSocket, F_SETFL, flags | O_NONBLOCK) < 0))
			{
				LOG_ERROR("default", "operate on socket %d error connect port %d!", iNewSocket, m_pSocketInfo->m_iConnectedPort);
				closesocket(iNewSocket);
				continue;
			}

			iTempRet = eph_new_conn(iNewSocket);
			if (0 != iTempRet)
			{
				LOG_ERROR("default", "add to epoll failed [socket %d connect port %d]!", iNewSocket, m_pSocketInfo->m_iConnectedPort);
				closesocket(iNewSocket);
				continue;
			}

			// 更改当前最大分配socket
			if (iNewSocket > maxfds)
			{
				maxfds = iNewSocket;
			}
			// 总连接数加1
			m_stTcpStat.m_iConnTotal++;

			// 搜索一个空闲的socket结构
			j = iNewSocket;
			char* pTempIP = inet_ntoa(m_stSockAddr.sin_addr);
			m_pstSocketInfo[j].m_iSrcIP = m_stSockAddr.sin_addr.s_addr;
			m_pstSocketInfo[j].m_nSrcPort = m_stSockAddr.sin_port;
			strncpy(m_pstSocketInfo[j].m_szClientIP, pTempIP, sizeof(m_pstSocketInfo[j].m_szClientIP) - 1);
			time(&(m_pstSocketInfo[j].m_tCreateTime)); // 记录该socket的生成时间
			m_pstSocketInfo[j].m_tStamp = m_pstSocketInfo[j].m_tCreateTime;
			m_pstSocketInfo[j].m_iSocketType = CONNECT_SOCKET;
			m_pstSocketInfo[j].m_iSocket = iNewSocket;
			m_pstSocketInfo[j].m_iSocketFlag = RECV_DATA;  // 设置socket的状态是待接收数据
			m_pstSocketInfo[j].m_iConnectedPort = m_pSocketInfo->m_iConnectedPort;

			m_pstSocketInfo[j].m_iUin = 0;

			LOG_NOTICE("default", "%s connected port %d, socket id = %d.", pTempIP, m_pSocketInfo->m_iConnectedPort, iNewSocket);
		}
		else
		{
			m_pSocketInfo = &m_pstSocketInfo[iFDTemp];
			RecvClientData(iFDTemp);
		}
	}

	return 0;
#else
	return 0;
#endif
}

bool CTCPServer::EpollDelSocket(CTCPSocket* pSocket)
{

#ifdef __LINUX__
	int iTempRet;

	// 关闭Socket，把socket相关信息清空
	if (pSocket != NULL && pSocket->GetSocket().IsValid())
	{
		epoll_event tmEvent;
		tmEvent.data.fd = pSocket->GetSocketFD();
		if (epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, pSocket->GetSocketFD(), &tmEvent) < 0)
		{
			LOG_ERROR("default", "epoll remove socket error: fd = %d ", pSocket->GetSocketFD());
		}

		pSocket->Close();

		
	}
#else
#endif
	return 0;
}

