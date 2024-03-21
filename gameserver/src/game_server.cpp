//
// Created by DGuco on 17-3-1.
//
#include "my_assert.h"
#include "common_def.h"
#include "server_config.h"
#include "base.h"
#include "my_assert.h"
#include "game_player.h"
#include "game_server.h"
#include "server_client.h"
#include "time_helper.h"
#include "module_manager.h"

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}


// 运行准备
int CGameServer::PrepareToRun()
{
	SafePointer<CServerConfig> pConfig = CServerConfig::GetSingletonPtr();
	SafePointer<ServerInfo> pGameInfo = pConfig->GetServerInfo(enServerType::FE_GAMESERVER);
	int nRet = InitTcpServer(eTcpEpoll, pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
	if (nRet == 0)
	{
		DISK_LOG(DEBUG_DISK, "CGameServer PrepareToRun success at {} : {}", pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
		return true;
	}
	else
	{
		DISK_LOG(DEBUG_DISK, "CGameServer PrepareToRun at {} : {} failed,failed reason {]", pGameInfo->m_sHost.c_str(),
			pGameInfo->m_iPort, strerror(errno));
		return false;
	}

	SafePointer<ServerInfo> pRroxyInfo = pConfig->GetServerInfo(enServerType::FE_PROXYSERVER);
	ConnectTo(pRroxyInfo->m_sHost.c_str(), pRroxyInfo->m_iPort,false);
// 	// 通知各模块启动
// 	if (m_pModuleManager->OnLaunchServer( ) != 0) {
// 		return -5;
// 	}
// 
//     if (m_pServerHandle->PrepareToRun( )) {
//         return -1;
//     }
	return 0;
}

void CGameServer::OnNewConnect(SafePointer<CTCPConn> pConnn)
{
	SafePointer<CGamePlayer> pConn = pConnn.DynamicCastTo<CGamePlayer>();
	if (pConnn != NULL)
	{
	}
}

//
SafePointer<CTCPConn> CGameServer::CreateTcpConn(CSocket tmSocket)
{
	SafePointer<CGamePlayer> pConn = new CGamePlayer();
	return pConn.DynamicCastTo<CTCPConn>();
}

//
SafePointer<CTCPClient> CGameServer::CreateTcpClient(CSocket tmSocket)
{
	SafePointer<CServerClient> pConn = new CServerClient();
	return pConn.DynamicCastTo<CTCPClient>();
}

// 运行
void CGameServer::Run()
{

}

// 退出
void CGameServer::Exit()
{
	exit(0);
}

//读取客户端上行数据
void CGameServer::RecvClientData(SafePointer<CGamePlayer> pGamePlayer)
{
	SafePointer<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt(true);
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	pGamePlayer->SetLastRecvKeepLive(tNow);
	CMessage tmMessage;
	if (!tmMessage.ParseFromArray(m_CacheData, packLen))
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	ProcessClientMessage(&tmMessage, pGamePlayer);
	return;
}

void CGameServer::ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError)
	{
		DisConnect(pGamePlayer, iError);
	}
}

void CGameServer::DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	static CMessage tmpMessage;
	tmpMessage.Clear();
	CMesHead* tmpHead = tmpMessage.mutable_msghead();
	CSocketInfo* pSocketInfo = tmpHead->mutable_socketinfos()->Add();
	if (pSocketInfo == NULL)
	{
		return;
	}
	pSocketInfo->set_socketid(pGamePlayer->GetSocketFD());
	pSocketInfo->set_createtime(pGamePlayer->GetCreateTime());
	pSocketInfo->set_state(iError);


	return;
}

void CGameServer::ProcessClientMessage(SafePointer<CMessage> pMsg, SafePointer<CGamePlayer> pPlayer)
{
	ASSERT(pMsg != NULL && pPlayer != NULL);
	int iTmpType = GetModuleClass(pMsg->msghead( ).cmd( ));
	CModuleManager::GetSingletonPtr()->OnClientMessage(iTmpType, pPlayer, pMsg);
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