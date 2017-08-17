//
// Created by DGuco on 17-7-27.
//
#include "../inc/player.h"
#include "../inc/sceneobjmanager.h"
#include "../inc/messagedispatcher.h"
#include "../inc/gameserver.h"
#include "../../framework/message/message_interface.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/base/performance.h"
#include "../../framework/base/servertool.h"

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
    if (pMsg == NULL)
    {
        return -1;
    }

    Message* pMsgPara = (Message*) pMsg->msgpara();
    if (!pMsgPara)
    {
        return -2;
    }

    MesHead tmpHead = pMsg->msghead();
    CSocketInfo tmpSocketInfo = tmpHead.socketinfos();
    // 获得CTeam 实体
    CPlayer* pPlayer = CSceneObjManager::GetSingletonPtr()->GetPlayer(tmpSocketInfo.socketid());
    if ( NULL == pTmpTeam )
    {
        // 未找到玩家实体
        LOG_ERROR("default", "[%s : %d : %s] ProcessClientMsg failed, Invalid team obj_id(%d).",
                  __MY_FILE__, __LINE__, __FUNCTION__, pHead->entityid());
        return -3;
    }

    //有消息正在处理（注:正常情况不会出现这种情况，客户端等一个消息回复后才能发下一个消息)
    if(pPlayer->GetPackage().GetIsDeal())
    {
        //todo 缓存消息 or 踢下线
        return -4;
    }

    const ::google::protobuf::Descriptor* pTmpDescriptor = pMsgPara->GetDescriptor();
#ifdef _DEBUG_
    OBJ_ID iTmpPlayerId = pTmpTeam->GetPlayerId();
    if ( pMsg->msghead().messageid() != CMsgPingRequest::MsgID )
    {
        LOG_DEBUG("default", "---- Recv Client(%d : %ld) Msg[ %s ][id: 0x%08x / %d] ----",
                  iTmpPlayerId, tmpSocketInfo->socketid(),
                  pTmpDescriptor->name().c_str(), pMsg->msghead().messageid(), pMsg->msghead().messageid());
        LOG_DEBUG("default", "[%s]", ((Message*)pMsg->msgpara())->ShortDebugString().c_str());
    }
#endif

    PERF_FUNC(pTmpDescriptor->name().c_str(), CGameServer::GetSingletonPtr()->ProcessClientMessage(pHead, pMsg, pTmpTeam));

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
int CMessageDispatcher::ProcessServerMessage(CProxyHead* pHead, CMessage* pMsg)
{

}