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
	//接受客户端数据
	void RecvClientData(CSafePtr<CProxyPlayer> pGamePlayer);
	//发送消息给client
	void RecvGameData();
	//向game 发送消息
	int SendToGame(char* data, int iTmpLen);
	//向client下行数据包
	//int SendToClient(CMessG2G& tmpMes);
private:
	//给特定client发送数据
	//void SendToClient(const CSocketInfo& socketInfo, const char* data, unsigned int len);
public:
	//新链接回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket);
private:
	//gateserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_C2SCodeQueue;
	//gameserver ==> gateserver
	CSafePtr<CShmMessQueue>	m_S2CCodeQueue;
	CSafePtr<CByteBuff>		m_pRecvBuff;
	BYTE						m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};

#endif //__GATE_SERVER_H__

