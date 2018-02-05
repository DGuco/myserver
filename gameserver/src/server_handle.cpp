//
// Created by dguco on 18-2-5.
//

#include <config.h>
#include <server_comm_engine.h>
#include "connector.h"
#include "../inc/server_handle.h"
#include "../inc/game_server.h"

CServerHandle::CServerHandle()
	:
	m_pNetWork(new CNetWork(eNetModule::NET_SELECT)),
	m_iProxyId(0)
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
		LOG_ERROR("default", "[%s : %d : %s] Connect to Proxy(%s:%d)(id=%d) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iServerId);
		return false;
	}
	LOG_NOTICE("default", "Connect to Proxy(%s:%d)(id=%d) succeed.",
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
	pbmsg_setproxy(tmpMessage.mutable_msghead(), enServerType::FE_GAMESERVER, rTmpProxy->m_iServerId,
				   enServerType::FE_PROXYSERVER, rTmpProxy->m_iServerId, GetMSTime(), enMessageCmd::MESS_REGIST);

	int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s : %d : %s] ConvertMsgToStream failed, iRet = %d.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}

	iRet = m_pNetWork->FindConnector(1)->Send((BYTE *) acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s : %d : %s] proxy SendOneCode failed, iRet = %d.",
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
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpProxy->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_KEEPALIVE);

	int iRet = ServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s : %d : %s] ConvertMsgToStream failed, iRet = %d.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
	SendMessageToProxy(acTmpMessageBuffer, unTmpTotalLen)
	return true;
}

bool CServerHandle::SendMessageToDB(char *data, PACK_LEN len)
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
				LOG_ERROR("default", "[%s : %d : %s] proxy SendOneCode failed, iRet = %d.",
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
