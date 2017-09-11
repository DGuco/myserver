//
// Created by DGuco on 17-6-21.
//

#include "../inc/clienthandle.h"
#include "../datamodule/inc/player.h"
#include "../inc/messagedispatcher.h"
#include "../../framework/mem/shm.h"
#include "../../framework/mem/sharemem.h"
#include "../../framework/mem/codequeue.h"
#include "../../framework/base/my_assert.h"
#include "../../framework/base/commondef.h"
#include "../../framework/message/client_comm_engine.h"
#include "../../framework/json/config.h"
#include "../../framework/base/servertool.h"
#include "../../framework/message/message.pb.h"
#include "../inc/messagefactory.h"
#include "../logicmodule/inc/coremodule.h"

CClientHandle::CClientHandle()
{
    Initialize();
}

CClientHandle::~CClientHandle()
{
}

int CClientHandle::Initialize()
{
    int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

    ////////////////////////////////mS2CPipe/////////////////////////////////////////
    system("touch ./scpipefile");
    char* pcTmpSCPipeID = getenv("SC_PIPE_ID");
    int iTmpSCPipeID = 0;
    if (pcTmpSCPipeID)
    {
        iTmpSCPipeID = atoi(pcTmpSCPipeID);
    }
    key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
    BYTE* pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
    MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize);
    //mS2CPipe地址相对pbyTmpS2CPipe的偏移地址sizeof(CSharedMem) + sizeof(CCodeQueue)
    mS2CPipe = CCodeQueue::CreateInsance(PIPE_SIZE, enLockIdx::IDX_PIPELOCK_S2C);

    ////////////////////////////////mC2SPipe/////////////////////////////////////////
    system("touch ./cspipefile");
    char* pcTmpCSPipeID = getenv("CS_PIPE_ID");
    int iTmpCSPipeID = 0;
    if (pcTmpCSPipeID)
    {
        iTmpCSPipeID = atoi(pcTmpCSPipeID);
    }
    key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
    BYTE* pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
    MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize);
    //mC2SPipe地址相对pbyTmpS2CPipe的偏移地址sizeof(CSharedMem) + sizeof(CCodeQueue)
    mC2SPipe = CCodeQueue::CreateInsance(PIPE_SIZE, enLockIdx::IDX_PIPELOCK_C2S);
    return 0;
}

int CClientHandle::SendResponse(Message* pMessage,CPlayer* pPlayer) {
    MY_ASSERT((pMessage != NULL && pPlayer != NULL), return -1);
    char aTmpCodeBuf[MAX_PACKAGE_LEN] = { 0 };
    MSG_LEN_TYPE unTmpCodeLength = sizeof(aTmpCodeBuf);

    MesHead pTmpHead;
    CSocketInfo* pTmpSocket = pTmpHead.mutable_socketinfos()->Add();
    STConnectInfo* pTmpConnInfo = pPlayer->GetPlayerBase()->GetSocketInfoPtr();
    if (pTmpConnInfo == NULL)
    {
        MY_ASSERT_STR(0, return -1, "CClientHandle::Send failed, Get player connection info failed.");
    }
    pTmpSocket->set_createtime(pTmpConnInfo->m_tCreateTime);
    pTmpSocket->set_socketid(pTmpConnInfo->m_iSocket);
    Package tmpPackage = pPlayer->GetPackage();
    pTmpHead.set_cmd(tmpPackage.GetCmd());
    pTmpHead.set_seq(tmpPackage.GetSeq());
    pTmpHead.set_serial(tmpPackage.GetSerial());

    tmpPackage.SetDeal(false);
    // 是否需要加密，在这里修改参数
    int iRet = ClientCommEngine::ConvertToGateStream(aTmpCodeBuf,
                                                        unTmpCodeLength,
                                                        &pTmpHead,
                                                        pMessage);
    if (iRet != 0)
    {
        MY_ASSERT_STR(0, return -2, "CClientHandle::Send failed, ClientCommEngine::ConvertGameServerMessageToStream failed.");
    }

    iRet = mS2CPipe->AppendOneCode((BYTE*)aTmpCodeBuf, unTmpCodeLength);
    if (iRet < 0)
    {
        MY_ASSERT_STR(0, return -3, "CClientHandle::Send failed, AppendOneCode return %d.", iRet);
    }

    LOG_DEBUG("default", "---- Send To Client Succeed ----");
    return 0;
}

