//
// Created by DGuco on 17-3-1.
//
#include <server_comm_engine.h>
#include "my_assert.h"
#include "config.h"
#include "base.h"
#include "../inc/client_handle.h"
#include "../inc/game_server.h"
#include "../inc/message_factory.h"

template<> shared_ptr<CGameServer> CSingleton<CGameServer>::spSingleton = NULL;

CGameServer::CGameServer()
	: m_pClientHandle(std::make_shared<CClientHandle>()),
	  m_pServerHandle(std::make_shared<CServerHandle>()),
	  m_pModuleManager(std::make_shared<CModuleManager>( )),
	  m_pMessageFactory(std::make_shared<CMessageFactory>( )),
	  m_pTimerManager(std::make_shared<CTimerManager>( )),
	  m_pLogicThread(std::make_shared<CThreadPool>(1,ignore_pipe)),
	  m_pComputeThread(std::make_shared<CThreadPool>(2)),
	  m_pConfigHandle(std::make_shared<CConfigHandle>( )),
	  m_iServerState(0)
{
	Initialize( );
}

CGameServer::~CGameServer()
{
    Exit();
}

int CGameServer::Initialize()
{
	return 0;
}

// 运行准备
int CGameServer::PrepareToRun()
{
#ifdef _DEBUG_
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/gameserver.log", level_enum::trace);
#else
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/gameserver.log", level_enum::info);
#endif
	// 读取服务器配置信息
	if (m_pConfigHandle->PrepareToRun( ) != 0) {
		return -1;
	}

	if (StartAllTimers( ) != 0) {
		return -4;
	}

	if (m_pClientHandle->PrepareToRun( )) {
		return -1;
	}

	if (m_pConfigHandle->PrepareToRun( )) {
        return -1;
    }

	// 通知各模块启动
	if (m_pModuleManager->OnLaunchServer( ) != 0) {
		return -5;
	}

    if (m_pServerHandle->PrepareToRun( )) {
        return -1;
    }
	return 0;
}

// 开启所有定时器
int CGameServer::StartAllTimers()
{
	return 0;
}

// perf日志打印
void CGameServer::OnTimePerfLog(CTimerBase *pTimer)
{

}

// 运行
void CGameServer::Run()
{
	LOG_INFO("default", "CGameServer start to run now.");
	//libevent事件循环
	m_pServerHandle->DispatchEvents();
}

// 退出
void CGameServer::Exit()
{
	LOG_INFO("default", "Save all data finished. now exit.");
	printf("exit gameserver succeed!\n");
	exit(0);
}

void CGameServer::ProcessClientMessage(CMessage *pMsg, CPlayer *pPlayer)
{
	MY_ASSERT(pMsg != NULL && pPlayer != NULL, return);
//    MY_ASSERT(pMsg->has_msghead() == true, return);
	int iTmpType = GetModuleClass(pMsg->msghead( ).cmd( ));
	try {
		m_pModuleManager->OnClientMessage(iTmpType, pPlayer, pMsg);
	}
	catch (std::logic_error error) {
		LOG_ERROR("default", "Catch logic exception,msg {}", error.what( ));
	}
}

void CGameServer::ProcessRouterMessage(CProxyMessage *pMsg)
{
	MY_ASSERT(pMsg != NULL, return);
//    MY_ASSERT(pMsg->has_msghead() == true, return);
	int iTmpType = GetModuleClass(pMsg->msghead( ).messageid( ));
	try {
		m_pModuleManager->OnRouterMessage(iTmpType, pMsg);
	}
	catch (std::logic_error error) {
		LOG_ERROR("default", "Catch logic exception,msg {}", error.what( ));
	}
}

bool CGameServer::SendMessageToDB(CProxyMessage *pMsg)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return 0,"Do SendMsg must be in logic thread:m_pLogicThread");
    m_pServerHandle->SendMessageToDB(pMsg);
    return true;
}

// 通过消息ID获取模块类型
int CGameServer::GetModuleClass(int iMsgID)
{
//    return ((iMsgID >> 16) & 0xFF);
	return iMsgID / 100;
}

void CGameServer::DisconnectClient(CPlayer *pPlayer)
{
	if (!pPlayer) {
		return;
	}
	m_pClientHandle->DisconnectClient(pPlayer->GetPlayerBase( )->GetSocket( ),
                                      pPlayer->GetPlayerBase( )->GetCreateTime( ));
}
// 设置服务器运行状态
void CGameServer::SetRunFlag(ERunFlag eRunFlag)
{
	m_oRunFlag.SetRunFlag(eRunFlag);
}

// 广播消息给玩家，广播时，发起人一定放第一个
int CGameServer::BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, stPointList *pTeamList)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    m_pClientHandle->BroadCastMsg( iMsgID, pMsgPara, pTeamList);
	return 0;
}

// 发送消息给单个玩家
int CGameServer::BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(pPlayer != NULL && pMsgPara != NULL, return -1);
	stPointList tmpList;
	tmpList.push_back(pPlayer);
    m_pClientHandle->SendResToPlayer( pMsgPara, pPlayer);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(pPlayer != NULL && pMsgPara != NULL,
			  return -1);
	m_pClientHandle->SendResToPlayer( pMsgPara, pPlayer);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<CGooMess> pMsgPara, std::shared_ptr<CMesHead> mesHead)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(mesHead != NULL && pMsgPara != NULL, return -1);
    m_pClientHandle->SendResponse(pMsgPara, mesHead);
}

