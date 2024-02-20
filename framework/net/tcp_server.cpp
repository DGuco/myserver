#include "tcp_server.h"
#include "my_assert.h"
#include "tcp_socket.h"
#include "log.h"

CTCPServer::CTCPServer(eTcpServerModule module)
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

int CTCPServer::InitTcpServer(const char* ipAddr, u_short unPort)
{
	m_IPAddr = ipAddr;
	m_nPort = unPort;
	return 0;
}

SafePointer<CTCPClient> CTCPServer::ConnectTo(const char* szLocalAddr,
											int port,
											unsigned int RecvBufLen_,
											unsigned int SendBufLen_,
											bool bblock)
{
	CSocket tmSocket;
	tmSocket.Open();
	SafePointer<CTCPClient> tcpClient =  CreateTcpClient(tmSocket);
	if (tcpClient != NULL)
	{
		if (tcpClient->ConnectTo(szLocalAddr, port, false) != 0)
		{
			tcpClient.ForceFree();
			return NULL;
		}
		else
		{
#ifdef __LINUX__
			if (m_nRunModule == eTcpSelect)
			{
			}
			else
			{
				EpollAddSocket(tcpClient->GetSocketFD());
			}
#else
#endif
		}
		m_ClientMap.insert(std::make_pair(tcpClient->GetSocketFD(), tcpClient));
	}
	return tcpClient;
}

int CTCPServer::InitTcpServer()
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

int CTCPServer::SelectTick()
{
	FD_ZERO(&m_fdsRead);  // 清除端口集
	FD_ZERO(&m_fdsWrite);  // 清除端口集

	// 每隔100毫秒超时
	timeval tvListen;
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;

	int nListenFD = m_ListenSocket.GetSocket();
	FD_SET(nListenFD, &m_fdsRead);  // 将listen端口加入端口集
	int iMaxSocketFD = nListenFD;
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
	int iTmp = select(iMaxSocketFD + 1, &m_fdsRead, &m_fdsWrite, NULL, &tvListen);

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
	if (FD_ISSET(nListenFD, &m_fdsRead))
	{
		//新的连接
		CSocket newSocket = m_ListenSocket.Accept();
		if (newSocket.IsValid())
		{
			SafePointer<CTCPConn> pConn = CreateTcpConn(newSocket);
			if (pConn != NULL)
			{
				m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
				LOG_DEBUG("default", "Accept new socket fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
			}
			else
			{
				LOG_ERROR("default", "CreateTcpConn failed fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
				newSocket.Close();
			}
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
				//接受tcp数据
				it->second->RecvData();
				//处理数据
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//把缓冲区数据发送出去
				it->second->Flush();
				//发送完成后逻辑
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
				//接受tcp数据
				it->second->RecvData();
				//处理数据
				it->second->DoRecvLogic();
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//把缓冲区数据发送出去
				it->second->Flush();
				//发送完成后逻辑
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

	// 设置接收队列大小*/
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
			LOG_ERROR("default", "Error cevents->data.fd = {}!", cevents->data.fd);
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			LOG_ERROR("default", "cevents->events generate error event!");
			continue;
		}
		
// 		if (0 == (EPOLLIN & cevents->events))
// 		{
// 			LOG_ERROR("default", "cevents->events is not input event!");
// 			continue;
// 		}

		//监听描述符
		if (nTmFd == m_ListenSocket.GetSocket())
		{
			CSocket tmConnSocket = m_ListenSocket.Accept();
			if (!tmConnSocket.IsValid())
			{
				// 客户端连接上来以后又立即关闭了
				LOG_ERROR("default", "client connected port {} and disconnected! errno({} : {})",
					tmConnSocket.GetHost().c_str(), errno, strerror(errno));
				continue;
			}

			SOCKET iNewSocket = tmConnSocket.GetSocket();
			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				LOG_ERROR("default", "error: socket id is so big {}", iNewSocket);
				close(iNewSocket);
				continue;
			}
			if (!tmConnSocket.SetSocketNoBlock())
			{
				LOG_ERROR("default", "error: socket set noblock failed ,fd = {}", iNewSocket);
				tmConnSocket.Close();
				continue;
			}

			SafePointer<CTCPConn> pConn = CreateTcpConn(tmConnSocket);
			if (pConn != NULL)
			{
				if (EpollAddSocket(iNewSocket) != 0)
				{
					LOG_ERROR("default", "error: EpollAddSocket failed ,fd = {}", iNewSocket);
					tmConnSocket.Close();
					continue;
				}
				m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
				LOG_DEBUG("default", "Accept new socket fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
			}
			else
			{
				LOG_ERROR("default", "CreateTcpConn failed fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
				tmConnSocket.Close();
				continue;
			}
		//可读
		}else if(EPOLLIN & cevents->events)
		{
			ClientMap::iterator itcleint = m_ClientMap.find(nTmFd);
			if (itcleint != m_ClientMap.end())
			{
				//接受tcp数据
				it->second->RecvData();
				//处理数据
				it->second->DoRecvLogic();
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itcleint != m_ConnMap.end())
			{
				//接受tcp数据
				it->second->RecvData();
				//处理数据
				it->second->DoRecvLogic();
				continue;
			}
		}
		//可写
		else if (EPOLLOUT & cevents->events)
		{
			ClientMap::iterator itcleint = m_ClientMap.find(nTmFd);
			if (itcleint != m_ClientMap.end())
			{
				//把缓冲区数据发送出去
				it->second->Flush();
				//发送完成后逻辑
				it->second->DoWriteLogic();
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itcleint != m_ConnMap.end())
			{
				//把缓冲区数据发送出去
				it->second->Flush();
				//发送完成后逻辑
				it->second->DoWriteLogic();
				continue;
			}
		}
	}
	return 0;
}

int CTCPServer::EpollDelSocket(SOCKET socket)
{
	int iTempRet;
	// 关闭Socket，把socket相关信息清空
	if (socket != INVALID_SOCKET)
	{
		epoll_event tmEvent;
		tmEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		tmEvent.data.ptr = NULL;
		tmEvent.data.fd = socket;
		if (epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, socket, &tmEvent) < 0)
		{
			LOG_ERROR("default", "epoll remove socket error: fd = {} ", socket);
			return -1;
		}
	}
	return 0;
}

int CTCPServer::EpollAddSocket(SOCKET socket)
{
	if (socket != INVALID_SOCKET)
	{
		epoll_event tmEvent;
		tmEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		tmEvent.data.ptr = NULL;
		tmEvent.data.fd = socket;
		if (epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, socket, &tmEvent) < 0)
		{
			LOG_ERROR("default", "epoll remove socket error: fd = {} ", socket);
			return -1;
		}
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