int CClientHandle::Push(int cmd,Message* pMessage, stPointList* pTeamList)
{
    MY_ASSERT((pMessage != NULL && pTeamList != NULL), return -1);

    time_t tTmpNow = time(NULL);
    bool bTmpKickoff = false;
    // 判断是否发送消息后断开连接(这个主动断开只针对与第一个玩家)
//    mNetHead.Initialize(tTmpNow, (bTmpKickoff == true ? -1 : 0));
    MesHead pTmpHead;;
    pTmpHead.set_cmd(cmd);
    pTmpHead.set_seq(0);
    BYTE aTmpMessageBuf[MAX_PACKAGE_LEN] = { 0 };

    for (int i = 0; i < pTeamList->GetBroadcastNum(); i++)
    {
        // 将列表中的实体信息加入nethead头中
        CPlayer* pPlayer = (CPlayer*)pTeamList->GetPointByIdx(i);
        if (pPlayer)
        {
            if (pPlayer->GetPlayerBase()->GetSocketInfoPtr()->m_iSocket != 0)
            {
                CSocketInfo* pTmpSocket = pTmpHead.mutable_socketinfos()->Add();
                STConnectInfo* pTmpConnInfo = pPlayer->GetPlayerBase()->GetSocketInfoPtr();
                if (pTmpConnInfo == NULL)
                {
                    MY_ASSERT_STR(0, return -1, "CClientHandle::Send failed, Get player connection info failed.");
                }
                pTmpSocket->set_createtime(pTmpConnInfo->m_tCreateTime);
                pTmpSocket->set_socketid(pTmpConnInfo->m_iSocket);
//                mNetHead.AddEntity(pTmpTeam->GetSocketInfoPtr()->iSocket, pTmpTeam->GetSocketInfoPtr()->tCreateTime);
//                LOG_DEBUG("default", "---- Send To Client( %d | %lu | %s ) socket(%d) createtime(%ld) ----",
//                          pTmpTeam->GetEntityID(), pTmpTeam->GetPlayerId(), pTmpTeam->GetPlayerName(),
//                          pTmpTeam->GetSocketInfoPtr()->iSocket, pTmpTeam->GetSocketInfoPtr()->tCreateTime);
            }
            else
            {
                LOG_DEBUG("default", "Client(%d | %lu | %s) has disconnected.", pPlayer->GetPlayerId(), pPlayer->GetPlayerId(), pPlayer->GetPlayerBase()->GetAccount());
            }
        }
    }

    unsigned char aTmpCodeBuf[MAX_PACKAGE_LEN] = { 0 };
    unsigned short unTmpCodeLength = sizeof(aTmpCodeBuf);
    // 是否需要加密，在这里修改参数
    int iRet = ClientCommEngine::ConvertToGateStream(aTmpCodeBuf,
                                                        unTmpCodeLength,
                                                        &pTmpHead,
                                                        pMessage);
    if (iRet != 0)
    {
        MY_ASSERT_STR(0, return -2, "CClientHandle::Send failed, ClientCommEngine::ConvertGameServerMessageToStream failed.");
    }

    iRet = mS2CPipe->AppendOneCode(aTmpCodeBuf, unTmpCodeLength);
    if (iRet < 0)
    {
        MY_ASSERT_STR(0, return -3, "CClientHandle::Send failed, AppendOneCode return %d.", iRet);
    }
    return iRet;
}