// 检查服务器状态
int CGameServer::CheckRunFlags()
{
	if (true == m_oRunFlag.CheckRunFlag(ERF_RELOAD)) {
		// 重新加载模板数据
//		if (CTemplateMgr::GetSingletonPtr()->ReloadAllTemplate() < 0) {
//			printf("\n 重新加载模板数据失败, 请查看具体的错误!!!!\n");
//		}
//		else {
//			printf("\n 重新加模板数据成功!!!!\n");
//		}
		SetRunFlag(ERF_RUNTIME);
		return 1;
	}
	else if (true == m_oRunFlag.CheckRunFlag(ERF_QUIT)) {
		// 保存数据,退出游戏
		StartSaveAllData( );
		SetRunFlag(ERF_RUNTIME);
		return 1;
	}
	return 0;
}

/*
// 刷新服务器状态
void CGameServer::FreshGame()
{
	// 这里计时器精度为毫秒
	unsigned long long tNowCount = GetMSTime();
	unsigned int tInterval = (unsigned int) ( tNowCount - mLastTickCount );
	if( tInterval < 100 )
	{
		return;
	}

	if( !CanProcessingClientMsg())
	{
		returnl;
	}
	CCoreModule::GetSingletonPtr()->OnTimerRefreshModule();
	mLastTickCount = tNowCount;
}
*/

// 服务器拉取数据完成
void CGameServer::LoadDataFinish()
{
	// 数据拉取完成,处理服务器状态
	SetServerState(CGameServer::ESS_LOADDATA);
	LOG_INFO("default", "[{} : {} : {}] Launch server load data finished.", __MY_FILE__, __LINE__, __FUNCTION__);
	// 开始处理服务器初始数据
	StartProcessingInitData( );
}

// 服务器开始处理初始数据
void CGameServer::StartProcessingInitData()
{
}

// 存储所有玩家数据(一般故障拉起或程序退出时使用)
void CGameServer::SaveAllTeamdata()
{
//	CSceneObjManagerImp::ObjMng_Team* pTmpObjMngTeam = CSceneObjManager::GetSingletonPtr()->GetObjMngTeam();
//	LOG_DEBUG("default", "[{} : {} : {}] begin save all teams(num={}) data.", __YQ_FILE__, __LINE__, __FUNCTION__, pTmpObjMngTeam->size());
//
//	std::vector<CTeam*> vecDeleteList;
//
//	CSceneObjManagerImp::ObjMng_Team::iterator itTeam = pTmpObjMngTeam->begin();
//	for (; itTeam != pTmpObjMngTeam->end(); itTeam++)
//	{
//		CTeam* pTmpTeam = &(*itTeam);
//		if (pTmpTeam)
//		{
//			if (pTmpTeam->GetTeamState() == CTeam::ETS_INGAMECONNECT
//					|| pTmpTeam->GetTeamState() == CTeam::ETS_INGAMEDISCONNECT)
//			{
//				CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, CTeam::LEAVE_NORMAL);
//			}
//			else
//			{
//				vecDeleteList.push_back(pTmpTeam);
//			}
//		}
//	}
//
//	std::vector<CTeam*>::iterator itBegin = vecDeleteList.begin();
//	for (; itBegin != vecDeleteList.end(); itBegin++)
//	{
//		CCoreModule::GetSingletonPtr()->LeaveGame(*itBegin, CTeam::LEAVE_LOGINFAILED);
//	}
//
//	// 开启服务器退出检测
//	CreateTimer
//	(
//			CTimerManager::GetSingletonPtr(),
//			ETM_EXITSAVE,
//			CTimer::ERTM_GAME,
//			1000,
//			CGameServer::GetSingletonPtr()->CheckSaveAllTeamsFinish,
//			CTimer::TIMER_FOREVER
//	);
}

// 检测是否所有玩家存储结束
void CGameServer::CheckSaveAllTeamsFinish(CTimerBase *pTimer)
{
}

// 拉取服务器数据
void CGameServer::StartLoadAllData()
{

}

// 检测是否开启拉取服务器数据
void CGameServer::CheckStartLoadAllData(CTimerBase *pTimer)
{
}

// 存储服务器数据
void CGameServer::StartSaveAllData()
{
}

// 限制玩家登陆
int CGameServer::LimitTeamLogin(unsigned int iTeamID, time_t iTimes)// itimes 暂定为小时
{
	return 0;
}

// 检测是否停服存储数据
void CGameServer::CheckStartSaveAllData(CTimerBase *pTimer)
{
}

void CGameServer::SendMsgSystemErrorResponse(int iResult,
											 long lMsgGuid,
											 int iServerID,
											 time_t tCreateTime,
											 bool bKickOff)
{
}

int CGameServer::InitStaticLog()
{
	return 0;
}
shared_ptr<CClientHandle> &CGameServer::GetClientHandle()
{
	return m_pClientHandle;
}

shared_ptr<CServerHandle> &CGameServer::GetServerHandle()
{
	return m_pServerHandle;
}

shared_ptr<CModuleManager> &CGameServer::GetModuleManager()
{
	return m_pModuleManager;
}

shared_ptr<CFactory> &CGameServer::GetMessageFactory()
{
	return m_pMessageFactory;
}

shared_ptr<CTimerManager> &CGameServer::GetTimerManager()
{
	return m_pTimerManager;
}

shared_ptr<CThreadPool> &CGameServer::GetLogicThread()
{
	return m_pLogicThread;
}

shared_ptr<CConfigHandle> &CGameServer::GetConfigHandle()
{
	return m_pConfigHandle;
}

shared_ptr<CThreadPool> &CGameServer::GetComputeThread()
{
	return m_pComputeThread;
}