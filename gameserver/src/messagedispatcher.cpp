//
// Created by DGuco on 17-7-27.
//
#include "../datamodule/inc/player.h"
#include "../inc/sceneobjmanager.h"
#include "../inc/messagedispatcher.h"
#include "../inc/gameserver.h"
#include "../../framework/base/performance.h"
#include "../../framework/message/player.pb.h"
#include "../../framework/base/servertool.h"
#include "../logicmodule/inc/coremodule.h"

template<> CMessageDispatcher* CSingleton<CMessageDispatcher>::spSingleton = NULL;

CMessageDispatcher::CMessageDispatcher()
{

}

CMessageDispatcher::~CMessageDispatcher()
{

}

// 消息客户端上传的消息派发
int CMessageDispatcher::ProcessClientMessage(CMessage* pMsg)
{
    MY_ASSERT(pMsg != NULL && pMsg->has_msghead(), return -1);
    Message* pMsgPara = (Message*) pMsg->msgpara();
    if (!pMsgPara)
    {
        return -2;
    }

    MesHead tmpHead = pMsg->msghead();
    if (tmpHead.socketinfos().size() <= 0)
    {
        return -1;
    }
    const ::google::protobuf::Descriptor* pTmpDescriptor = pMsgPara->GetDescriptor();
    if (tmpHead.cmd() == PlayerCommandId::USER_ACCOUNT_LOGIN)
    {
        PERF_FUNC(pTmpDescriptor->name().c_str(),CCoreModule::GetSingletonPtr()->OnMsgUserLoginRequest(pMsg));
    }else if (tmpHead.cmd() == PlayerCommandId::PLAYER_LOGIN)
    {
        PERF_FUNC(pTmpDescriptor->name().c_str(),CCoreModule::GetSingletonPtr()->OnMsgPlayerLoginRequest(pMsg));
    }else {
        CSocketInfo tmpSocketInfo = tmpHead.socketinfos().Get(0);
        // 获得CTeam 实体
        CPlayer* pPlayer = CSceneObjManager::GetSingletonPtr()->GetPlayer(tmpSocketInfo.socketid());
        if ( NULL == pPlayer )
        {
            // 未找到玩家实体
            LOG_ERROR("default", "[%s : %d : %s] ProcessClientMsg failed, Invalid socket (%d).",
                      __MY_FILE__, __LINE__, __FUNCTION__, tmpHead.socketinfos(0).socketid());
            return -3;
        }
        //有消息正在处理（注:正常情况不会出现这种情况，客户端等一个消息回复后才能发下一个消息)
        Package tmpPackage = pPlayer->GetPackage();
        if(tmpPackage.GetIsDeal())
        {
            //todo 缓存消息 or 踢下线
            return -4;
        }
        tmpPackage.SetDeal(true);
#ifdef _DEBUG_
        OBJ_ID iTmpPlayerId = pPlayer->GetPlayerId();
#endif

        PERF_FUNC(pTmpDescriptor->name().c_str(), CGameServer::GetSingletonPtr()->ProcessClientMessage(pMsg, pPlayer));
    }
//    if ( pMsg->msghead().cmd() != CMsgPingRequest::MsgID )
//    {
//        LOG_DEBUG("default", "---- Recv Client(%d : %ld) Msg[ %s ][id: 0x%08x / %d] ----",
//                  iTmpPlayerId, tmpSocketInfo->socketid(),
//                  pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
//        LOG_DEBUG("default", "[%s]", ((Message*)pMsg->msgpara())->ShortDebugString().c_str());
//    }
    // 消息回收
    if (pMsgPara)
    {
        pMsgPara->~Message();
        pMsg->set_msgpara((unsigned long)NULL);
        pMsgPara = NULL;
    }

    return 0;
}


// 服务器消息派发
int CMessageDispatcher::ProcessServerMessage(CProxyMessage* pMsg)
{
    MY_ASSERT(pMsg != NULL && pMsg->has_msghead(), return -1);
    Message* pTmpMsgPara = (Message*) pMsg->msgpara();
    if (!pTmpMsgPara)
    {
        return -2;
    }

    const ::google::protobuf::Descriptor* pTmpDescriptor = pTmpMsgPara->GetDescriptor();
    CProxyHead* pHead = pMsg->mutable_msghead();
    switch(pHead->srcfe())
    {
        case FE_GAMESERVER:
        {
            LOG_DEBUG("default", "---- Recv Game(%d) Msg[ %s ][id: 0x%08x / %d] ----",
                      pHead->srcid(), pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
            break;
        }
        case FE_LOGINSERVER:
        {
            LOG_DEBUG("default", "---- Recv Login(%d) Msg[ %s ][id: 0x%08x / %d] ----",
                      pHead->srcid(), pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
            break;
        }
        case FE_DBSERVER:
        {
            LOG_DEBUG("default", "---- Recv DB(%d) Msg[ %s ][id: 0x%08x / %d] ----",
                      pHead->srcid(), pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
            break;
        }
        case FE_WEBSERVER:
        {
            LOG_DEBUG("default", "---- Recv Web(%d) Msg[ %s ][id: 0x%08x / %d] ----",
                      pHead->srcid(), pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
            break;
        }
        default:
        {
            LOG_ERROR("default", "[%s : %d : %s] invalid FE = %d.",
                      __MY_FILE__, __LINE__, __FUNCTION__, pHead->srcfe());
            // 消息回收
            if (pTmpMsgPara)
            {
                pTmpMsgPara->~Message();
                pMsg->set_msgpara((unsigned long)NULL);
                pTmpMsgPara = NULL;
            }
            return -3;
        }
    }

    LOG_DEBUG("default", "[%s]", ((Message*)pMsg->msgpara())->ShortDebugString().c_str());
    PERF_FUNC(pTmpDescriptor->name().c_str(), CGameServer::GetSingletonPtr()->ProcessRouterMessage(pMsg));

    // 消息回收
    if (pTmpMsgPara)
    {
        pTmpMsgPara->~Message();
        pMsg->set_msgpara((unsigned long)NULL);
        pTmpMsgPara = NULL;
    }

    return 0;
}