int CClientHandle::Recv()
{
    BYTE abyTmpCodeBuf[MAX_PACKAGE_LEN] =
            { 0 };
    MSG_LEN_TYPE iTmpCodeLength = sizeof(abyTmpCodeBuf);

    // 从共享内存管道提取消息
    int iRet = mC2SPipe->GetHeadCode((BYTE *) abyTmpCodeBuf,
                                     &(int)iTmpCodeLength);

    if (iRet < 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] When GetHeadCode from C2SPipe, error ocurr %d",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return ClienthandleErrCode::CLIENTHANDLE_QUEUE_CRASH;
    }

    if (iTmpCodeLength == 0)
    {
        return ClienthandleErrCode::CLIENTHANDLE_QUEUE_EMPTY;
    }

    CMessage tmpMessage;
    Message* pMessage;
    iRet = DecodeNetMsg(abyTmpCodeBuf, iTmpCodeLength,tmpMessage.mutable_msghead(), &tmpMessage);
    if (iRet != 0)
    {
        return iRet;
    }
    if (pMessage == NULL)
    {
        return ClienthandleErrCode::CLIENTHANDLE_PARSE_FAILED;
    }
    tmpMessage.set_msgpara((int64)pMessage);
    CMessageDispatcher::GetSingletonPtr()->ProcessClientMessage(&tmpMessage);

    return CLIENTHANDLE_SUCCESS;
}

