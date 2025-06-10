/*****************************************************************
* FileName:gate_server.h
* Summary :
* Date	  :2024-2-22
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__
#include "safe_pointer.h"
#include "tcp_server.h"
#include "singleton.h"
#include "message.pb.h"
#include "proxy_conn.h"
#include "shm_queue.h"

class CProxyServer : public CTCPServer,public CSingleton<CProxyServer>
{
public:
	//构造函数
	CProxyServer();
	//析构函数
	virtual ~CProxyServer();
public:
	//InnitTcp
	bool InitTcp();
	//通知gameserver client 断开连接
	void RemoveConnect(CSafePtr<CProxyConn> pGamePlayer, short iError);
	//
	void ProcessServerMessage(CSafePtr<CProxyConn> pGamePlayer);
	//
	void RegisterNewConn(CSafePtr<CProxyConn> pGamePlayer);
	//
	CSafePtr<CProxyConn> FindProxyPlayer(int servertype, int serverid);
	//
	void TransferMessage(CSafePtr<CProxyConn> pGamePlayer,int servertype, int serverid, shared_ptr<ProxyMessage> pMessage);
	//
	void CheckKickConn(time_t now);
	//新的连接来了
	virtual void OnAccept(CSocket newSocket);
public:
	//新链接回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket socket);
private:
	typedef std::unordered_map<int, CSafePtr<CProxyConn>> ConnMap;
	ConnMap					m_ConnMap;
	BYTE					m_CacheData[MAX_PACKAGE_LEN];
};

#endif //__GATE_SERVER_H__

