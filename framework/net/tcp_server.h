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

enum eTcpServerModule
{
	eTcpSelect = 0,
	eTcpEpoll = 1,
};


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
	void Run();
	//
	int GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr);
	//
	u_long GetConnAddr();
	//
	u_short GetConnPort();
	//
	short  GetEntityType();
	//
	short  GetEntityID();
	//
	CTCPSocket& GetSocket();

public:
	virtual SafePointer<CTCPClient> CreateTcpClient() = 0;
	virtual SafePointer<CTCPClient> CreateTcpConn() = 0;
private:
private:
	CTCPSocket			 m_Socket;
	eTcpServerModule     m_nRunModule;
	CString<ADDR_LENGTH> m_IPAddr;
	int					 m_nPort;
};

#endif //__TCP_SERVER_H__
