#include "tcp_server.h"
#include "my_assert.h"
#include "tcp_socket.h"
#include "time_helper.h"
#include "log.h"

CTCPServer::CTCPServer()
	: m_nRunModule(eTcpSelect)
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

int CTCPServer::InitTcpServer(eTcpServerModule module,const char* ipAddr, u_short unPort)
{
	m_nRunModule = module;
	m_IPAddr = ipAddr;
	m_nPort = unPort;
	return PrepareToRun();
}

bool CTCPServer::ConnectTo(CSafePtr<CTCPClient> pClient,const char* szLocalAddr,int port,bool bblock)
{
	if (pClient == NULL)
	{
		return false;
	}

	if (pClient->GetSocket().IsValid())
	{
		m_ClientMap.erase(pClient->GetSocketFD());
		pClient->Close();
	}

	int nRet = pClient->ConnectTo(szLocalAddr, port, false);
	if (nRet != 0)
	{
		CACHE_LOG(TCP_ERROR, "Conn to [{} : {}] failed,errorcode = {}", szLocalAddr, port, nRet);
		return false;
	}
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
	m_ClientMap.insert(std::make_pair(pClient->GetSocketFD(), pClient));
	return true;
}

int CTCPServer::PrepareToRun()
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
	if (m_nRunModule == eTcpEpoll)
	{
		m_nRunModule = eTcpSelect;
	}
	ASSERT_EX(m_nRunModule == eTcpSelect, "Windows platform only run in select module");
	return InitSelect(m_IPAddr.c_str(), m_nPort);
#endif
}

bool CTCPServer::TcpTick()
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

//
CSafePtr<CTCPConn> CTCPServer::FindTcpConn(SOCKET socket)
{
	ConnMap::iterator itconn = m_ConnMap.find(socket);
	if (itconn != m_ConnMap.end())
	{
		return itconn->second;
	}
	return NULL;
}

//
CSafePtr<CTCPClient>   CTCPServer::FindTcpClient(SOCKET socket)
{
	ClientMap::iterator itcleint = m_ClientMap.find(socket);
	if (itcleint != m_ClientMap.end())
	{
		return itcleint->second;
	}
	return NULL;
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

	CACHE_LOG(TCP_DEBUG, "CTCPServer::InitSelect listen {} : {},fdsize = {}.", ip,port,FD_SETSIZE);
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
			CACHE_LOG(TCP_ERROR, "Select error, {}.", strerror(errno));
		}
		return -1;
	}

	// 如果iListenSocketFD在fds_read中
	if (FD_ISSET(nListenFD, &m_fdsRead))
	{
		//新的连接
		CSocket newSocket = m_ListenSocket.Accept();
		if (newSocket.IsValid())
		{
			CSafePtr<CTCPConn> pConn = CreateTcpConn(newSocket);
			if (pConn != NULL)
			{
				pConn->SetCreateTime(CTimeHelper::GetSingletonPtr()->GetANSITime());
				m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
				CACHE_LOG(TCP_DEBUG, "Accept new socket fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
				OnNewConnect(pConn);
			}
			else
			{
				CACHE_LOG(TCP_ERROR, "CreateTcpConn failed fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
				newSocket.Close();
			}
		}
		else
		{
			CACHE_LOG(TCP_ERROR, "Accept new socket error,listenfd = {},erromsg =  %s.", strerror(errno));
		}
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid())
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				//接受tcp数据
				int nRet = it->second->Recv();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				//处理数据
				nRet = it->second->DoRecvLogic();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				continue;
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//把缓冲区数据发送出去
				int nRet =  it->second->Flush();
				if (nRet != ERR_SEND_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				//发送完成后逻辑
				nRet = it->second->DoWriteLogic();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				continue;
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
				int nRet = it->second->Recv();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				//处理数据
				nRet = it->second->DoRecvLogic();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				continue;
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				//把缓冲区数据发送出去
				int nRet = it->second->Flush();
				if (nRet != ERR_SEND_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				//发送完成后逻辑
				nRet = it->second->DoWriteLogic();
				if (nRet != ERR_RECV_OK)
				{
					it->second->DoErrorLogic(nRet);
					it->second->Close();
					continue;
				}
				continue;
			}
		}
	}

	//回收已关闭或者出错的连接
	FreeClosedSocket();
	return 0;
}

