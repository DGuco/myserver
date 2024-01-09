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

# define MAX_SOCKET_NUM 65536

typedef std::unordered_map<int, SafePointer<CTCPClient>> ClientMap;
typedef std::unordered_map<int, SafePointer<CTCPConn>>   ConnMap;

class CTCPServer
{
public:
	//
	CTCPServer(eTcpServerModule module, unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPServer();
	//
	int InitServer(const char* ipAddr, u_short unPort);
	//
	int ConnectTo(const char* szLocalAddr,int port, bool bblock);
	//
	int CreateServer();
	//
	bool Run();

	CTCPSocket& GetSocket();
private:
	//
	int InitSelect(const char* ip, int port);
	//
	int SelectTick();
#ifdef __LINUX__
	//
	int InitEpoll(const char* ip, int port);
	//
	int EpollTick();
	//
	int EpollDelSocket(CTCPSocket* pSocket);
	//
	int ClearEpoll();
#endif
public:
	virtual SafePointer<CTCPClient> CreateTcpClient(CSocket tmSocket) = 0;
	virtual SafePointer<CTCPConn> CreateTcpConn(CSocket tmSocket) = 0;
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
