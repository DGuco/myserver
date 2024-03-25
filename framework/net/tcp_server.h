/*****************************************************************
* FileName:tcp_server.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "t_array.h"
#include "tcp_socket.h"
#include "tcp_conn.h"
#include "tcp_client.h"
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
	virtual ~CTCPServer();
	//
	int InitTcpServer(eTcpServerModule module,const char* ipAddr = "", u_short unPort = 0);
	//
	bool ConnectTo(CSafePtr<CTCPClient> pClient,const char* szLocalAddr,int port,bool bblock);
	//
	bool TcpTick();
	//
	CSafePtr<CTCPConn>     FindTcpConn(SOCKET socket);
	//
	CSafePtr<CTCPClient>   FindTcpClient(SOCKET socket);
private:
	//
	int PrepareToRun();
	//
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn) = 0;
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket) = 0;
private:
	//
	int InitSelect(const char* ip, int port);
	//
	int SelectTick();
	//
	void FreeClosedSocket();
#ifdef __LINUX__
	//
	int InitEpoll(const char* ip, int port);
	//
	int EpollTick();
	//
	int EpollDelSocket(SOCKET socket);
	//
	int EpollAddSocket(SOCKET socket);
	//
	int ClearEpoll();
#endif
private:
	CSocket			     m_ListenSocket;
	eTcpServerModule     m_nRunModule;
	CString<ADDR_LENGTH> m_IPAddr;
	int					 m_nPort;
	ClientMap			 m_ClientMap;
	ConnMap				 m_ConnMap;
	fd_set				 m_fdsRead;
	fd_set				 m_fdsWrite;
#ifdef __LINUX__
	struct epoll_event*	 m_pEpollEventList;
	int                  m_nEpollFd;
#endif
};

#endif //__TCP_SERVER_H__
