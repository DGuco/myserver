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
#include "common_def.h"
#include "safe_pointer.h"
#include "tcp_server.h"
#include "game_player.h"
#include "server_client.h"

class CGameServer: public CTCPServer,public CSingleton<CGameServer>
{
public:
	enum EServerState
	{
		ESS_CONNECTPROXY = 0x0001,    // 链接proxyl
		ESS_LOADDATA = 0x0002,    // 加载数据
		ESS_PROCESSINGDATA = 0x0004,    // 处理数据
		ESS_SAVEDATA = 0x0008,    // 停服存储数据

		ESS_NORMAL = (ESS_CONNECTPROXY | ESS_LOADDATA | ESS_PROCESSINGDATA),        // 正常
		ESS_PROCESSINGCLIENTMSG = (ESS_LOADDATA | ESS_PROCESSINGDATA)    // 可以正常处理客户端上行的消息
	};

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
public:
	//新链接回调
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket);
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
	// 服务器是否正常
	bool IsNormal()
	{ return m_iServerState == ESS_NORMAL; }

	// 服务器拉取数据完成
	void LoadDataFinish();
	// 服务器开始处理初始数据
	void StartProcessingInitData();

	// 是否能正常处理客户端上行消息
	bool CanProcessingClientMsg()
	{ return (m_iServerState & ESS_PROCESSINGCLIENTMSG) == ESS_PROCESSINGCLIENTMSG; }

	// 给DB Server发消息
	//bool SendMessageToDB(CSafePtr <CProxyMessage> pMsg);
	// 通过消息ID获取模块类型
	int GetModuleClass(int iMsgID);
private:

	CRunFlag	m_oRunFlag;                         // 服务器运行状态
	int			m_iServerState;    // 服务器状态
	BYTE		m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};
#endif //SERVER_GAMESERVER_H
