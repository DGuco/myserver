//
// Created by dguco on 18-2-5.
//

#include <config.h>
#include <server_comm_engine.h>
#include "connector.h"
#include "my_assert.h"
#include "db_module.h"
#include "../inc/server_handle.h"
#include "../inc/game_server.h"

int CServerHandle::m_iProxyId = 0;

CByteBuff CServerHandle::m_RecvBuff = CByteBuff();

CServerHandle::CServerHandle()
	: m_pNetWork(CNetWork::GetSingletonPtr()),
	  m_tLastSendKeepAlive(0),
	  m_tLastRecvKeepAlive(0)
{

}

CServerHandle::~CServerHandle()
{

}

int CServerHandle::PrepareToRun()
{
    m_pClientMsgTimer = std::make_shared<CTimerEvent>(
            m_pNetWork->GetEventReactor(),
            &CServerHandle::lcb_OnCheckClientMsgCome,
            (void*)NULL,
            0,
            100,
            -1);  //100毫秒检测一次是否又数据要发送
    m_pNetWork->RegisterSignalHandler(SIGPIPE, &CServerHandle::lcb_OnSigPipe, 0);
    if (!Connect2Proxy()) {
        return -1;
    }
    ignore_pipe();
	return 0;
}

void CServerHandle::DispatchEvents()
{
    m_pNetWork->DispatchEvents();
}

// 连接到Proxy
bool CServerHandle::Connect2Proxy()
{
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	if (!m_pNetWork->Connect(rTmpProxy->m_sHost.c_str(),
							 rTmpProxy->m_iPort,
							 rTmpProxy->m_iServerId,
							 &CServerHandle::lcb_OnCnsSomeDataSend,
							 &CServerHandle::lcb_OnCnsSomeDataRecv,
							 &CServerHandle::lcb_OnCnsDisconnected,
							 &CServerHandle::lcb_OnConnectFailed,
							 &CServerHandle::lcb_OnConnected,
							 &CServerHandle::lcb_OnPingServer,
							 CServerConfig::GetSingletonPtr()->GetSocketTimeOut())
		)
	{
		LOG_ERROR("default", "[{} : {} : {}] Connect to Proxy({}:{})(id={}) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iServerId);
		return false;
	}
	LOG_INFO("default", "Connect to Proxy({}:{})(id={}) succeed.",
			 rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iPort);
	m_tLastRecvKeepAlive = GetMSTime();
	m_tLastSendKeepAlive = GetMSTime();
	return true;
}

// 向Proxy注册
bool CServerHandle::Register2Proxy()
{
	CProxyMessage tmpMessage;
    CByteBuff tmBuff;
    unsigned short tTotalLen = tmBuff.GetCapaticy();
	ServerInfo *rTmpProxy =CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_REGIST);

	int iRet =
		CServerCommEngine::ConvertMsgToStream(&tmpMessage, &tmBuff, tTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "ConvertMsgToStream failed, iRet = {}.", iRet);
		return false;
	}

	SendMessageToProxy(tmBuff.GetData(), tTotalLen);
	return true;
}

// 发送心跳到Proxy
bool CServerHandle::SendKeepAlive2Proxy()
{
	CProxyMessage tmpMessage;
	CByteBuff tmBuff;
	unsigned short unTmpTotalLen = tmBuff.GetCapaticy();
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_KEEPALIVE);

	int iRet = CServerCommEngine::ConvertMsgToStream(&tmpMessage, &tmBuff, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
	SendMessageToProxy(tmBuff.GetData(), unTmpTotalLen);
	return true;
}

bool CServerHandle::SendMessageToDB(CProxyMessage *pMsg)
{
	CProxyHead *pHead = pMsg->mutable_msghead();
    CByteBuff tmBuff;
    unsigned short unTmpTotalLen = tmBuff.GetCapaticy();
	ServerInfo *gameInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	ServerInfo *dbInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_DBSERVER);
	int iTmpServerID = gameInfo->m_iServerId;
	int iTmpDBServerID = dbInfo->m_iServerId;
	pbmsg_setproxy(pHead,
				   FE_GAMESERVER,
				   iTmpServerID,
				   FE_DBSERVER,
				   iTmpDBServerID,
				   GetMSTime(),
				   enMessageCmd::MESS_NULL);

	int iRet = CServerCommEngine::ConvertMsgToStream(pMsg, &tmBuff, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
	SendMessageToProxy((char*)tmBuff.GetData(),unTmpTotalLen);
	LOG_DEBUG("default", "[{}]", ((CGooMess *) pMsg->msgpara())->ShortDebugString().c_str());
	return true;
}

time_t CServerHandle::GetLastSendKeepAlive() const
{
	return m_tLastSendKeepAlive;
}
time_t CServerHandle::GetLastRecvKeepAlive() const
{
	return m_tLastRecvKeepAlive;
}

void CServerHandle::SetLastSendKeepAlive(time_t tLastSendKeepAlive)
{
	m_tLastSendKeepAlive = tLastSendKeepAlive;
}

void CServerHandle::SetLastRecvKeepAlive(time_t tLastRecvKeepAlive)
{
	m_tLastRecvKeepAlive = tLastRecvKeepAlive;
}