void CTCPServer::FreeClosedSocket()
{
	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end();)
	{
		if (!it->second->GetSocket().IsValid())
		{
			it->second.Free();
			it = m_ConnMap.erase(it);
		}
		else
		{
			it++;
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end();)
	{
		if (!it->second->GetSocket().IsValid())
		{
			it->second.Free();
			it = m_ClientMap.erase(it);
		}
		else
		{
			it++;
		}
	}
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
			CACHE_LOG(TCP_ERROR, "malloc epoll_event error!");
			return -1;
		}
		if ((m_nEpollFd = epoll_create(MAX_SOCKET_NUM)) < 0)
		{
			CACHE_LOG(TCP_ERROR, "epoll_create Error : {}", strerror(errno));
			CACHE_LOG(TCP_ERROR, "epoll_create error!");
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
		CACHE_LOG(TCP_ERROR, "listen fd = {} connection error!", mEpollSocket);
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
		CACHE_LOG(TCP_ERROR, "epoll_wait return -1!");
	}

	for (i = 0, cevents = m_pEpollEventList; i < nfds; i++, cevents++)
	{
		SOCKET nTmFd = cevents->data.fd;
		if (0 >= nTmFd)
		{
			CACHE_LOG(TCP_ERROR, "Error cevents->data.fd = {}!", cevents->data.fd);
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			CACHE_LOG(TCP_ERROR, "cevents->events generate error event!");
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
				CACHE_LOG(TCP_ERROR, "client connected port {} and disconnected! errno({} : {})",
					tmConnSocket.GetHost().c_str(), errno, strerror(errno));
				continue;
			}

			SOCKET iNewSocket = tmConnSocket.GetSocket();
			if (MAX_SOCKET_NUM <= iNewSocket)
			{
				CACHE_LOG(TCP_ERROR, "error: socket id is so big {}", iNewSocket);
				close(iNewSocket);
				continue;
			}
			if (!tmConnSocket.SetSocketNoBlock())
			{
				CACHE_LOG(TCP_ERROR, "error: socket set noblock failed ,fd = {}", iNewSocket);
				tmConnSocket.Close();
				continue;
			}

			CSafePtr<CTCPConn> pConn = CreateTcpConn(tmConnSocket);
			if (pConn != NULL)
			{
				if (EpollAddSocket(iNewSocket) != 0)
				{
					CACHE_LOG(TCP_ERROR, "error: EpollAddSocket failed ,fd = {}", iNewSocket);
					tmConnSocket.Close();
					continue;
				}
				m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
				CACHE_LOG(TCP_ERROR, "Accept new socket fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
				OnNewConnect(pConn);
			}
			else
			{
				CACHE_LOG(TCP_ERROR, "CreateTcpConn failed fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
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
				int retCode = it->second->RecvData();
				if (retCode != ERR_RECV_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				//处理数据
				retCode = it->second->DoRecvLogic();
				if (nRet != ERR_RECV_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itcleint != m_ConnMap.end())
			{
				//接受tcp数据
				int retCode = it->second->RecvData();
				if (retCode != ERR_RECV_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				//处理数据
				retCode = itcleint->second->DoRecvLogic();
				if (nRet != ERR_RECV_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
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
				int retCode = itcleint->second->Flush();
				if (retCode != ERR_SEND_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				//发送完成后逻辑
				retCode = itcleint->second->DoWriteLogic();
				if (nRet != ERR_SEND_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itcleint != m_ConnMap.end())
			{
				//把缓冲区数据发送出去
				int retCode = itcleint->second->Flush();
				if (retCode != ERR_SEND_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				//发送完成后逻辑
				retCode = itcleint->second->DoWriteLogic();
				if (nRet != ERR_SEND_OK)
				{
					itcleint->second->DoErrorLogic(retCode);
					EpollDelSocket(itcleint->second->GetSocketFD());
					itcleint->second->Close();
					continue;
				}
				continue;
			}
		}
	}
	//回收已关闭或者出错的连接
	FreeClosedSocket();
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
			CACHE_LOG(TCP_ERROR, "epoll remove socket error: fd = {} ", socket);
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
			CACHE_LOG(TCP_ERROR, "epoll remove socket error: fd = {} ", socket);
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

