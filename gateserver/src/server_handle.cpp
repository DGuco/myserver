//
// Created by dguco on 18-1-30.
//

#include <share_mem.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include <my_macro.h>
#include <my_assert.h>
#include <message.pb.h>
#include <server_comm_engine.h>
#include "byte_buff.h"
#include "connector.h"
#include "net_work.h"
#include "../inc/server_handle.h"
#include "../inc/gate_ctrl.h"

CServerHandle::CServerHandle(shared_ptr<CNetWork> pNetWork)
    : m_pNetWork(pNetWork),
      m_tLastRecvKeepAlive(0),
      m_tLastSendKeepAlive(0)
{
}

CServerHandle::~CServerHandle()
{

}

int CServerHandle::PrepareToRun()
{
    ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
    if (!m_pNetWork->Connect(rTmpGame->m_sHost.c_str(),
                             rTmpGame->m_iPort,
                             rTmpGame->m_iServerId,
                             CServerHandle::lcb_OnCnsSomeDataSend,
                             &CServerHandle::lcb_OnCnsSomeDataRecv,
                             &CServerHandle::lcb_OnCnsDisconnected,
                             &CServerHandle::lcb_OnConnectFailed,
                             &CServerHandle::lcb_OnConnected,
                             &CServerHandle::lcb_OnPingServer,
                             CServerConfig::GetSingletonPtr()->GetTcpKeepAlive() / 1000)
        ) {
        LOG_ERROR("default", "[{} : {} : {}] Connect to Game({}:{})(id={}) failed.",
                  __MY_FILE__, __LINE__, __FUNCTION__,
                  rTmpGame->m_sHost.c_str(), rTmpGame->m_iPort, rTmpGame->m_iServerId);
        return false;
    }
    LOG_INFO("default", "Connect to Proxy({}:{})(id={}) succeed.",
             rTmpGame->m_sHost.c_str(), rTmpGame->m_iPort, rTmpGame->m_iPort);
    m_tLastRecvKeepAlive = GetMSTime();
    m_tLastSendKeepAlive = GetMSTime();
    return true;
}

void CServerHandle::CheckWaitSendData()
{
    int iTmpRet = 0;
    int unTmpCodeLength = 0;

    std::shared_ptr<CByteBuff> tmpBuff(new CByteBuff);
    iTmpRet = RecvServerData(tmpBuff->CanWriteData());
    if (iTmpRet == 0) {
        return;
    }

    CMessage tmpMes;
    iTmpRet = CClientCommEngine::ConvertStreamToMessage(tmpBuff.get(),
                                                        unTmpCodeLength,
                                                        &tmpMes,
                                                        NULL);
    //序列化失败继续发送
    if (iTmpRet < 0) {
        LOG_ERROR("default",
                  "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return {}.",
                  iTmpRet);
        return;
    }

    CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
        std::mem_fn(&CServerHandle::SendClientData), this, tmpMes, tmpBuff);

}

int CServerHandle::SendClientData(CMessage &tmpMes, std::shared_ptr<CByteBuff> tmpBuff)
{
    int nTmpSocket;
    auto tmpSendList = tmpMes.msghead().socketinfos();
    int tmpDataLen = tmpBuff->ReadableDataLen();
    const char *data = tmpBuff->CanReadData();
    for (int i = 0; i < tmpSendList.size(); ++i) {
        //向后移动socket索引
        CSocketInfo tmpSocketInfo = tmpSendList.Get(i);
        nTmpSocket = tmpSocketInfo.socketid();
        //socket 非法
        if (nTmpSocket <= 0 || MAX_SOCKET_NUM <= nTmpSocket) {
            LOG_ERROR("default", "Invalid socket index {}", nTmpSocket);
            continue;
        }
        CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
            std::mem_fn(&CServerHandle::SendToClient), this, tmpSocketInfo, data, tmpDataLen);
    }
    return 0;
}

int CServerHandle::RecvServerData(char *data)
{
    int unTmpCodeLength = MAX_PACKAGE_LEN;
    if (m_pS2CPipe->GetHeadCode((BYTE *) data, unTmpCodeLength) < 0) {
        unTmpCodeLength = 0;
    }
    return unTmpCodeLength;
}

void CServerHandle::Register2Game()
{
    CMessage tmpMessage;
    char acTmpMessageBuffer[1024];
    unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);

    CMesHead *tmpMesHead = tmpMessage.mutable_msghead();
    tmpMesHead->set_opflag(enMessageCmd::MESS_REGIST);
    SendToGame(tmpMessage);
    return;
}

void CServerHandle::SendToGame(CMessage &tmpMes)
{
    int iRet = CClientCommEngine::ConvertToGameStream(&m_oSendBuff, &tmpMes);
    if (iRet != 0) {
        LOG_ERROR("default", "ConvertMsgToStream failed, iRet = {}.", iRet);
        return;
    }
    ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
    shared_ptr<CConnector> pTmpConn = m_pNetWork->FindConnector(rTmpGame->m_iServerId);
    if (pTmpConn) {
        iRet = pTmpConn->Send(m_oSendBuff.CanReadData(), m_oSendBuff.ReadableDataLen());
        if (iRet < 0) {
            LOG_ERROR("default", "Send to game error, iRet = {}.", iRet);
        }
    }
}

void CServerHandle::SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len)
{
    CAcceptor *pAcceptor = CNetWork::GetSingletonPtr()->FindAcceptor(socketInfo.socketid());
    if (pAcceptor == NULL) {
        LOG_ERROR("default", "CAcceptor has gone, socket = {}", socketInfo.socketid());
        return;
    }

    /*
     * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
     * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
     * 接的数据，原来连接的数据,中断发送
    */
    if (pAcceptor->GetCreateTime() != socketInfo.createtime()) {
        LOG_ERROR("default",
                  "sokcet[{}] already closed(tcp createtime:{}:gate createtime:{}) : gate ==> client failed",
                  socketInfo.socketid(),
                  pAcceptor->GetCreateTime(),
                  socketInfo.createtime());
        return;
    }
    int iTmpCloseFlag = socketInfo.state();
    int iRet = pAcceptor->Send(data, len);
    if (iRet != 0) {
        //发送失败
        CClientHandle::ClearSocket(pAcceptor, Err_ClientClose);
        LOG_ERROR("default",
                  "send to client {} Failed due to error {}",
                  pAcceptor->GetSocket().GetSocket(),
                  errno);
        return;
    }

    //gameserver 主动关闭
    if (iTmpCloseFlag < 0) {
        CClientHandle::ClearSocket(pAcceptor, Client_Succeed);
    }
}

void CServerHandle::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
}

void CServerHandle::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{

}

void CServerHandle::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{

}
void CServerHandle::lcb_OnConnectFailed(CConnector *pConnector)
{

}
void CServerHandle::lcb_OnConnected(CConnector *pConnector)
{
    MY_ASSERT(pConnector != NULL, return);
    CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
        ->PushTaskBack([pConnector]
                       {
                           MY_ASSERT(pConnector != NULL, return);
                           CGateCtrl::GetSingletonPtr()->GetCS2cHandle()->Register2Proxy();
                       });
}
void CServerHandle::lcb_OnPingServer(int fd, short what, CConnector *pConnector)
{

}
