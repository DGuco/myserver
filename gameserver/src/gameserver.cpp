//
// Created by DGuco on 17-3-1.
//
#include "../inc/gameserver.h"
#include "../inc/messagefactory.h"
#include "../../framework/json/config.h"
#include "../../framework/net/server_comm_engine.h"

template<> CGameServer* CSingleton<CGameServer>::spSingleton = NULL;

CGameServer::CGameServer()
{
    Initialize();
}

CGameServer::~CGameServer()
{
    if (m_pClientHandle != NULL)
    {
        delete m_pClientHandle;
    }

    if (m_pMessageDispatcher != NULL)
    {
        delete m_pMessageDispatcher;
    }

    if (m_pMessageFactory != NULL)
    {
        delete m_pMessageFactory;
    }

    if (m_pModuleManager != NULL)
    {
        delete m_pModuleManager;
    }

    if (m_pTimerManager != NULL)
    {
        delete m_pTimerManager;
    }
}

int CGameServer::Initialize()
{
    miServerState = 0;
    mLastTickCount = 0;
    m_pModuleManager = new CModuleManager;
    m_pClientHandle = new CClientHandle;
    m_pMessageDispatcher = new CMessageDispatcher;
    m_pMessageFactory = new CMessageFactory;
    m_pTimerManager = new CTimerManager;
}

// 读取配置
int CGameServer::ReadCfg()
{
    LOG_NOTICE("default", "Read server config begin.");
    return 0;
}


const CProxyClient& CGameServer::GetProxyClient()
{
    return m_ProxyClient;
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


    if (Connect2Proxy())
    {
        if (Regist2Proxy())
        {
            m_ProxyClient.InitTimer((time_t) CServerConfig::GetSingletonPtr()->GetSocketTimeOut());
        }
    }

    // 通知各模块启动
    if (m_pModuleManager->OnLaunchServer() != 0)
    {
        return -5;
    }

    return 0;
}

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

    while(true)
    {
        m_pTimerManager->CheckTimerQueue(tTmpNow);
        // 处理客户端上传请求
        iRet = RecvClientMsg(tTmpNow);
        // 处理服务器间请求
        iRet += RecvServerMsg(tTmpNow);
        // 检查服务器状态
        CheckRunFlags();

        //当前没有可处理的消息等待1ms
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
    MY_ASSERT(pMsg != NULL && pPlayer != NULL, return);
//    MY_ASSERT(pMsg->has_msghead() == true, return);
    int iTmpType = GetModuleClass(pMsg->msghead().cmd());
    try {
        m_pModuleManager->OnClientMessage(iTmpType, pPlayer, pMsg);
    }catch (std::logic_error error) {
        LOG_ERROR("default","Catch execption,msg %s",error.what());
    }
}


void CGameServer::ProcessRouterMessage(CProxyMessage* pMsg)
{
    MY_ASSERT(pMsg != NULL, return );
//    MY_ASSERT(pMsg->has_msghead() == true, return);
    int iTmpType = GetModuleClass(pMsg->msghead().messageid());
    try {
        m_pModuleManager->OnRouterMessage(iTmpType, pMsg);
    }catch (std::logic_error error) {
        LOG_ERROR("default","Catch execption,msg %s",error.what());
    }
}


bool CGameServer::SendMessageToDB(CProxyMessage* pMsg)
{
    CProxyHead *pHead = pMsg->mutable_msghead();
    char acTmpMessageBuffer[MAX_PACKAGE_LEN];
    unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);

    ServerInfo gameInfo = CServerConfig::GetSingleton().GetServerMap().find(enServerType::FE_GAMESERVER)->second;
    ServerInfo dbInfo = CServerConfig::GetSingleton().GetServerMap().find(enServerType::FE_DBSERVER)->second;
    int iTmpServerID = gameInfo.m_iServerId;
    int iTmpDBServerID = dbInfo.m_iServerId;
    pbmsg_setproxy(pHead, FE_GAMESERVER, iTmpServerID, FE_DBSERVER, iTmpDBServerID, GetMSTime(), enMessageCmd::MESS_NULL);

    int iRet = ServerCommEngine::ConvertMsgToStream(pMsg, acTmpMessageBuffer, unTmpTotalLen);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] ConvertMsgToStream failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return false;
    }
    iRet = m_ProxyClient.SendOneCode(unTmpTotalLen, (BYTE*)acTmpMessageBuffer);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] proxy(index=%d) SendOneCode failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, 0, iRet);
        return false;
    }