void CServerHandle::SendMessageToProxy(char *data, unsigned short len)
{
	IBufferEvent *pConn = m_pNetWork->FindConnector(CServerHandle::GetProxyId());
	if (pConn == NULL) {
		LOG_ERROR("default", "ProxyServer connection has gone");
		return;
	}
	int iRet = pConn->Send(data, len);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] proxy SendOneCode failed, iRet = {},err = {}",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet,strerror(errno));
		return;
	}
}

void CServerHandle::DealServerData(IBufferEvent *pBufferEvent)
{
    MY_ASSERT(pBufferEvent != NULL, return);
    //数据不完整
    if (!pBufferEvent->IsPackageComplete()) {
        return;
    }
    //读取数据
    m_RecvBuff.Clear();
    m_RecvBuff.WriteUnShort(pBufferEvent->GetRecvPackLen());
    unsigned short iTmpLen = pBufferEvent->GetRecvPackLen() - sizeof(unsigned short);
    pBufferEvent->RecvData(m_RecvBuff.CanWriteData(), iTmpLen);
    pBufferEvent->CurrentPackRecved();
    // 将收到的二进制数据转为protobuf格式
    CProxyMessage tmpMessage;
    int iRet = CServerCommEngine::ConvertStreamToMsg(&m_RecvBuff,
                                                     &tmpMessage,
                                                     CGameServer::GetSingletonPtr()->GetMessageFactory().get());
    if (iRet < 0) {
        LOG_ERROR("default", "[{} : {} : {}] convert stream to message failed, iRet = {}.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
        return;
    }

    ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
    CProxyHead stTmpHead = tmpMessage.msghead();
    if (stTmpHead.has_srcfe() && stTmpHead.srcfe() == FE_PROXYSERVER)
    {
        if (stTmpHead.has_srcid() && stTmpHead.srcid() == (unsigned short) rTmpProxy->m_iServerId
            && stTmpHead.has_opflag() && stTmpHead.opflag() == enMessageCmd::MESS_KEEPALIVE) {
            // 设置proxy为已连接状态
            CGameServer::GetSingletonPtr()->GetServerHandle()->SetLastRecvKeepAlive(GetMSTime());
            LOG_INFO("default", "Recv keepalive msg, from(FE = {} : ID = {}) to(FE = {} : ID = {}), timestamp = {}",
                     stTmpHead.srcfe( ), stTmpHead.srcid( ),
                     stTmpHead.dstfe( ), stTmpHead.dstid( ), stTmpHead.timestamp( ));
        }
        return;
    }
    CGameServer::GetSingletonPtr()->GetLogicThread()->PushTaskBack(
            &CMessageDispatcher::ProcessServerMessage, &tmpMessage);
}

void CServerHandle::lcb_OnConnected(CConnector *pConnector)
{
    MY_ASSERT(pConnector != NULL, return);
    SetProxyId(pConnector->GetTargetId());
    CGameServer::GetSingletonPtr()->GetServerHandle()->Register2Proxy();
}

void CServerHandle::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
    MY_ASSERT(pBufferEvent != NULL, return);
    LOG_WARN("default", "The connection to game is gone,try to reconnect to it");
    // 断开连接重新连接到proxy服务器
    if (((CConnector *) pBufferEvent)->ReConnect() < 0) {
        LOG_ERROR("default", "Reconnect to proxyServer failed!");
        return;
    }
}

void CServerHandle::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{
    DealServerData(pBufferEvent);
    CGameServer::GetSingletonPtr()->GetClientHandle()->DealClientCameMsg();
}

void CServerHandle::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
    CGameServer::GetSingletonPtr()->GetClientHandle()->DealClientCameMsg();
}

void CServerHandle::lcb_OnConnectFailed(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
}

void CServerHandle::lcb_OnPingServer(int fd, short what, CConnector *pConnector)
{
    MY_ASSERT(pConnector != NULL, return);
    time_t tNow = GetMSTime();
    std::shared_ptr<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
    shared_ptr<CServerHandle> tmpServerHandle = CGameServer::GetSingletonPtr()->GetServerHandle();
    if (pConnector->GetState() == CConnector::eCS_Connected && pConnector->GetSocket().GetSocket() > 0) {
        if (tNow - tmpServerHandle->GetLastSendKeepAlive() >= tmpConfig->GetSocketTimeOut()) {
            tmpServerHandle->SendKeepAlive2Proxy();
            LOG_DEBUG("default", "SendkeepAliveToProxy succeed..");
        }
    }
    else {
        //断开连接
        pConnector->SetState(CConnector::eCS_Disconnected);
        // 断开连接重新连接到proxy服务器
        if (pConnector->ReConnect() < 0) {
            LOG_ERROR("default", "Reconnect to proxyServer failed!\n");
            return;
        }
    }
    CGameServer::GetSingletonPtr()->GetClientHandle()->DealClientCameMsg();
}

void CServerHandle::lcb_OnSigPipe(uint,void*)
{
    LOG_ERROR("default", "Game recv sig pipe,call lcb_OnSigPipe\n");
}

//检测是否又数据要发送
void CServerHandle::lcb_OnCheckClientMsgCome(int fd, short what, void *param)
{
    CGameServer::GetSingletonPtr()->GetClientHandle()->DealClientCameMsg();
}

void CServerHandle::SetProxyId(int id)
{
	m_iProxyId = id;
}

int CServerHandle::GetProxyId()
{
	return m_iProxyId;
}
