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
	bool SelectTick();
	//
	bool EpollTick();

	bool InitTcpServer(const char* ip, int port);
public:
	virtual SafePointer<CTCPClient> CreateTcpClient() = 0;
	virtual SafePointer<CTCPConn> CreateTcpConn() = 0;
private:
	CSocket			     m_ListenSocket;
	eTcpServerModule     m_nRunModule;
	CString<ADDR_LENGTH> m_IPAddr;
	int					 m_nPort;
	ClientMap			 m_ClientMap;
	ConnMap				 m_ConnMap;
};

#endif //__TCP_SERVER_H__
