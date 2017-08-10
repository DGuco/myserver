//
// Created by DGuco on 17-3-1.
//
#include "../inc/gameserver.h"
#include "../inc/clienthandle.h"
#include "../logicmodule/inc/modulemanager.h"
#include "../../framework/sharemem/shm.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/base/performance.h"
#include "../../framework/base/my_assert.h"
#include "../../framework/json/config.h"
#include "../../framework/message/proxymessage.pb.h"
#include "../../framework/net/server_comm_engine.h"

template<> CGameServer* CSingleton<CGameServer>::spSingleton = NULL;

CGameServer::CGameServer()
{
    miServerState = 0;
    mLastTickCount = 0;
}

CGameServer::~CGameServer()
{
    if (mpClientHandle != NULL)
    {
        delete mpClientHandle;
    }

    if (mpMessageDispatcher != NULL)
    {
        delete mpMessageDispatcher;
    }

    if (mpMessageFactory != NULL)
    {
        delete mpMessageFactory;
    }

    if (mpModuleManager != NULL)
    {
        delete mpModuleManager;
    }

    if (mpTimerManager != NULL)
    {
        delete mpTimerManager;
    }
}

int CGameServer::Initialize()
{
    mpModuleManager = new CModuleManager;
}

// 读取配置
int CGameServer::ReadCfg()
{
    LOG_NOTICE("default", "Read server config begin.");
    return 0;
}


CProxyClient* CGameServer::GetProxyClient(int iIndex)
{
    if (iIndex < 0 || iIndex >= MAX_PROXY_NUM)
    {
        return NULL;
    }

    return &mProxyClient[iIndex];
}

int CGameServer::AddMsgToMsgSet(CMessageSet* pMsgSet, Message* pMsg)
{
    return mpClientHandle->AddMsgToMsgSet(pMsgSet, pMsg);
}

// 运行准备
int CGameServer::PrepareToRun()
{
    LOG_NOTICE("default", "CGameServer prepare to run.");

    // 读取服务器配置信息
    if (ReadCfg() != 0)
    {
        return -1;
    }

    if (StartAllTimers() != 0)
    {
        return -4;
    }

//    // 连接proxy
//    for (int i = 0; i < CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().proxyinfo_size() && i < MAX_PROXY_NUM; i++)
//    {
//        if (Connect2Proxy(i) == true)
//        {
//            if (Regist2Proxy(i) == true)
//            {
//                mProxyClient[i].InitTimer((time_t) CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().proxytimeout());
//            }
//        }
//    }
//
//    // 通知各模块启动
//    if (m_pModuleManager->OnLaunchServer() != 0)
//    {
//        return -5;
//    }

    return 0;
}

//// 处理定时相关功能
//void CGameServer::OnTimer(time_t tNow)
//{
//	// 每100毫秒执行一次
//	if (mServerTick.IsTimeout(tNow))
//	{
//		// 处理定时器
//		mTimerManager->CheckTimerQueue(tNow);
//
//		// 检查服务器间连接状态及心跳
//		CheckStateInServer(tNow);
//
////		// 通知各模块处理自己的定时器
////		mModuleManager->OnTimer(tNow);
//
//		// perf日志打印(用于服务器性能分析)
//		if ( mSavePerf.IsTimeout(tNow) )
//		{
//			PERF_LOG();
//		}
//	}
//}

// 开启所有定时器
int CGameServer::StartAllTimers()
{

}

// 服务器间心跳检测
void CGameServer::OnTimeCheckStateInServer(CTimerBase* pTimer)
{
}

// perf日志打印
void CGameServer::OnTimePerfLog(CTimerBase* pTimer)
{
}

