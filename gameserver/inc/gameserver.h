//
//  gameserver.h
//  gameserver 头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H

#ifndef GAMESERVER_H_
#define GAMESERVER_H_


#include "../../framework/base/servertool.h"
#include "../../framework/net/runflag.h"
#include "../../framework/base/base.h"
#include "../../framework/net/tcp_conn.h"
//#include "../../framework/timer/timer.h"
//#include "message_interface.h"
//#include "shm.h"
//#include "tlogmodule.h"

class CMessageSet;
class CMessage;
class CCSHead;
class CProxyHead;
class CCoreModule;
class CClientHandle;
class CSceneObjManager;
class CModuleManager;
class CMessageDispatcher;
class CFactory;
class CTeam;
//class CEntity;


#define MAX_PROXY_NUM	(2)	// proxy服务器的最大数量

typedef CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH> MyTCPConn;

class CProxyClient : public MyTCPConn
{
public:
    CProxyClient() {}
    ~CProxyClient() {}

protected:
    // 上次keepalive时间
    CWTimer m_LastKeepaliveTime;

public:
    CWTimer* GetLastKeepaliveTimePtr() {return &m_LastKeepaliveTime;}

    void InitTimer(time_t tTime) {m_LastKeepaliveTime.Initialize(tTime);}
    bool IsTimeout(time_t tNow) {return m_LastKeepaliveTime.IsTimeout(tNow);}
    void ResetTimeout(time_t tTime) {m_LastKeepaliveTime.ResetTimeout(tTime);}

    bool IsConnected() {return GetStatus() == tcs_connected ? true : false;}
    int GetStatus() {return GetSocket()->GetStatus();}
    int SendOneCode(unsigned short nCodeLength, BYTE *pCode) {return GetSocket()->SendOneCode(nCodeLength, pCode);}
    int GetSocketFD() {return GetSocket()->GetSocketFD();}
    int RecvData() {return GetSocket()->RecvData();}
    int GetOneCode(unsigned short &nCodeLength, BYTE *pCode) {return GetSocket()->GetOneCode(nCodeLength, pCode);}
};

class CGameServer : public CSingleton<CGameServer>
{
public:
    enum
    {
        MAX_CHECK_CLIENT_MSG	= 2000,		// 每次循环收取客户端消息最大数
        MAX_CHECK_SERVER_MSG	= 2000,		// 每次循环收取服务器消息最大数
//		KEEPALIVE_TIME			= 10000,	// 每隔30秒发送一次keepalive消息(单位 : 毫秒)
//		KEEPALIVE_TIMEOUT		= 30000,	// 30秒没收到回复即为超时(单位 : 毫秒)
    };

    enum EServerState
    {
        ESS_CONNECTPROXY					= 0x0001,	// 链接proxy
        ESS_LOADDATA								= 0x0002,	// 加载数据
        ESS_PROCESSINGDATA				= 0x0004,	// 处理数据
        ESS_SAVEDATA								= 0x0008,	// 停服存储数据

        ESS_NORMAL								= (ESS_CONNECTPROXY | ESS_LOADDATA|ESS_PROCESSINGDATA),		// 正常
        ESS_PROCESSINGCLIENTMSG		= (ESS_LOADDATA|ESS_PROCESSINGDATA)	// 可以正常处理客户端上行的消息
    };

public:
    CGameServer();
    ~CGameServer();

    // 初始化
    int Initialize();
    int Resume();
    // 运行准备
    int PrepareToRun();
    // 运行
    void Run();
    // 退出
    void Exit();

public:
    static CSharedMem* mShmPtr;
    static size_t CountSize();

    void* operator new(size_t size) {return (void*) CGameServer::mShmPtr->CreateSegment(size);}
    void  operator delete(void* pointer) {}

public:
    // 设置服务器状态
    void SetServerState(int iState) {mServerState = (mServerState | iState);}
    // 清除服务器状态
    void EraseServerState(int iState) {mServerState = (mServerState & (~iState));}
    // 判断服务器状态
    bool IsOk(int iState) {return ((mServerState & iState) == iState);}
    // 服务器是否正常
    bool IsNormal() {return mServerState == ESS_NORMAL;}

    // 服务器拉取数据完成
    void LoadDataFinish();
    // 服务器开始处理初始数据
    void StartProcessingInitData();

    // 是否能正常处理客户端上行消息
    bool CanProcessingClientMsg() {return (mServerState &  ESS_PROCESSINGCLIENTMSG) == ESS_PROCESSINGCLIENTMSG;}
    // 创建实体
    int	CreateEntity(CTeam* pTeam);
    // 销毁实体
    void DestroyEntity(CTeam* pTeam);

