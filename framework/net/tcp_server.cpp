#include "tcp_server.h"
#include "my_assert.h"
#include "tcp_socket.h"
#include "time_helper.h"
#include "log.h"

CTCPServer::CTCPServer()
	: m_nRunModule(eTcpEpoll)
{
#ifdef __LINUX__
	m_pEpollEventList = NULL;
	m_nEpollFd = -1;
#endif
}

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

	if (pClient->IsValid())
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

//socket tick
void CTCPServer::SocketTick(time_t now)
{
	//要在缓冲区的数据发出去前检查利用率
	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		it->second->DoTick(now);
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		it->second->DoTick(now);
	}
}

//socket tick
void CTCPServer::CheckSocketResize()
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	//要在缓冲区的数据发出去前检查利用率
	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		int nOldCap = it->second->GetReadBuff()->GetCapaticy();
		if (it->second->GetReadBuff()->ShrinkBuff(nNow))
		{
			CSocket tmSocket = it->second->GetSocket();
			CACHE_LOG(TCP_DEBUG, "Resize conn readbuff socket fd = {} ,host = {},port = {},oldcap = {},newcap = {}", tmSocket.GetSocket(), tmSocket.GetHost().c_str(), 
				tmSocket.GetPort(),nOldCap,it->second->GetReadBuff()->GetCapaticy());
		}

		nOldCap = it->second->GetSendBuff()->GetCapaticy();
		if (it->second->GetSendBuff()->ShrinkBuff(nNow))
		{
			CSocket tmSocket = it->second->GetSocket();
			CACHE_LOG(TCP_DEBUG, "Resize conn sendbuff socket fd = {} ,host = {},port = {},oldcap = {},newcap = {}", tmSocket.GetSocket(), tmSocket.GetHost().c_str(),
				tmSocket.GetPort(), nOldCap, it->second->GetSendBuff()->GetCapaticy());
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		int nOldCap = it->second->GetReadBuff()->GetCapaticy();
		if (it->second->GetReadBuff()->ShrinkBuff(nNow))
		{
			CSocket tmSocket = it->second->GetSocket();
			CACHE_LOG(TCP_DEBUG, "Resize client readbuff socket fd = {} ,host = {},port = {},oldcap = {},newcap = {}", tmSocket.GetSocket(), tmSocket.GetHost().c_str(),
				tmSocket.GetPort(), nOldCap, it->second->GetReadBuff()->GetCapaticy());
		}

		nOldCap = it->second->GetSendBuff()->GetCapaticy();
		if (it->second->GetSendBuff()->ShrinkBuff(nNow))
		{
			CSocket tmSocket = it->second->GetSocket();
			CACHE_LOG(TCP_DEBUG, "Resize client sendbuff socket fd = {} ,host = {},port = {},oldcap = {},newcap = {}", tmSocket.GetSocket(), tmSocket.GetHost().c_str(),
				tmSocket.GetPort(), nOldCap, it->second->GetSendBuff()->GetCapaticy());
		}
	}
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

void CTCPServer::AcceptIncomingConnect()
{
	CSafeLock lock(m_ConnectingListLock);
	for (auto it = m_ConnectingList.begin(); it!= m_ConnectingList.end(); it++)
	{
		OnAccept(*it);
		CACHE_LOG(TCP_DEBUG, "AcceptIncomingConnect socket fd = {},host = {},port = {}", it->GetSocket(), it->GetHost().c_str(), it->GetPort());
	}
	m_ConnectingList.clear();
}

//
void CTCPServer::RegisterNewConn(CSafePtr<CTCPConn> pConn)
{
	if(pConn == NULL)
	{
		return;
	}

	if(pConn->ConnKey() <= 0)
	{
		return;
	}

	if(FindTcpConn(pConn->GetSocket().GetSocket()) == NULL)
	{
		return;
	}

	auto it = m_ConnSecondMap.find(pConn->ConnKey());
	if(it == m_ConnSecondMap.end())
	{
		m_ConnSecondMap.insert(std::make_pair(pConn->ConnKey(), pConn));
		CACHE_LOG(TCP_DEBUG, "RegisterNewConn socket fd = {},ConnKey = {},ConnInfo = {}", pConn->GetSocket().GetSocket(),pConn->ConnKey(),pConn->ConnInfo());
	}
}

