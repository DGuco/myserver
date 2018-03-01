//
// Created by dguco on 18-2-5.
//

#include <config.h>
#include <server_comm_engine.h>
#include "connector.h"
#include "../inc/server_handle.h"
#include "../inc/game_server.h"

int CServerHandle::m_iProxyId = 0;

char CServerHandle::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

CServerHandle::CServerHandle()
	:
	m_pNetWork(new CNetWork(eNetModule::NET_SELECT))
{

}

CServerHandle::~CServerHandle()
{

}

int CServerHandle::PrepareToRun()
{
	if (Connect2Proxy()) {
		if (Regist2Proxy()) {
//			m_ProxyClient.InitTimer((time_t) CServerConfig::GetSingletonPtr()->GetSocketTimeOut());
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

void CServerHandle::Run()
{
	m_pNetWork->DispatchEvents();
}

// 连接到Proxy
bool CServerHandle::Connect2Proxy()
{
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	if (!m_pNetWork->Connect(rTmpProxy->m_sHost.c_str(),
							 rTmpProxy->m_iPort,
							 &CServerHandle::lcb_OnCnsDisconnected,
							 &CServerHandle::lcb_OnConnectFailed,
							 &CServerHandle::lcb_OnConnectted,
							 &CServerHandle::lcb_OnCnsSomeDataSend,
							 &CServerHandle::lcb_OnCnsSomeDataRecv,
							 &CServerHandle::lcb_OnPingServer,
							 CServerConfig::GetSingletonPtr()->GetTcpKeepAlive(),
							 CServerConfig::GetSingletonPtr()->GetSocketTimeOut())
		) {
		LOG_ERROR("default", "[{} : {} : {}] Connect to Proxy({}:{})(id={}) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iServerId);
		return false;
	}
	LOG_NOTICE("default", "Connect to Proxy({}:{})(id={}) succeed.",
			   rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iPort);
	return true;
}

// 向Proxy注册
bool CServerHandle::Regist2Proxy()
{
	CProxyMessage tmpMessage;
	char acTmpMessageBuffer[1024];
	unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);

	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(), enServerType::FE_GAMESERVER, rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER, rTmpProxy->m_iServerId, GetMSTime(), enMessageCmd::MESS_REGIST);

	int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}

	iRet = m_pNetWork->FindConnector(m_iProxyId)->Send((BYTE *) acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] proxy SendOneCode failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}

	LOG_NOTICE("default", "Regist to Proxy now.");
	return true;
}

// 发送心跳到Proxy
bool CServerHandle::SendKeepAlive2Proxy()
{
	CProxyMessage tmpMessage;
	char acTmpMessageBuffer[1024];
	unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_KEEPALIVE);

	int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
	SendMessageToProxy(acTmpMessageBuffer, unTmpTotalLen);
	return true;
}

void CServerHandle::SendMessageToDB(char *data, PACK_LEN len)
{
	SendMessageToProxy(data, len);
}

void CServerHandle::SendMessageToProxy(char *data, PACK_LEN len)
{
	CGameServer::GetSingletonPtr()->GetIoThread()->PushTaskBack(
		[data, len, this]
		{
			CConnector *pConn = m_pNetWork->FindConnector(CServerHandle::GetProxyId());
			if (pConn == NULL) {
				LOG_ERROR("default", "ProxyServer connection has gone");
				return;
			}
			//直接通过socket发送，不同过buffer_event保证buffer_event线程安全
			int iRet = pConn->SendBySocket(data, len);
			if (iRet <= 0) {
				LOG_ERROR("default", "[{} : {} : {}] proxy SendOneCode failed, iRet = {}.",
						  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
				return;
			}
		});
}

void CServerHandle::lcb_OnConnectted(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
	SetProxyId(pConnector->GetSocket().GetSocket());
}

void CServerHandle::lcb_OnCnsDisconnected(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
}

void CServerHandle::lcb_OnCnsSomeDataRecv(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
	//数据不完整
	if (!pConnector->IsPackageComplete()) {
		return;
	}
	int iTmpLen = pConnector->GetRecvPackLen() - sizeof(PACK_LEN);
	//读取数据
	pConnector->RecvData(m_acRecvBuff, iTmpLen);
	// 将收到的二进制数据转为protobuf格式
	CProxyMessage tmpMessage;
	int iRet = ServerCommEngine::ConvertStreamToMsg(m_acRecvBuff,
													iTmpLen,
													&tmpMessage,
													CGameServer::GetSingletonPtr()->GetMessageFactory());
	if (iRet < 0) {
		LOG_ERROR("default", "[{} : {} : {}] convert stream to message failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	CProxyHead tmpProxyHead = tmpMessage.msghead();
	if (tmpProxyHead.has_srcfe() && tmpProxyHead.srcfe() == FE_PROXYSERVER) {
		if (tmpProxyHead.has_srcid() && tmpProxyHead.srcid() == (unsigned short) rTmpProxy->m_iServerId
			&& tmpProxyHead.has_opflag() && tmpProxyHead.opflag() == enMessageCmd::MESS_KEEPALIVE) {
			// 设置proxy为已连接状态
//			SetServerState(ESS_CONNECTPROXY);
//			// 从proxy过来只有keepalive，所以其他的可以直接抛弃
//			m_ProxyClient.ResetTimeout(GetMSTime());
		}
	}
	// 处理服务器间消息
	CGameServer::GetSingletonPtr()->GetLogicThread()->PushTaskBack(
		&CMessageDispatcher::ProcessServerMessage, &tmpMessage);
}

void CServerHandle::lcb_OnCnsSomeDataSend(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);

}
void CServerHandle::lcb_OnConnectFailed(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);

}

void CServerHandle::lcb_OnPingServer(CConnector *pConnector)
{
}

void CServerHandle::SetProxyId(int id)
{
	m_iProxyId = id;
}

int CServerHandle::GetProxyId()
{
	return m_iProxyId;
}