    // 处理客户端上行消息
    void ProcessClientMessage(CCSHead* pHead, CMessage* pMsg, CTeam* pTeam);
    // 处理服务器内部消息
    void ProcessRouterMessage(CMessage* pMsg);

    // 给DB Server发消息
    bool SendMessageToDB(CMessage* pMsg);
    // 给World Server 发消息
    bool SendMessageToWorld(CMessage* pMsg);
    // 给Web Server 发消息
    bool SendMessageToWeb(CMessage* pMsg);
    // 给Offline Server 发消息
    bool SendMessageToOffline(CMessage* pMsg);
    // 给check Server 发消息
    bool SendMessageToCheck(CMessage* pMsg);

    // 收取客户端消息
    int RecvClientMsg(time_t tTime);
    // 收取服务器消息
    int RecvServerMsg(time_t tTime);
    // 广播消息给玩家，广播时，发起人一定放第一个
    int SendTeam(CMessageSet* pMsgSet, stPointList* pTeamList);
    // 发送消息给单个玩家
    int SendTeam(CMessageSet* pMsgSet, CTeam* pTeam);
    int SendTeam(unsigned int iMsgID, Message* pMsgPara, CTeam* pTeam);
    // 组合消息
    int AddMsgSet(CMessageSet* pMsgSet, unsigned int iMsgID, Message* pMsgPara);


    // 通过消息ID获取模块类型
    int GetModuleClass(int iMsgID);

    // 主动断开链接
    void DisconnectClient(CTeam* pTeam);

    // 连接到Proxy
    bool Connect2Proxy(int iIndex);
    // 向Proxy注册
    bool Regist2Proxy(int iIndex);
    // 发送心跳到Proxy
    bool SendKeepAlive2Proxy(int iIndex);

    // 设置服务器运行状态
    void SetRunFlag(ERunFlag eRunFlag);
    // 检查服务器状态
    void CheckRunFlags();
    // 刷新服务器状态
    //void FreshGame();

    // 读取配置
    int ReadCfg();
//	// 获取配置信息
//	CConfigure* GetConfig() {return &mConfig;}

    // 根据配置初始化信息
    int	InitializeByConfig();

    // 开启所有定时器
    int StartAllTimers();
    // 服务器间心跳检测
    static void OnTimeCheckStateInServer(CTimerBase* pTimer);
    // perf日志打印
    static void OnTimePerfLog(CTimerBase* pTimer);

    // 存储所有玩家数据(一般故障拉起或程序退出时使用)
    void SaveAllTeamdata();
    // 检测是否所有玩家存储结束
    static void CheckSaveAllTeamsFinish(CTimerBase* pTimer);

    // 拉取服务器数据
    void StartLoadAllData();
    // 检测是否开启拉取服务器数据
    static void CheckStartLoadAllData(CTimerBase* pTimer);
    // 存储服务器数据
    void StartSaveAllData();
    // 检测是否停服存储数据
    static void CheckStartSaveAllData(CTimerBase* pTimer);
    // 获取服务器ID
    int InitStaticLog();
    // 限制玩家登陆
    int LimitTeamLogin( unsigned int iTeamID, time_t iTimes ); // itimes 暂定为小时

protected:
    // 需要new在共享内存上的
    CClientHandle* 				mpClientHandle;										// 与客户端通信的连接
    CSceneObjManager* 		mpSceneObjManager;							// 场景对象管理器
    CModuleManager* 			m_pModuleManager;								// 模块管理器

    // 不需要new在共享内存上的
    CTimerManager*				mpTimerManager;				// 定时器管理器
    CMessageDispatcher*		mpMessageDispatcher;		// 消息派发器
    CFactory*							mpMessageFactory;				// 消息工厂
    CTemplateMgr*				mpTemplateMgr;					// 模版管理器
    CRunFlag 							mRunFlag;								// 服务器运行状态
//	CConfigure 						mConfig;									// 服务器配置信息

    CProxyClient						mProxyClient[MAX_PROXY_NUM];	// 服务器间通信的连接

    int										mServerState;	// 服务器状态
    unsigned long long						mLastTickCount; // tick
public:
    CProxyClient* GetProxyClient(int iIndex);

    int AddMsgToMsgSet(CMessageSet* pMsgSet, CMessage* pMsg);

    // 为找不到CTeam的连接发送消息
    void SendMsgSystemErrorResponse(int iResult, long lMsgGuid, int iServerID, time_t tCreateTime, unsigned int uiIP, unsigned short unPort, bool bKickOff = false);
};

#endif //SERVER_GAMESERVER_H