int CClientHandle::DecodeNetMsg(BYTE* pCodeBuff, MSG_LEN_TYPE& nLen, MesHead* pCSHead, Message* pMsg)
{
    //长度小于消息头的长度+数据总长度+字节对齐长度
    if (!pCodeBuff || nLen < int(pCSHead->ByteSize() + (sizeof(unsigned short) * 2)))
    {
        return ClienthandleErrCode::CLIENTHANDLE_SMALL_LENGTH;
    }

    if (ClientCommEngine::ConvertStreamToMessage(pCodeBuff,
                                                nLen,
                                                pCSHead,
                                                pMsg,
                                                CMessageFactory::GetSingletonPtr()) != 0)
    {
        return ClienthandleErrCode::CLIENTHANDLE_PARSE_FAILED;
    }

    //gate上行数据必须带client的socket信息
    if (pCSHead->socketinfos().size() != 1)
    {
        return ClienthandleErrCode::CLIENTHANDLE_MSGINVALID;
    }
    CSocketInfo tmpSocketInfo = pCSHead->socketinfos(0);
    int iTmpSocket = tmpSocketInfo.socketid();
    int tTmpCreateTime = tmpSocketInfo.createtime();
    if (tmpSocketInfo.state() < 0)
    {
        // 客户端主动关闭连接，也有可能是连接错误被关闭
        LOG_INFO("default", "client(%d : %d) commhandle closed by err = %d. ", iTmpSocket, tTmpCreateTime, tmpSocketInfo->state());
        // 从连接容器中取出玩家实体
        CPlayer* pTmpTeam = CCoreModule::GetSingletonPtr()->GetTeamBySocket(iTmpSocket);
        if (NULL == pTmpTeam)
        {
            // 找不到玩家，连接已经关闭了
            LOG_ERROR("default", "[%s : %d : %s] socket(%d : %d) EntityID = %d has closed.",
                      __MY_FILE__, __LINE__, __FUNCTION__, iTmpSocket, tTmpCreateTime);
            return CLIENTHANDLE_HASCLOSED;
        }
        else
        {
            if (pTmpTeam->GetSocketInfoPtr()->iSocket != iTmpSocket
                || pTmpTeam->GetSocketInfoPtr()->tCreateTime != tTmpCreateTime)
            {
                // 当前玩家与该连接信息不匹配,说明该玩家的连接已经失效
                LOG_WARN("default", "[%s : %d : %s] socket(%d : %d) not match, now(%d : %d).",
                         __MY_FILE__, __LINE__, __FUNCTION__, iTmpSocket, tTmpCreateTime, pTmpTeam->GetSocketInfoPtr()->iSocket, pTmpTeam->GetSocketInfoPtr()->tCreateTime);
                // 重置玩家连接信息
                CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
//				CCoreModule::GetSingletonPtr()->EraseSockInfoList(iTmpSocket);
//				pTmpTeam->GetSocketInfoPtr()->Clear();
//				pTmpTeam->SetTeamState(CTeam::ETS_INGAMEDISCONNECT);

                return CLIENTHANDLE_NOTSAMETEAM;
            }
            else
            {
                // 模拟玩家下线
                pMsg->mutable_msghead()->set_messageid(CMsgLeaveGameRequest::MsgID);

                CMsgLeaveGameRequest* pTmpMsgPara = new (macMessageBuff) CMsgLeaveGameRequest;

                pTmpMsgPara->set_leavetype(mNetHead.m_cState);
                pMsg->set_msgpara((unsigned long) pTmpMsgPara);

                pCSHead->set_teamid(pTmpTeam->GetTeamID());
                pCSHead->set_entityid(pTmpTeam->GetEntityID());
                pCSHead->set_timestamp(mNetHead.m_tStamp);

                return CLIENTHANDLE_SUCCESS;
            }
        }
    }

    // 服务器数据拉完了才能让玩家正常游戏
    if (CGameServer::GetSingletonPtr()->CanProcessingClientMsg() == false)
    {
        // 通知客户端服务器未开启并断开连接
        CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_noservice, lTmpMsgGuid, iTmpSocket, tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);
        return CLIENTHANDLE_ISNOTNORMAL;
    }

    CTeam* pTmpTeam = NULL;
    // 如果是登陆消息
    if (pMsg  && pCSHead->cmd() == CMsgLoginGameRequest::MsgID)
    {
        // 5500踢掉断线玩家 检测是否需要踢掉断连玩家
        if( CCoreModule::GetSingletonPtr()->CheckOnlineIsFull()  <  0)
        {
            LOG_INFO("default", "[%s : %d : %s]  login failed, team list is full.",__YQ_FILE__, __LINE__, __FUNCTION__);

            // 通知客户端服务器已满并断开连接
            CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_isfull,lTmpMsgGuid,iTmpSocket, tTmpCreateTime,
                                                                       mNetHead.m_iSrcIP, mNetHead.m_nSrcPort,true);
            return CLIENTHANDLE_ONLINEFULL;
        }
        pTmpTeam = CCoreModule::GetSingletonPtr()->GetTeamBySocket(iTmpSocket);
        if( pTmpTeam)
        {
            // 同一个fd
            if (tTmpCreateTime == pTmpTeam->GetSocketInfoPtr()->tCreateTime)
            {
                // 连续多次发TryLogin消息则直接把消息抛掉
                LOG_DEBUG("default", "[%s : %d : %s]  Socket(%d) is useed, don't TryLogin again.",
                          __YQ_FILE__, __LINE__, __FUNCTION__, iTmpSocket);
                // 通知客户端解析消息失败并断开连接
                CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msgerr, lTmpMsgGuid, iTmpSocket,
                                                                           tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);
                return CLIENTHANDLE_MSGINVALID;
            }
            else
            {
                // 假如是同一个fd,时间戳不一样, 可能socket断掉未检测到
                LOG_INFO("default", "[%s : %d : %s]  Socket(%d) is used, old team(TeamID=%lu, EntityID=%d), reset.",
                         __YQ_FILE__, __LINE__, __FUNCTION__, iTmpSocket, pTmpTeam->GetTeamID(), pTmpTeam->GetEntityID());

                // 先把老玩家断连
                CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
            }
        }

        // 解析登陆消息
        CMsgLoginGameRequest* pTmpMsg = (CMsgLoginGameRequest*) pMsg->msgpara();
        if (pTmpMsg == NULL)
        {
            LOG_ERROR("default", "[%s : %d : %s]  team(Account=%s, ServerID=%d) login failed, msg is NULL.",
                      __YQ_FILE__, __LINE__, __FUNCTION__);

            // 通知客户端解析消息失败并断开连接
            CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msginvalid, lTmpMsgGuid, iTmpSocket,
                                                                       tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);
            return CLIENTHANDLE_MSGINVALID;
        }
        if (strcmp(pTmpMsg->account().c_str(), "") == 0 || strcmp(pTmpMsg->password().c_str(), "") == 0)
        {
            // 通知客户端解析消息失败并断开连接
            CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msginvalid, lTmpMsgGuid, iTmpSocket,
                                                                       tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);
            return CLIENTHANDLE_MSGINVALID;
        }

        // 登陆时候判断session是否已验证
        string sSession = CCoreModule::GetSingletonPtr()->GetTeamIDBySession(pTmpMsg->account().c_str(), pTmpMsg->serverid(), pTmpMsg->pfrom());
        LOG_INFO("default", "[%s : %d : %s]  uid: %s, new session: %s, old session: %s",__YQ_FILE__, __LINE__, __FUNCTION__,pTmpMsg->account().c_str(),pTmpMsg->password().c_str(), sSession.c_str());
        // 是否验证sdk登陆
        bool bSdkCheck = CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().sdkcheck();
        if( bSdkCheck && strcmp(pTmpMsg->password().c_str(), sSession.c_str()) != 0)
        {
            CWebModule::GetSingletonPtr()->XiaoMiLoginRequest(pMsg, iTmpSocket, tTmpCreateTime, lTmpMsgGuid, mNetHead.m_iSrcIP, mNetHead.m_nSrcPort, mNetHead.m_tStamp);
            return CLIENTHANDLE_LOGINCHECK;
        }
        else
        {
            CCoreModule::GetSingletonPtr()->SDKLoginSucces(pTmpMsg->account().c_str(),pTmpMsg->password().c_str(),pTmpMsg->serverid(),pTmpMsg->pfrom(),iTmpSocket,tTmpCreateTime,lTmpMsgGuid,mNetHead.m_iSrcIP,mNetHead.m_nSrcPort,mNetHead.m_tStamp);
            return CLIENTHANDLE_LOGINCHECK;
        }
    }
    else
    {
        // 消息链接玩家
        pTmpTeam = CCoreModule::GetSingletonPtr()->GetTeamBySocket(iTmpSocket);
        // 判断 玩家跟消息合法性
        if(pTmpTeam  ==  NULL  || pMsg  ==  NULL  || tTmpCreateTime != pTmpTeam->GetSocketInfoPtr()->tCreateTime || pCSHead->teamid() != pTmpTeam->GetTeamID()
           ||  pTmpTeam->GetTeamDebug() != 0)
        {
            LOG_DEBUG("default", "teamid[%lu]msgid[%u] return -1008", pCSHead->teamid(), pMsg->mutable_msghead()->messageid());
            // 通知客户端解析消息失败并断开连接
            CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msgerr, lTmpMsgGuid, iTmpSocket,
                                                                       tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);

            if( pTmpTeam )
            {
                LOG_DEBUG("default", "team leavegame 1008");
                // 断开链接
                CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
                // 测试
                pTmpTeam->SetTeamDebug(0);
            }

            return CLIENTHANDLE_MSGINVALID;
        }

        if( pTmpTeam->IsLoginLimitTime() )
        {
            // 通知客户端解析消息失败并断开连接
            CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_loginlimit, lTmpMsgGuid, iTmpSocket,
                                                                       tTmpCreateTime, mNetHead.m_iSrcIP,mNetHead.m_nSrcPort, true);
        }

        pTmpTeam->SetTeamState(CTeam::ETS_INGAMECONNECT);
    }


    pTmpTeam->GetSocketInfoPtr()->tLastActiveTime = mNetHead.m_tStamp;
    pCSHead->set_entityid( pTmpTeam->GetEntityID() );
	if (pTmpTeam->GetSocketInfoPtr()->lMsgGuid == lTmpMsgGuid)
	{
		// 如果消息的GUID相等,说明这是客户端重发的消息,服务器已经处理过了,直接抛弃
		LOG_INFO("default", "[%s : %d : %s] message guid(%ld) is same, ignore it.",
				__YQ_FILE__, __LINE__, __FUNCTION__, lTmpMsgGuid);
		return CLIENTHANDLE_SUCCESS;
	}
    pTmpTeam->GetSocketInfoPtr()->lMsgGuid = lTmpMsgGuid;

    //////////////////////////////////////////////////////////////////////////////////////////////////

    return CLIENTHANDLE_SUCCESS;
}