#ifdef _DEBUG_

    Message* pTmpUnknownMessagePara = (Message*) pMsg->msgpara();
    // 如果是打印出错依然返回成功
    MY_ASSERT(pTmpUnknownMessagePara != NULL, return true);
    const ::google::protobuf::Descriptor* pDescriptor = pTmpUnknownMessagePara->GetDescriptor();
    LOG_DEBUG("default", "---- Send DB(%d) Msg[ %s ][id: 0x%08x / %d] ----", pHead->dstid(), pDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
    LOG_DEBUG("default", "[%s]", ((Message*) pMsg->msgpara())->ShortDebugString().c_str());
#endif
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
    m_RunFlag.SetRunFlag(eRunFlag);
}


// 收取客户端消息
int CGameServer::RecvClientMsg(time_t tTime)
{
    int iTmpRecvCount = 0;

    while(iTmpRecvCount < MAX_CHECK_CLIENT_MSG)
    {
        int iRet = m_pClientHandle->Recv();
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
    int iTmpRecvCount = 0;
    fd_set fds_read;
    // select超时时间 10毫秒
    struct timeval stTmpMonTime;
    stTmpMonTime.tv_sec = 0;
    stTmpMonTime.tv_usec = 10000;
    int iTmpMaxFD = 0;
    int iRet = 0;

    FD_ZERO(&fds_read);

    int iTmpFD;
    iTmpFD = m_ProxyClient.GetSocketFD();
    if (iTmpFD > 0 && m_ProxyClient.IsConnected())
    {
        FD_SET(iTmpFD, &fds_read);
        if (iTmpFD > iTmpMaxFD)
        {
            iTmpMaxFD = iTmpFD;
        }
    }
    int iTmpOpenFDNum = select(iTmpMaxFD + 1, &fds_read, NULL, NULL, &stTmpMonTime);
    if (iTmpOpenFDNum <= 0)
    {
        return iTmpRecvCount;
    }

    iTmpFD = m_ProxyClient.GetSocketFD();
    if (FD_ISSET(iTmpFD, &fds_read))
    {
        iRet = m_ProxyClient.RecvData();
        if (iRet < 0)
        {
            LOG_ERROR("default", "[%s : %d : %s] recv proxy code failed, iRet = %d.",
                      __MY_FILE__, __LINE__, __FUNCTION__, iRet);
            return 0;
        }

        CProxyMessage tmpMessage;
        BYTE abyTmpBuffer[MAX_PACKAGE_LEN];
        unsigned short unTmpBuffLen = sizeof(abyTmpBuffer);

        while(true)
        {
            unTmpBuffLen = sizeof(abyTmpBuffer);
            iRet = m_ProxyClient.GetOneCode(unTmpBuffLen, abyTmpBuffer);
            if (iRet <= 0)
            {
                // 等于0说明没数据了
                if (iRet != 0)
                {
                    LOG_ERROR("default", "[%s : %d : %s]GetOneCode from mProxyClient failed, iRet = %d.",
                              __MY_FILE__, __LINE__, __FUNCTION__,  iRet);
                }
                break;
            }

            if (unTmpBuffLen > sizeof(abyTmpBuffer))
            {
                // 数据长度异常
                LOG_ERROR("default", "[%s : %d : %s] code len %d impossible form proxy.",
                          __MY_FILE__, __LINE__, __FUNCTION__, unTmpBuffLen);
                break;
            }

            // 增加已处理消息计数
            iTmpRecvCount++;

            // 将收到的二进制数据转为protobuf格式
            iRet = ServerCommEngine::ConvertStreamToMsg(abyTmpBuffer,
                                                        unTmpBuffLen,
                                                        &tmpMessage,
                                                        m_pMessageFactory);
            if (iRet < 0)
            {
                LOG_ERROR("default", "[%s : %d : %s] convert stream to message failed, iRet = %d.",
                          __MY_FILE__, __LINE__, __FUNCTION__, iRet);
                continue;
            }

            CProxyHead tmpProxyHead = tmpMessage.msghead();
            if (tmpProxyHead.has_srcfe() && tmpProxyHead.srcfe() == FE_PROXYSERVER)
            {
                if (tmpProxyHead.has_srcid() && tmpProxyHead.srcid() == (unsigned short)m_ProxyClient.GetEntityID()
                    && tmpProxyHead.has_opflag() && tmpProxyHead.opflag() == enMessageCmd::MESS_KEEPALIVE)
                {
                    // 设置proxy为已连接状态
                    SetServerState(ESS_CONNECTPROXY);
                    // 从proxy过来只有keepalive，所以其他的可以直接抛弃
                    m_ProxyClient.ResetTimeout(GetMSTime());
                }
                continue;
            }
            // 处理服务器间消息
            m_pMessageDispatcher->ProcessServerMessage(&tmpMessage);
        }
    }
    return iTmpRecvCount;
}

// 主动断开链接
void CGameServer::DisconnectClient(CPlayer* pPlayer)
{
    return m_pClientHandle->DisconnectClient(pPlayer);
}

// 广播消息给玩家，广播时，发起人一定放第一个
int CGameServer::Push(unsigned int iMsgID,Message* pMsg, stPointList* pTeamList)
{
    return m_pClientHandle->Push(iMsgID,pMsg,pTeamList);
}

// 发送消息给单个玩家
int CGameServer::Push(unsigned int iMsgID,Message* pMsg, CPlayer* pPlayer)
{
    MY_ASSERT( pPlayer != NULL && pMsg != NULL,return -1 );
    stPointList tmpList;
    tmpList.push_back(pPlayer);
    return m_pClientHandle->Push(iMsgID,pMsg, &tmpList);
}

// 回复客户端上行的请求
int CGameServer::SendResponse(Message* pMsgPara, CPlayer* pPlayer)
{
    MY_ASSERT( pPlayer != NULL && pMsgPara != NULL,return -1 );
    return m_pClientHandle->SendResponse(pMsgPara, pPlayer);
}

// 回复客户端上行的请求
int CGameServer::SendResponse(Message* pMsgPara, MesHead* mesHead)
{
    MY_ASSERT( mesHead != NULL && pMsgPara != NULL,return -1 );
    return m_pClientHandle->SendResponse(pMsgPara, mesHead);
}

// 连接到Proxy
bool CGameServer::Connect2Proxy()
{
    ServerInfo rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerMap().find(enServerType::FE_PROXYSERVER)->second;
    m_ProxyClient.Initialize(enServerType::FE_PROXYSERVER, rTmpProxy.m_iServerId, inet_addr(rTmpProxy.m_sHost.c_str()), rTmpProxy.m_iPort);

    if (m_ProxyClient.ConnectToServer((char*)rTmpProxy.m_sHost.c_str()))
    {
        LOG_ERROR("default", "[%s : %d : %s] Connect to Proxy(%s:%d)(id=%d) failed.",
                  __MY_FILE__, __LINE__, __FUNCTION__,
                  rTmpProxy.m_sHost.c_str(), rTmpProxy.m_iPort, rTmpProxy.m_iServerId);
        return false;
    }

    LOG_NOTICE("default", "Connect to Proxy(%s:%d)(id=%d) succeed.",
               rTmpProxy.m_sHost.c_str(), rTmpProxy.m_iPort, rTmpProxy.m_iPort);

    return true;
}


// 向Proxy注册
bool CGameServer::Regist2Proxy()
{
    CProxyMessage tmpMessage;
    char acTmpMessageBuffer[1024];
    unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);

    ServerInfo rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerMap().find(enServerType::FE_PROXYSERVER)->second;
    pbmsg_setproxy(tmpMessage.mutable_msghead(), enServerType::FE_GAMESERVER,rTmpProxy.m_iServerId,
                   enServerType::FE_PROXYSERVER, m_ProxyClient.GetEntityID(), GetMSTime(), enMessageCmd::MESS_REGIST);

    int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage,acTmpMessageBuffer, unTmpTotalLen);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] ConvertMsgToStream failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return false;
    }

    iRet = m_ProxyClient.SendOneCode(unTmpTotalLen, (BYTE*)acTmpMessageBuffer);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] proxy SendOneCode failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return false;
    }

    LOG_NOTICE("default", "Regist to Proxy now.");
    return true;
}


// 发送心跳到Proxy
bool CGameServer::SendKeepAlive2Proxy()
{
    CProxyMessage tmpMessage;
    char acTmpMessageBuffer[1024];
    unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);
    ServerInfo rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerMap().find(enServerType::FE_PROXYSERVER)->second;
    pbmsg_setproxy(tmpMessage.mutable_msghead(), enServerType::FE_GAMESERVER, rTmpProxy.m_iServerId,
                   enServerType::FE_PROXYSERVER, m_ProxyClient.GetEntityID(), GetMSTime(), enMessageCmd::MESS_KEEPALIVE);

    int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage,acTmpMessageBuffer, unTmpTotalLen);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] ConvertMsgToStream failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return false;
    }

    iRet = m_ProxyClient.SendOneCode(unTmpTotalLen, (BYTE*)acTmpMessageBuffer);
    if (iRet != 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] proxy SendOneCode failed, iRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return false;
    }
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

void CGameServer::SendMsgSystemErrorResponse(int iResult, long lMsgGuid, int iServerID, time_t tCreateTime, bool bKickOff)
{
}

int CGameServer::InitStaticLog()
{
    return 0;
}


