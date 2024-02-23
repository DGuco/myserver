/*****************************************************************
* FileName:gate_server.h
* Summary :
* Date	  :2024-2-22
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "tcp_server.h"
#include "server_tool.h"
#include "message.pb.h"
#include "game_player.h"

class CGateServer : public CTCPServer,public CSingleton<CGateServer>
{
public:
	//构造函数
	CGateServer();
	//析构函数
	virtual ~CGateServer();
public:
	//准备run
	bool PrepareToRun();
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
	//清除socket
	void ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//通知gameserver client 断开连接
	void DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//接受客户端数据
	void RecvClientData(SafePointer<CGamePlayer> pGamePlayer);
public:
	//新链接回调
	virtual void OnNewConnect(SafePointer<CTCPConn> pConnn);
	//
	virtual SafePointer<CTCPConn> CreateTcpConn(CSocket tmSocket);
	//
	virtual SafePointer<CTCPClient> CreateTcpClient(CSocket tmSocket);
private:
	BYTE m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};

#endif //__GATE_SERVER_H__