// 断开玩家连接
void CClientHandle::DisconnectClient(CPlayer* cPlayer)
{
    if (!cPlayer)
    {
        return;
    }

    DisconnectClient(
            cPlayer->GetSocketInfoPtr()->m_iSocket,
            cPlayer->GetSocketInfoPtr()->m_tCreateTime);
}

// 断开玩家连接
void CClientHandle::DisconnectClient(int iSocket,
                                     time_t tCreateTime)
{
    BYTE abyTmpCodeBuff[MAX_PACKAGE_LEN];
    unsigned char* pucTmpBuff = (unsigned char*) abyTmpCodeBuff;
    unsigned short unTmpLen = 0;

//    // 这里必须用临时变量，因为有可能是在接收到消息的时候发起断连
//    CNetHead tmpNetHead;
//    tmpNetHead.Initialize(time(NULL), -1, uiIP, unPort);
//    tmpNetHead.AddEntity(iSocket, tCreateTime);

    // 总长度
    *(unsigned short*) pucTmpBuff =
            ((sizeof(unsigned short) * 2) + tmpNetHead.Size());
    pucTmpBuff += sizeof(unsigned short);
    unTmpLen += sizeof(unsigned short);

    // CNetHead长度
    *(unsigned short*) pucTmpBuff = tmpNetHead.Size();
    pucTmpBuff += sizeof(unsigned short);
    unTmpLen += sizeof(unsigned short);

    // 序列化CNetHead
    unsigned short unRet = tmpNetHead.SerializeToArray(pucTmpBuff,
                                                       MAX_PACKAGE_LEN - unTmpLen);
    if (unRet < 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] SerializeToArray failed, tRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, unRet);
        return;
    }
    if (unRet != tmpNetHead.Size())
    {
        LOG_ERROR("default", "[%s : %d : %s] length is not same (%d : %d).",
                  __MY_FILE__, __LINE__, __FUNCTION__, unRet, tmpNetHead.Size());
        return;
    }
    pucTmpBuff += unRet;
    unTmpLen += unRet;

    unRet = mS2CPipe->AppendOneCode(abyTmpCodeBuff, unTmpLen);
    if (unRet < 0)
    {
        LOG_ERROR("default", "[%s : %d : %s] AppendOneCode failed, tRet = %d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, unRet);
        return;
    }

    LOG_NOTICE("default", "Disconnect Client [Socket = %d : CreateTime = %d].", iSocket, tCreateTime);
}

// 打印管道状态
void CClientHandle::Dump(char* pBuffer, unsigned int& uiLen)
{
    unsigned int uiTmpMaxLen = uiLen;
    uiLen = 0;

    int iTmpBegin = 0;
    int iTmpEnd = 0;
    int iTmpLeft = 0;

    uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen,
                      "------------------------------CClientHandle------------------------------");
    uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10s\t%10s",
                      "name", "free", "total");

    mC2SPipe->GetCriticalData(iTmpBegin, iTmpEnd, iTmpLeft);
    uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10d\t%10d",
                      "c2s pipe", iTmpLeft, PIPE_SIZE);

    mS2CPipe->GetCriticalData(iTmpBegin, iTmpEnd, iTmpLeft);
    uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10d\t%10d",
                      "s2c pipe", iTmpLeft, PIPE_SIZE);
}