CSafePtr<CTCPConn> CTCPServer::FindRegisterConn(int secondKey)
{
	auto it = m_ConnSecondMap.find(secondKey);
	if(it != m_ConnSecondMap.end())
	{
		return it->second;
	}
	return NULL;
}

void CTCPServer::TcpTick(time_t now)
{
	//在缓冲区数据发送出去之前，检查缓冲区利用率
	CheckSocketResize();
	//接受其他监听线程过来的连接
	AcceptIncomingConnect();
	try
	{
#ifdef __LINUX__
		if (m_nRunModule == eTcpSelect)
		{
			SelectTick();
		}
		else
		{
			EpollTick();
		}
#else
		SelectTick();
#endif
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(TCP_ERROR, "TcpTick catch execption msg : {}", e.what());
	}

	//回收已关闭或者出错的连接
	FreeClosingSocket();
	//tcp tick
	SocketTick(now);
	//
	KickIllegalConn(now);
	return;
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

void CTCPServer::OnAccept(CSocket newSocket)
{
	if(!newSocket.IsValid())
	{
		return;
	}
#ifdef __LINUX__
	CSafePtr<CTCPConn> pConn = CreateTcpConn(tmConnSocket);
	if (pConn != NULL)
	{
		m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
		if (m_nRunModule == eTcpSelect)
		{
			if (EpollAddSocket(newSocket.GetSocket()) != 0)
			{
				CACHE_LOG(TCP_ERROR, "error: EpollAddSocket failed ,fd = {}", iNewSocket);
				pConn->DoClosingLogic(socket_error);
				pConn->Close(false);
				return;
			}
		}
		OnNewConnect(pConn);
		CACHE_LOG(TCP_DEBUG, "Accept new socket fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
	}
	else
	{
		CACHE_LOG(TCP_DEBUG, "CreateTcpConn failed fd = {} ,host = {},port = {}", tmConnSocket.GetSocket(), tmConnSocket.GetHost().c_str(), tmConnSocket.GetPort());
		tmConnSocket.Close();
		return;
	}
#else
	CSafePtr<CTCPConn> pConn = CreateTcpConn(newSocket);
	if (pConn != NULL && pConn->IsValid())
	{
		m_ConnMap.insert(std::make_pair(pConn->GetSocketFD(), pConn));
		OnNewConnect(pConn);
		CACHE_LOG(TCP_DEBUG, "Accept new socket fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
	}
	else
	{
		CACHE_LOG(TCP_ERROR, "CreateTcpConn failed fd = {} ,host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
		pConn->Close(false);
	}
#endif
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

void CTCPServer::AddNewIncomingConn(CSocket newSocket)
{
	if(newSocket.IsValid())
	{
		CSafeLock lock(m_ConnectingListLock);
		m_ConnectingList.push_back(newSocket);
	}
}

int CTCPServer::InitSelect(const char* ip, int port)
{
	if (!BeginListen(ip, port))
	{
		return -1;
	}
	return 0;
}

void CTCPServer::SelectTick()
{
	FD_ZERO(&m_fdsRead);  // 清除端口集
	FD_ZERO(&m_fdsWrite);  // 清除端口集

	// 每隔100毫秒超时
	timeval tvListen;
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;
	SOCKET iMaxSocketFD = -1;

	//tcpserver 如果进行了socket监听
	if (m_ListenSocket.IsValid())
	{
		SOCKET nListenFD = m_ListenSocket.GetSocket();
		FD_SET(nListenFD, &m_fdsRead);  // 将listen端口加入端口集
		iMaxSocketFD = nListenFD;
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid() && it->second->GetStatus() != eSocketClosing)
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
		if (it->second->GetSocket().IsValid() && it->second->GetStatus() != eSocketClosing)
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

	if(iMaxSocketFD == -1)
	{
		return;
	}
	
	// 等待读取
	int iTmp = select(iMaxSocketFD + 1, &m_fdsRead, &m_fdsWrite, NULL, &tvListen);

	// 如果没有可读或者出错则返回
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			CACHE_LOG(TCP_ERROR, "Select error, {}.", strerror(socket_error));
		}
		return;
	}

	//如果进行了监听，检查时候有新链接
	if (m_ListenSocket.IsValid())
	{
		SOCKET nListenFD = m_ListenSocket.GetSocket();
		// 如果iListenSocketFD在fds_read中
		if (FD_ISSET(nListenFD, &m_fdsRead))
		{
			do
			{
				CSocket newSocket = m_ListenSocket.Accept();
				if (!newSocket.IsValid())
				{
					// 客户端连接上来以后又立即关闭了
					CACHE_LOG(TCP_ERROR, "Accept  invalid socket socket_error({} : {})",socket_error, strerror(socket_error));
					break;
				}

				if (!newSocket.SetSocketNoBlock())
				{
					CACHE_LOG(TCP_ERROR, "Accept error: socket set noblock failed ,fd = {},host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
					newSocket.Close();
					break;
				}

				OnAccept(newSocket);
			} while (0);
		}
	}

	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid() && it->second->GetStatus() != eSocketClosing)
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				if(!TcpConnDoRecv(it->second))
				{
					continue;
				}
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				if(!TcpConnDoFlush(it->second))
				{
					continue;
				}
			}
		}
	}

	for (auto it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
	{
		if (it->second->GetSocket().IsValid() && it->second->GetStatus() != eSocketClosing)
		{
			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsRead))
			{
				if(!TcpClientDoRecv(it->second))
				{
					continue;
				}
			}

			if (FD_ISSET(it->second->GetSocketFD(), &m_fdsWrite))
			{
				if(!TcpClientDoFlush(it->second))
				{
					continue;
				}
			}
		}
	}
	return;
}


