//
//  gameserver.h
//  gameserver 头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H

#include "singleton.h"
#include "runflag.h"
#include "base.h"
#include "message_interface.h"
#include "message_dispatcher.h"
#include "tcp_server.h"
#include "game_player.h"
#include "client_handle.h"
#include "server_handle.h"
#include "config_handle.h"

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
	int PrepareToRun();
	// 运行
	void Run();
	// 退出
	void Exit();
	//读取客户端上行数据
	void RecvClientData(SafePointer<CGamePlayer> pGamePlayer);
	//
	void ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//
	void DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError);
public:
	//新链接回调
	virtual void OnNewConnect(SafePointer<CTCPConn> pConnn);
	//
	virtual SafePointer<CTCPConn> CreateTcpConn(CSocket tmSocket);
	//
	virtual SafePointer<CTCPClient> CreateTcpClient(CSocket tmSocket);

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

	// 处理客户端上行消息
	void ProcessClientMessage(SafePointer<CMessage> pMsg, SafePointer<CGamePlayer> pPlayer);
	// 处理服务器内部消息
	void ProcessRouterMessage(CProxyMessage *pMsg);

	// 给DB Server发消息
	bool SendMessageToDB(CProxyMessage *pMsg);
	// 给World Server 发消息

	// 广播消息给玩家，广播时，发起人一定放第一个
	int BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, stPointList *pTeamList);
	// 推送消息给单个玩家
	int BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer);
	// 回复客户端上行的请求
	int SendResponse(std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer);
	int SendResponse(std::shared_ptr<CGooMess> pMsgPara, std::shared_ptr<CMesHead> mesHead);

	// 通过消息ID获取模块类型
	int GetModuleClass(int iMsgID);

	// 主动断开链接
	void DisconnectClient(CPlayer *pPlayer);

	// 设置服务器运行状态
	void SetRunFlag(ERunFlag eRunFlag);
	// 检查服务器状态
	int CheckRunFlags();
	// perf日志打印
	static void OnTimePerfLog(CTimerBase *pTimer);

	// 存储所有玩家数据(一般故障拉起或程序退出时使用)
	void SaveAllTeamdata();
	// 检测是否所有玩家存储结束
	static void CheckSaveAllTeamsFinish(CTimerBase *pTimer);

	// 拉取服务器数据
	void StartLoadAllData();
	// 检测是否开启拉取服务器数据
	static void CheckStartLoadAllData(CTimerBase *pTimer);
	// 存储服务器数据
	void StartSaveAllData();
	// 检测是否停服存储数据
	static void CheckStartSaveAllData(CTimerBase *pTimer);
	// 获取服务器ID
	int InitStaticLog();
	// 限制玩家登陆
	int LimitTeamLogin(unsigned int iTeamID, time_t iTimes); // itimes 暂定为小时
	CRunFlag &GetRunFlag();
	int GetMiServerState();
public:
	// 为找不到CTeam的连接发送消息
	void SendMsgSystemErrorResponse(int iResult,
									long lMsgGuid,
									int iServerID,
									time_t tCreateTime,
									bool bKickOff = false);
	CRunFlag	m_oRunFlag;                         // 服务器运行状态
	int			m_iServerState;    // 服务器状态
	BYTE		m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};
#endif //SERVER_GAMESERVER_H
