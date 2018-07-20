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

CByteBuff *CServerHandle::m_pSendBuff = new CByteBuff();

CByteBuff *CServerHandle::m_pRecvBuff = new CByteBuff();

CServerHandle::CServerHandle(shared_ptr<CNetWork> pNetWork)
	: m_pNetWork(std::move(pNetWork)),
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
//	iTmpRet = RecvServerData(tmpBuff->CanWriteData());
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
//		CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
//			std::mem_fn(&CServerHandle::SendToClient), this, tmpSocketInfo, data, tmpDataLen);
	}
	return 0;
}

void CServerHandle::Register2Game()
{
	static CMessage tmpMessage;
	CMesHead *tmpMesHead = tmpMessage.mutable_msghead();
	tmpMesHead->set_opflag(enMessageCmd::MESS_REGIST);
	SendToGame(&tmpMessage);
	return;
}

// 发送心跳到Proxy
void CServerHandle::SendKeepAlive2Game()
{
	static CMessage tmpMessage;
	CMesHead *tmpMesHead = tmpMessage.mutable_msghead();
	tmpMesHead->set_opflag(enMessageCmd::MESS_KEEPALIVE);
	SendToGame(&tmpMessage);
}

int CServerHandle::SendToGame(CMessage *pMessage)
{
	int iRet = CClientCommEngine::ConvertToGameStream(m_pSendBuff, pMessage);
	if (iRet != 0) {
		LOG_ERROR("default", "ConvertMsgToStream failed, iRet = {}.", iRet);
		return iRet;
	}
	return SendToGame();
}

int CServerHandle::SendToGame()
{
	int iRet = 0;
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	CConnector *pTmpConn = m_pNetWork->FindConnector(rTmpGame->m_iServerId);
	if (pTmpConn) {
		iRet = pTmpConn->Send(m_pSendBuff->CanReadData(), m_pSendBuff->ReadableDataLen());
		if (iRet < 0) {
			LOG_ERROR("default", "Send to game error, iRet = {}.", iRet);
		}
		else {
			//断开重新连接
			if (pTmpConn->ReConnect()) {
				iRet = pTmpConn->Send(m_pSendBuff->CanReadData(), m_pSendBuff->ReadableDataLen());
				if (iRet < 0) {
					LOG_ERROR("default", "ReSend to game error, iRet = {}.", iRet);
				}
			}
			else {
				LOG_ERROR("default", "Reconnect to game failed");
			};
		}
	}
	else {
		LOG_ERROR("default", "Connection to game has gone");
	}
	return iRet;
}

CByteBuff *CServerHandle::GetSendBuff()
{
	return m_pSendBuff;
}
CByteBuff *CServerHandle::GetRecvBuff()
{
	return m_pRecvBuff;
}
time_t CServerHandle::GetLastSendKeepAlive() const
{
	return m_tLastSendKeepAlive;
}
time_t CServerHandle::GetLastRecvKeepAlive() const
{
	return m_tLastRecvKeepAlive;
}

void CServerHandle::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
}

void CServerHandle::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{

}

void CServerHandle::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([pBufferEvent]
					   {
						   MY_ASSERT(pBufferEvent != NULL, return);
						   LOG_WARN("default", "The connection to proxy is gone,try to reconnect to it");
						   // 断开连接重新连接到game
						   if (((CConnector *) pBufferEvent)->ReConnect() < 0) {
							   LOG_ERROR("default", "Reconnect to proxyServer failed!");
							   return;
						   }
					   });
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
						   CGateCtrl::GetSingletonPtr()->GetServerHandle()->Register2Game();
					   });
}

void CServerHandle::lcb_OnPingServer(int fd, short what, CConnector *pConnector)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([pConnector]
					   {
						   MY_ASSERT(pConnector != NULL, return);
						   time_t tNow = GetMSTime();
						   std::shared_ptr<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
						   std::shared_ptr<CServerHandle>
							   &tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetServerHandle();
						   if (pConnector->GetState() == CConnector::eCS_Connected &&
							   pConnector->GetSocket().GetSocket() > 0 &&
							   tNow - tmpServerHandle->GetLastRecvKeepAlive() < tmpConfig->GetTcpKeepAlive() * 3) {
							   if (tNow - tmpServerHandle->GetLastSendKeepAlive() >= tmpConfig->GetTcpKeepAlive()) {
								   tmpServerHandle->SendKeepAlive2Game();
								   LOG_DEBUG("default", "Send keepAlive to game succeed..");
							   }
						   }
						   else {
							   //断开连接
							   pConnector->SetState(CConnector::eCS_Disconnected);
							   // 断开连接重新连接到proxy服务器
							   if (pConnector->ReConnect() < 0) {
								   LOG_ERROR("default", "Reconnect to game failed!\n");
								   return;
							   }
						   }
					   });
}