// 运行
void CGameServer::Run()
{
    LOG_NOTICE("default", "CGameServer start to run now.");

    time_t tTmpNow;
    int iRet = 0;

//	time_t tLastTick = 0;
//	int iCnt = 0;

    while(true)
    {
        tTmpNow = GetMSTime();
        iRet = 0;
//		if ((tTmpNow / 100) != tLastTick)
//		{
//			tLastTick = (tTmpNow / 100);
////			LOG_DEBUG("default", "now = %ld, tick = %ld, iCnt = %d.", tNow, tLastTick, iCnt);
//			iCnt = 0;
//		}
//
//		iCnt++;

        // 处理内部定时请求
//		OnTimer(tTmpNow);
        mpTimerManager->CheckTimerQueue(tTmpNow);
        // 处理客户端上传请求
        iRet = RecvClientMsg(tTmpNow);
        // 处理服务器间请求
        iRet += RecvServerMsg(tTmpNow);
        // 检查服务器状态
        CheckRunFlags();

        if (iRet == 0)
        {
            usleep(1000);
        }

    }
}

// 退出
void CGameServer::Exit()
{
    LOG_INFO("default", "Save all data finished. now exit.");
    printf("exit gameserver succeed!\n");
    exit(0);
}


void CGameServer::ProcessClientMessage(CMessage* pMsg, CPlayer* pPlayer)
{
    MY_ASSERT(pMsg != NULL && pHead != NULL && pPlayer != NULL, return);
    int iTmpType = GetModuleClass(pMsg->msghead().messageid());
    m_pModuleManager->OnClientMessage(iTmpType, pTeam, pMsg);
}


void CGameServer::ProcessRouterMessage(CMessage* pMsg)
{
    MY_ASSERT(pMsg != NULL, return );
    MY_ASSERT(pMsg->has_msghead() == true, return);

    int iTmpType = GetModuleClass(pMsg->msghead().messageid());
    m_pModuleManager->OnRouterMessage(iTmpType, pMsg);
}


bool CGameServer::SendMessageToDB(CMessage* pMsg)
{
    return true;
}

// 给World Server 发消息
bool CGameServer::SendMessageToWorld(CMessage* pMsg)
{
    return true;
}

// 给offline Server 发消息
bool CGameServer::SendMessageToOffline(CMessage* pMsg)
{
    return true;
}

// 给check Server 发消息
bool CGameServer::SendMessageToCheck(CMessage* pMsg)
{
    return true;
}

// 给Web Server 发消息
bool CGameServer::SendMessageToWeb(CMessage* pMsg)
{
    return true;
}

// 通过消息ID获取模块类型
int CGameServer::GetModuleClass(int iMsgID)
{
    return ((iMsgID >> 16) & 0xFF);
}


// 设置服务器运行状态
void CGameServer::SetRunFlag(ERunFlag eRunFlag)
{
    mRunFlag.SetRunFlag(eRunFlag);
}


