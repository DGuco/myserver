/*****************************************************************
* FileName:tcp_server.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "t_array.h"
#include "tcp_socket.h"
#include "tcp_conn.h"
#include "tcp_client.h"
#include "my_lock.h"
#include <unordered_map>

enum eTcpServerModule
{
	eTcpSelect = 0,
	eTcpEpoll = 1,
};

typedef std::unordered_map<SOCKET, CSafePtr<CTCPConn>>   ConnMap;
typedef std::unordered_map<SOCKET, CSafePtr<CTCPClient>> ClientMap;

class CTCPServer
{
public:
	//
	CTCPServer();
	//
	CTCPServer(eTcpServerModule module);
	//
	virtual ~CTCPServer();
	//
	int InitTcpServer(eTcpServerModule module,const char* ipAddr = "", u_short unPort = 0);
	//
	bool ConnectTo(CSafePtr<CTCPClient> pClient,const char* szLocalAddr,int port,bool bblock);
	//
	void TcpTick(time_t now);
	//
	void KickIllegalConn();
	//
	CSafePtr<CTCPConn>     FindTcpConn(SOCKET socket);
	//
	CSafePtr<CTCPClient>   FindTcpClient(SOCKET socket);
	//添加一个新来的连接
	void AddNewIncomingConn(CSocket newSocket);
private:
	//socket tick
	void SocketTick(time_t now);
	//socket tick
	void CheckSocketResize();
	//
	int PrepareToRun();
	// 注册新的连接
	void AcceptIncomingConnect();
	//新的连接来了
	virtual void OnAccept(CSocket newSocket);
private:
	//连接成功回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn) = 0;
	//创建新的连接
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket) = 0;
private:
	//
	int InitSelect(const char* ip, int port);
	//
	void SelectTick();
	//
	void FreeClosingSocket();
	//
	bool BeginListen(std::string addrress, int port);
	//
	bool TcpConnDoRecv(CSafePtr<CTCPConn> pConn);
	//
	bool TcpConnDoFlush(CSafePtr<CTCPConn> pConn);
	//
	bool TcpClientDoRecv(CSafePtr<CTCPClient> pClient);
	//
	bool TcpClientDoFlush(CSafePtr<CTCPClient> pClient);
#ifdef __LINUX__
	//
	int InitEpoll(const char* ip = NULL, int port = -1);
	//
	void EpollTick();
	//
	int EpollDelSocket(SOCKET socket);
	//
	int EpollAddSocket(SOCKET socket);
	//
	int ClearEpoll();
private:
	struct epoll_event*	 m_pEpollEventList;
	int                  m_nEpollFd;
#endif
private:
	//其他监听者的socket
	std::list<CSocket>   m_ConnectingList;
	CMyLock			     m_ConnectingListLock;
private:
	CSocket			     m_ListenSocket;
	eTcpServerModule     m_nRunModule;
	CString<ADDR_LENGTH> m_IPAddr;
	int					 m_nPort;
	ClientMap			 m_ClientMap;
	ConnMap				 m_ConnMap;
	fd_set				 m_fdsRead;
	fd_set				 m_fdsWrite;
};

#endif //__TCP_SERVER_H__
