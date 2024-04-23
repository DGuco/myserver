/*****************************************************************
* FileName:gate_server.h
* Summary :
* Date	  :2024-2-22
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__
#include "safe_pointer.h"
#include "tcp_server.h"
#include "singleton.h"
#include "message.pb.h"
#include "proxy_player.h"
#include "common_def.h"
#include "shm_queue.h"

class CProxyServer : public CTCPServer,public CSingleton<CProxyServer>
{
public:
	//构造函数
	CProxyServer();
	//析构函数
	virtual ~CProxyServer();
public:
	//准备run
	bool PrepareToRun();
	//清除socket
	void ClearSocket(CSafePtr<CProxyPlayer> pGamePlayer, short iError);
	//通知gameserver client 断开连接
	void DisConnect(CSafePtr<CProxyPlayer> pGamePlayer, short iError);
	//
	void RecvMessage(CSafePtr<CProxyPlayer> pGamePlayer);
	//
	void RegisterNewConn(CSafePtr<CProxyPlayer> pGamePlayer);
	//
	CSafePtr<CProxyPlayer> FindProxyPlayer(int servertype, int serverid);
	//
	void TransferMessage(CSafePtr<CProxyPlayer> pGamePlayer,int servertype, int serverid, shared_ptr<ProxyMessage> pMessage);
public:
	//新链接回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket socket);
private:
	typedef std::unordered_map<int, CSafePtr<CProxyPlayer>> ConnMap;
	ConnMap					m_ConnMap;
	BYTE					m_CacheData[MAX_PACKAGE_LEN];
};

#endif //__GATE_SERVER_H__

