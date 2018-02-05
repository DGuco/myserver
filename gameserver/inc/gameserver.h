//
//  gameserver.h
//  gameserver 头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H

#include "server_tool.h"
#include "runflag.h"
#include "base.h"
#include "timer.h"
#include "message_interface.h"
#include "message.pb.h"
#include "dbmessage.pb.h"
#include "messagedispatcher.h"
#include "clienthandle.h"
#include "net_work.h"
#include "../datamodule/inc/player.h"
#include "../logicmodule/inc/modulemanager.h"

class CGameServer: public CSingleton<CGameServer>
{
public:
	enum
	{
		MAX_CHECK_CLIENT_MSG = 2000,        // 每次循环收取客户端消息最大数
		MAX_CHECK_SERVER_MSG = 2000,        // 每次循环收取服务器消息最大数
//		KEEPALIVE_TIME			= 30000,	// 每隔30秒发送一次keepalive消息(单位 : 毫秒)
//		KEEPALIVE_TIMEOUT		= 30000,	// 30秒没收到回复即为超时(单位 : 毫秒)
	};

	enum EServerState
	{
		ESS_CONNECTPROXY = 0x0001,    // 链接proxy
		ESS_LOADDATA = 0x0002,    // 加载数据
		ESS_PROCESSINGDATA = 0x0004,    // 处理数据
		ESS_SAVEDATA = 0x0008,    // 停服存储数据

		ESS_NORMAL = (ESS_CONNECTPROXY | ESS_LOADDATA | ESS_PROCESSINGDATA),        // 正常
		ESS_PROCESSINGCLIENTMSG = (ESS_LOADDATA | ESS_PROCESSINGDATA)    // 可以正常处理客户端上行的消息
	};

public:
	CGameServer();
	~CGameServer();

	// 初始化
	int Initialize();
	// 运行准备
	int PrepareToRun();
	// 运行
	void Run();
	// 退出
	void Exit();

public:
	// 设置服务器状态
	void SetServerState(int iState)
	{ miServerState = (miServerState | iState); }
	// 清除服务器状态
	void EraseServerState(int iState)
	{ miServerState = (miServerState & (~iState)); }
	// 判断服务器状态
	bool IsOk(int iState)
	{ return ((miServerState & iState) == iState); }
	// 服务器是否正常
	bool IsNormal()
	{ return miServerState == ESS_NORMAL; }

	// 服务器拉取数据完成
	void LoadDataFinish();
	// 服务器开始处理初始数据
	void StartProcessingInitData();

	// 是否能正常处理客户端上行消息
	bool CanProcessingClientMsg()
	{ return (miServerState & ESS_PROCESSINGCLIENTMSG) == ESS_PROCESSINGCLIENTMSG; }
	// 创建实体
	int CreateEntity(CPlayer *pPlayer);
	// 销毁实体
	void DestroyEntity(CPlayer *pPlayer);

	// 处理客户端上行消息
	void ProcessClientMessage(CMessage *pMsg, CPlayer *pPlayer);
	// 处理服务器内部消息
	void ProcessRouterMessage(CProxyMessage *pMsg);

	// 给DB Server发消息
	bool SendMessageToDB(CProxyMessage *pMsg);
	// 给World Server 发消息

	// 收取客户端消息
	int RecvClientMsg(time_t tTime);
	// 收取服务器消息
	int RecvServerMsg(time_t tTime);
	// 广播消息给玩家，广播时，发起人一定放第一个
	int Push(unsigned int iMsgID, Message *pMsg, stPointList *pTeamList);
	// 推送消息给单个玩家
	int Push(unsigned int iMsgID, Message *pMsgPara, CPlayer *pPlayer);
	// 回复客户端上行的请求
	int SendResponse(Message *pMsgPara, CPlayer *pPlayer);
	int SendResponse(Message *pMsgPara, MesHead *mesHead);

	// 通过消息ID获取模块类型
	int GetModuleClass(int iMsgID);

	// 主动断开链接
	void DisconnectClient(CPlayer *pPlayer);

	// 连接到Proxy
	bool Connect2Proxy();
	// 向Proxy注册
	bool Regist2Proxy();
	// 发送心跳到Proxy
	bool SendKeepAlive2Proxy();

	// 设置服务器运行状态
	void SetRunFlag(ERunFlag eRunFlag);
	// 检查服务器状态
	void CheckRunFlags();
	// 刷新服务器状态
	void FreshGame();

	// 读取配置
	int ReadCfg();

	// 根据配置初始化信息
	int InitializeByConfig();

	// 开启所有定时器
	int StartAllTimers();
	// 服务器间心跳检测
	static void OnTimeCheckStateInServer(CTimerBase *pTimer);
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
public:
	static void lcb_OnConnectted(CConnector *pConnector);
	//断开连接回调
	static void lcb_OnCnsDisconnected(CConnector *pConnector);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(CConnector *pConnector);
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnCnsSomeDataSend(CConnector *pConnector);
	static void lcb_OnPingServer(CConnector *pConnector);
private:
	CClientHandle *m_pClientHandle;            // 与客户端通信的连接,需要new在共享内存上的
	CModuleManager *m_pModuleManager;            // 模块管理器
	CMessageDispatcher *m_pMessageDispatcher;        // 消息派发器
	CFactory *m_pMessageFactory;            // 消息工厂
	CTimerManager *m_pTimerManager;            // 定时器管理器
	CRunFlag m_RunFlag;                    // 服务器运行状态
	CNetWork *m_pNetWork;                // 服务器间通信的连接

	int miServerState;    // 服务器状态
	unsigned long long mLastTickCount; // tick
public:
	const CNetWork *GetNetWork();

	// 为找不到CTeam的连接发送消息
	void SendMsgSystemErrorResponse(int iResult,
									long lMsgGuid,
									int iServerID,
									time_t tCreateTime,
									bool bKickOff = false);
};
#endif //SERVER_GAMESERVER_H