bool CTCPServer::TcpConnDoRecv(CSafePtr<CTCPConn> pConn)
{
	//接受tcp数据
	int nRet = pConn->Recv();
	if (nRet != ERR_SOCKE_OK && nRet != ERR_SOCKE_WOULD_BLOCK)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket recv error ready to close,fd = {},nRet = {},host = {},port = {}", pConn->GetSocketFD(), nRet, pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;
	}

	try
	{
		//处理数据
		nRet = pConn->DoRecvLogic();
	}
	catch(const std::exception& e)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket DoRecvLogic catch execption: {},fd = {},msg = {},host = {},port = {}",e.what(),pConn->GetSocketFD(), e.what(), pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;
	}

	if (nRet != ERR_SOCKE_OK)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket DoRecvLogic error,fd = {},nRet = {},host = {},port = {}", pConn->GetSocketFD(), nRet, pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;;
	}
	return true;
}

bool CTCPServer::TcpConnDoFlush(CSafePtr<CTCPConn> pConn)
{
	//把缓冲区数据发送出去
	int nRet =  pConn->Flush();
	if (nRet != ERR_SOCKE_OK && nRet != ERR_SOCKE_WOULD_BLOCK)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket Flush error ready to close,fd = {},nRet = {},host = {},port = {}", pConn->GetSocketFD(), nRet, pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;
	}

	try
	{
		//发送完成后逻辑
		nRet = pConn->DoWriteLogic();
	}
	catch(const std::exception& e)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket DoWriteLogic catch execption: {},fd = {},msg = {},host = {},port = {}",e.what(),pConn->GetSocketFD(), e.what(), pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;
	}

	if (nRet != ERR_SOCKE_OK)
	{
		pConn->Close(false);
		CACHE_LOG(TCP_ERROR, "Conn Socket DoWriteLogic error ready to close,fd = {},nRet = {},host = {},port = {}", pConn->GetSocketFD(), nRet, pConn->GetSocket().GetHost().c_str(), pConn->GetSocket().GetPort());
		return false;
	}
	return true;
}

bool CTCPServer::TcpClientDoRecv(CSafePtr<CTCPClient> pClient)
{
	//接受tcp数据
	int nRet = pClient->Recv();
	if (nRet != ERR_SOCKE_OK && nRet != ERR_SOCKE_WOULD_BLOCK)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket recv error ready to close,fd = {},nRet = {},host = {},port = {}", pClient->GetSocketFD(), nRet, pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;
	}

	try
	{
		//处理数据
		nRet = pClient->DoRecvLogic();
	}
	catch(const std::exception& e)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket DoRecvLogic catch execption: {},fd = {},msg = {},host = {},port = {}",e.what(),pClient->GetSocketFD(), e.what(), pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;
	}

	if (nRet != ERR_SOCKE_OK)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket DoRecvLogic error,fd = {},nRet = {},host = {},port = {}", pClient->GetSocketFD(), nRet, pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;;
	}
	return true;
}