// 收取客户端消息
int CGameServer::RecvClientMsg(time_t tTime)
{
    int iTmpRecvCount = 0;

    while(iTmpRecvCount < MAX_CHECK_CLIENT_MSG)
    {
        int iRet = mpClientHandle->Recv();
        if (iRet == CLIENTHANDLE_QUEUE_EMPTY)
        {
            // 队列已空，直接返回
            return iTmpRecvCount;
        }

        if (iRet != CLIENTHANDLE_SUCCESS && iRet != CLIENTHANDLE_LOGINCHECK)
        {
            LOG_ERROR("default", "[%s : %d : %s] ClientHandle->Recv failed, iRet = %d.",
                      __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        }

        // 记数自增长
        iTmpRecvCount++;
    }

    return iTmpRecvCount;
}


// 收取服务器消息
int CGameServer::RecvServerMsg(time_t tTime)
{
//    int iTmpRecvCount = 0;
//    fd_set fds_read;
//    // select超时时间 10毫秒
//    struct timeval stTmpMonTime;
//    stTmpMonTime.tv_sec = 0;
//    stTmpMonTime.tv_usec = 10000;
//    int iTmpMaxFD = 0;
//    int iRet = 0;
//
//    FD_ZERO(&fds_read);
//
//    int iTmpFD;
////	int iTmpStatus;
//    for (int i = 0; i < MAX_PROXY_NUM && i < CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().proxyinfo_size(); i++)
//    {
//        iTmpFD = mProxyClient[i].GetSocketFD();
////		iTmpStatus = mProxyClient[i].GetStatus();
//        if (iTmpFD > 0 && mProxyClient[i].IsConnected())
//        {
//            FD_SET(iTmpFD, &fds_read);
//            if (iTmpFD > iTmpMaxFD)
//            {
//                iTmpMaxFD = iTmpFD;
//            }
//        }
//    }
//
//    int iTmpOpenFDNum = select(iTmpMaxFD + 1, &fds_read, NULL, NULL, &stTmpMonTime);
//    if (iTmpOpenFDNum <= 0)
//    {
//        return iTmpRecvCount;
//    }
//
//    for (int i = 0; i < MAX_PROXY_NUM && i < CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().proxyinfo_size(); i++)
//    {
//        iTmpFD = mProxyClient[i].GetSocketFD();
//        if (FD_ISSET(iTmpFD, &fds_read))
//        {
//            iRet = mProxyClient[i].RecvData();
//            if (iRet < 0)
//            {
//                LOG_ERROR("default", "[%s : %d : %s] recv proxy(%d) code failed, iRet = %d.",
//                          __YQ_FILE__, __LINE__, __FUNCTION__, i, iRet);
//                continue;
//            }
//
//            CProxyHead tmpProxyHead;
//            CMessage tmpMessage;
//            BYTE abyTmpBuffer[MAX_PACKAGE_LEN];
//            unsigned short unTmpBuffLen = sizeof(abyTmpBuffer);
//
//            while(true)
//            {
//                unTmpBuffLen = sizeof(abyTmpBuffer);
//                iRet = mProxyClient[i].GetOneCode(unTmpBuffLen, abyTmpBuffer);
//                if (iRet <= 0)
//                {
//                    // 等于0说明没数据了
//                    if (iRet != 0)
//                    {
//                        LOG_ERROR("default", "[%s : %d : %s]GetOneCode from mProxyClient(%d) failed, iRet = %d.",
//                                  __YQ_FILE__, __LINE__, __FUNCTION__, i, iRet);
//                    }
//                    break;
//                }
//
//                if (unTmpBuffLen > sizeof(abyTmpBuffer))
//                {
//                    // 数据长度异常
//                    LOG_ERROR("default", "[%s : %d : %s] code len %d impossible form proxy(%d).",
//                              __YQ_FILE__, __LINE__, __FUNCTION__, unTmpBuffLen, i);
//                    break;
//                }
//
//                // 增加已处理消息计数
//                iTmpRecvCount++;
//
//                // 将收到的二进制数据转为protobuf格式
//                iRet = ServerCommEngine::ConvertStreamToMsg(abyTmpBuffer, unTmpBuffLen, &tmpProxyHead, &tmpMessage, mpMessageFactory);
//                if (iRet < 0)
//                {
//                    LOG_ERROR("default", "[%s : %d : %s] convert stream to message failed, iRet = %d.",
//                              __YQ_FILE__, __LINE__, __FUNCTION__, iRet);
//                    continue;
//                }
//
//                if (tmpProxyHead.has_srcfe() && tmpProxyHead.srcfe() == FE_PROXYSERVER)
//                {
//                    if (tmpProxyHead.has_srcid() && tmpProxyHead.srcid() == (unsigned short)mProxyClient[i].GetEntityID()
//                        && tmpProxyHead.has_opflag() && tmpProxyHead.opflag() == CMD_KEEPALIVE)
//                    {
//                        // 设置proxy为已连接状态
//                        SetServerState(ESS_CONNECTPROXY);
//                        // 从proxy过来只有keepalive，所以其他的可以直接抛弃
//                        mProxyClient[i].ResetTimeout(GetMSTime());
////#ifdef _DEBUG_
////						LOG_DEBUG("default", "[%s : %d : %s] recv proxy(id=%d, index=%d) keepalive, stamp(%lu).",
////								__YQ_FILE__, __LINE__, __FUNCTION__,
////								tmpProxyHead.srcid(), i, tmpProxyHead.timestamp());
////#endif
//                    }
//                    continue;
//                }
//
//                // 处理服务器间消息
//                mpMessageDispatcher->ProcessServerMessage(&tmpProxyHead, &tmpMessage);
//            }
//        }
//    }
//
//    return iTmpRecvCount;
}


// 广播消息给玩家，广播时，发起人一定放第一个
int CGameServer::SendPlayer(CMessageSet* pMsgSet, stPointList* pTeamList)
{
    return mpClientHandle->Send(pMsgSet, pTeamList);
}

// 主动断开链接
void CGameServer::DisconnectClient(CPlayer* pPlayer)
{
    return mpClientHandle->DisconnectClient(pPlayer);
}

// 发送消息给单个玩家
int CGameServer::SendPlayer(CMessageSet* pMsgSet, CPlayer* pPlayer)
{
    MY_ASSERT( pPlayer != NULL && pMsgSet != NULL,return -1 );
    // 消息集合处理
//    if( pPlayer->IsSendTask() )
//    {
//        pPlayer->SetSendTask(false);
//        if( (!pPlayer->GetTaskOnlion()->IsHaveRecv())  && (!pPlayer->GetTaskLoop()->IsHaveRecv()))
//        {
//            CTaskModule::GetSingletonPtr()->AddMsgIsHaveRecvResponse(pTeam,  pMsgSet,  RESULT_SUCCESS);
//        }
//        else
//        {
//            CTaskModule::GetSingletonPtr()->AddMsgIsHaveRecvResponse(pTeam,  pMsgSet, ONE);
//        }
//    }
//
//    stPointList tmpList;
//    tmpList.push_back(pTeam);

//    return mpClientHandle->Send(pMsgSet, &tmpList);
}

int CGameServer::SendPlayer(unsigned int iMsgID, Message* pMsgPara, CPlayer* pPlayer)
{
    MY_ASSERT(pMsgPara != NULL && pPlayer != NULL, return -1);

    stPointList tmpList;
    tmpList.push_back(pPlayer);

    return mpClientHandle->Send(pMsgPara, &tmpList);
}

// 连接到Proxy
bool CGameServer::Connect2Proxy(int iIndex)
{

    return true;
}


// 向Proxy注册
bool CGameServer::Regist2Proxy(int iIndex)
{
}


// 发送心跳到Proxy
bool CGameServer::SendKeepAlive2Proxy(int iIndex)
{
    return true;
}


// 检查服务器状态
void CGameServer::CheckRunFlags()
{
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
		return;
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
    LOG_INFO("default", "[%s : %d : %s] Launch server load data finished.",__MY_FILE__, __LINE__, __FUNCTION__);
    // 开始处理服务器初始数据
    StartProcessingInitData();
}

// 服务器开始处理初始数据
void CGameServer::StartProcessingInitData()
{
}

// 存储所有玩家数据(一般故障拉起或程序退出时使用)
void CGameServer::SaveAllTeamdata()
{
//	CSceneObjManagerImp::ObjMng_Team* pTmpObjMngTeam = CSceneObjManager::GetSingletonPtr()->GetObjMngTeam();
//	LOG_DEBUG("default", "[%s : %d : %s] begin save all teams(num=%d) data.", __YQ_FILE__, __LINE__, __FUNCTION__, pTmpObjMngTeam->size());
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
void CGameServer::CheckSaveAllTeamsFinish(CTimerBase* pTimer)
{
}

// 拉取服务器数据
void CGameServer::StartLoadAllData()
{

}

// 检测是否开启拉取服务器数据
void CGameServer::CheckStartLoadAllData(CTimerBase* pTimer)
{
}

// 存储服务器数据
void CGameServer::StartSaveAllData()
{
}

// 限制玩家登陆
int CGameServer::LimitTeamLogin( unsigned int iTeamID, time_t iTimes )// itimes 暂定为小时
{
}

// 检测是否停服存储数据
void CGameServer::CheckStartSaveAllData(CTimerBase* pTimer)
{
}

void CGameServer::SendMsgSystemErrorResponse(int iResult, long lMsgGuid, int iServerID, time_t tCreateTime, unsigned int uiIP, unsigned short unPort, bool bKickOff)
{
}

int CGameServer::InitStaticLog()
{
    return 0;
}


