//
//  gameserver.h
//  gameserver 头文件
//  Created by DGuco on 17-3-1.
//  Copyright ? 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H
#include "base.h"
#include "singleton.h"
#include "runflag.h"
#include "tcp_def.h"
#include "safe_pointer.h"
#include "tcp_server.h"
#include "game_player.h"
#include "server_client.h"
#include "time_helper.h"
#include "shm_queue.h"

class CGameServer: public CTCPServer,public CSingleton<CGameServer>
{
public:
	CGameServer();
	~CGameServer();
	// 运行准备
	bool PrepareToRun();
	// 退出
	void Exit();
	//处理客户端上行数据
	void ProcessClientMessage(CSafePtr<CGamePlayer> pGamePlayer);
	//处理读取服务器数据
	void ProcessServerMessage(CSafePtr<CServerClient> pServerPlayer);
	//
	void ClearSocket(CSafePtr<CGamePlayer> pGamePlayer, short iError);
	//
	void DisConnect(CSafePtr<CGamePlayer> pGamePlayer, short iError);
	//
	void RecvDBMessage();
	//
	int  SendMessageToDB(char* data, int iTmpLen);
public:
	//新链接回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket socket);
public:
	// 设置服务器状态
	void SetServerState(int iState)
	{ m_iServerState = (m_iServerState | iState); }
	// 清除服务器状态
	void EraseServerState(int iState)
	{ m_iServerState = (m_iServerState & (~iState)); }
	// 判断服务器状态
	bool IsOk(int iState)
	{ return ((m_iServerState & iState) == iState); }
	// 服务器拉取数据完成
	void LoadDataFinish();
	// 服务器开始处理初始数据
	void StartProcessingInitData();
	// 给DB Server发消息
	//bool SendMessageToDB(CSafePtr <CProxyMessage> pMsg);
	// 通过消息ID获取模块类型
	int GetModuleClass(int iMsgID);
private:
	CSafePtr<CByteBuff>		m_pRecvBuff;
	//gateserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_DB2SCodeQueue;
	//gameserver ==> gateserver
	CSafePtr<CShmMessQueue>	m_S2DBCodeQueue;
	CRunFlag				m_oRunFlag;                         // 服务器运行状态
	int						m_iServerState;    // 服务器状态
	BYTE					m_CacheData[MAX_PACKAGE_LEN];
};
#endif //SERVER_GAMESERVER_H