bool CTCPServer::TcpClientDoFlush(CSafePtr<CTCPClient> pClient)
{
	//把缓冲区数据发送出去
	int nRet =  pClient->Flush();
	if (nRet != ERR_SOCKE_OK && nRet != ERR_SOCKE_WOULD_BLOCK)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket Flush error ready to close,fd = {},nRet = {},host = {},port = {}", pClient->GetSocketFD(), nRet, pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;
	}

	try
	{
		//发送完成后逻辑
		nRet = pClient->DoWriteLogic();
	}
	catch(const std::exception& e)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket DoWriteLogic catch execption: {},fd = {},msg = {},host = {},port = {}",e.what(),pClient->GetSocketFD(), e.what(), pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;
	}

	if (nRet != ERR_SOCKE_OK)
	{
		pClient->Close(false);
		CACHE_LOG(TCP_ERROR, "Client Socket DoWriteLogic error ready to close,fd = {},nRet = {},host = {},port = {}", pClient->GetSocketFD(), nRet, pClient->GetSocket().GetHost().c_str(), pClient->GetSocket().GetPort());
		return false;
	}
	return true;
}

void CTCPServer::FreeClosingSocket()
{
	for (auto it = m_ConnMap.begin(); it != m_ConnMap.end();)
	{
		CSocket tmSocket = it->second->GetSocket();
		if (it->second->GetStatus() == eSocketClosing)
		{
			CACHE_LOG(TCP_DEBUG, "FreeClosingSocket tcpconn socket fd = {} ,host = {},port = {},connKey = {},connInfo = {}",
				 tmSocket.GetSocket(), tmSocket.GetHost().c_str(), tmSocket.GetPort(),it->second->ConnKey(),it->second->ConnInfo());

#ifdef __LINUX__
			if (m_nRunModule == eTcpEpoll)
			{
				EpollDelSocket(tmSocket.GetSocket());
			}
#endif		
			m_ConnSecondMap.erase(it->second->ConnKey());
			it->second->Close();
			//回收内存
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
		CSocket tmSocket = it->second->GetSocket();
		if (it->second->GetStatus() == eSocketClosing)
		{
			CACHE_LOG(TCP_DEBUG, "FreeClosingSocket tcp client socket fd = {} ,host = {},port = {}", tmSocket.GetSocket(), tmSocket.GetHost().c_str(), tmSocket.GetPort());
#ifdef __LINUX__
			if (m_nRunModule == eTcpEpoll)
			{
				EpollDelSocket(tmSocket.GetSocket());
			}
#endif
			it->second->Close();
			it->second.Free();
			it = m_ClientMap.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CTCPServer::KickIllegalConn(time_t nNowAns)
{
	ConnMap::iterator it = m_ConnMap.begin();
	for (; it != m_ConnMap.end();)
	{
		CSocket& tmSocket = it->second->GetSocket();
		SOCKET nSocketFD = tmSocket.GetSocket();
		if (it->second->GetStatus() == eSocketClosing)
		{
			CACHE_LOG(TCP_DEBUG, "KickIllegalConn Conn socket fd = {} ,host = {},port = {},connKey = {},connInfo = {}", 
				tmSocket.GetSocket(), tmSocket.GetHost().c_str(), tmSocket.GetPort(),it->second->ConnKey(),it->second->ConnInfo());
			m_ConnSecondMap.erase(it->second->ConnKey());
			it->second->Close();
			it->second.Free();
			it = m_ConnMap.erase(it);
			continue;
		}
		if (nNowAns - it->second->GetLastRecvHeartbeatTime() > TCP_CONN_TIME_OUT)
		{
			CACHE_LOG(TCP_DEBUG, "KickIllegalConn timeount Conn socket fd = {} ,host = {},port = {},connKey = {},connInfo = {}", 
				tmSocket.GetSocket(), tmSocket.GetHost().c_str(), tmSocket.GetPort(),it->second->ConnKey(),it->second->ConnInfo());
			m_ConnSecondMap.erase(it->second->ConnKey());
			it->second->Close();
			it->second.Free();
			it = m_ConnMap.erase(it);
			continue;
		}
		it++;
	}
}

bool CTCPServer::BeginListen(std::string addrress, int port)
{
	//不进行监听
	if (port <= 0)
	{
		return true;
	}

	if (!m_ListenSocket.IsValid())
	{
		if (!m_ListenSocket.Open())
		{
			return false;
		}
	}
	if(!m_ListenSocket.SetSocketNoBlock())return false;
	if(!m_ListenSocket.SetKeepAlive()) return false;
	if(!m_ListenSocket.SetLinger(0)) return false;
	if(!m_ListenSocket.SetTcpNoDelay()) return false;
	if(!m_ListenSocket.SetReuseAddr())  return false;
	if(!m_ListenSocket.SetSendBufSize(1024 * 1024)) return false;
	if(!m_ListenSocket.SetRecvBufSize(1024 * 1024)) return false;

	bool bRet = 0;
	if (addrress != "")
	{
		bRet = m_ListenSocket.Bind(port);
	}
	else
	{
		bRet = m_ListenSocket.Bind(addrress.c_str(), port);
	}

	if (!bRet) return false;

	if (!m_ListenSocket.Listen()) return false;

	CACHE_LOG(TCP_DEBUG, "CTCPServer::BeginListen listen {} : {},fdsize = {}.", addrress, port, FD_SETSIZE);
	return true;
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
			CACHE_LOG(TCP_ERROR, "epoll_create Error : {}", strerror(socket_error));
			return -1;
		}

		return 0;
	}

	if(ip != NULL && strlen(ip) > 0 && port > 0)
	{
		if (!BeginListen(ip, port))
		{
			ClearEpoll();
			return -2;
		}
	}

	if (m_ListenSocket.IsValid())
	{
		if (epoll_ctl(m_ListenSocket.GetSocket(), EPOLL_CTL_ADD, sfd, &m_stEpollEvent) < 0)
		{
			ClearEpoll();
			m_ListenSocket.Close();
			return -3;
		}
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
			CACHE_LOG(TCP_ERROR, "epoll_wait Error cevents->data.fd = {}!", cevents->data.fd);
			continue;
		}

		if (0 != (EPOLLERR & cevents->events))
		{
			CACHE_LOG(TCP_ERROR, "epoll_wait Error cevents->events generate error event {}",EPOLLERR & cevents->events);
			continue;
		}
		
		//监听描述符
		if (nTmFd == m_ListenSocket.GetSocket())
		{
			CSocket tmConnSocket = m_ListenSocket.Accept();
			if (!tmConnSocket.IsValid())
			{
				// 客户端连接上来以后又立即关闭了
				CACHE_LOG(TCP_ERROR, "Accept  invalid socket socket_error({} : {})",socket_error, strerror(socket_error));
				continue;
			}

			if (!tmConnSocket.SetSocketNoBlock())
			{
				CACHE_LOG(TCP_ERROR, "Accept error: socket set noblock failed ,fd = {},host = {},port = {}", newSocket.GetSocket(), newSocket.GetHost().c_str(), newSocket.GetPort());
				tmConnSocket.Close();
				continue;
			}
			//新的连接
			OnAccept(tmConnSocket);
		//可读
		}else if(EPOLLIN & cevents->events)
		{
			ClientMap::iterator itcleint = m_ClientMap.find(nTmFd);
			if (itcleint != m_ClientMap.end())
			{
				TcpClientDoRecv(itcleint->second);
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itconn != m_ConnMap.end())
			{
				TcpConnDoRecv(itconn->second);
				continue;
			}
		}
		//可写
		else if (EPOLLOUT & cevents->events)
		{
			ClientMap::iterator itcleint = m_ClientMap.find(nTmFd);
			if (itcleint != m_ClientMap.end())
			{
				TcpClientDoFlush(itcleint->second);
				continue;
			}
			ConnMap::iterator itconn = m_ConnMap.find(nTmFd);
			if (itconn != m_ConnMap.end())
			{
				TcpConnDoFlush(itconn->second);
				continue;
			}
		}
	}
	return;
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

