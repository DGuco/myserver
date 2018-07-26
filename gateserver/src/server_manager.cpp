//
// Created by dguco on 18-1-30.
//

#include <share_mem.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include <my_assert.h>
#include "connector.h"
#include "net_work.h"
#include "../inc/server_manager.h"
#include "../inc/gate_ctrl.h"

CServerManager::CServerManager(shared_ptr<CNetWork> pNetWork)
	: m_pNetWork(pNetWork),
	  m_pSendBuff(new CByteBuff),
	  m_pRecvBuff(new CByteBuff),
	  m_tLastRecvKeepAlive(0),
	  m_tLastSendKeepAlive(0)
{
}

CServerManager::~CServerManager()
{
}

int CServerManager::PrepareToRun()
{
	ConnectToGame();
	m_tLastRecvKeepAlive = GetMSTime();
	m_tLastSendKeepAlive = GetMSTime();
	return true;
}

void CServerManager::CheckWaitSendData()
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

	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(std::mem_fn(&CServerManager::SendClientData), this, tmpMes, tmpBuff);

}

int CServerManager::SendClientData(CMessage &tmpMes, std::shared_ptr<CByteBuff> tmpBuff)
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
//			std::mem_fn(&CServerManager::SendToClient), this, tmpSocketInfo, data, tmpDataLen);
	}
	return 0;
}

void CServerManager::Register2Game()
{
	static CMessage tmpMessage;
	CMesHead *tmpMesHead = tmpMessage.mutable_msghead();
	tmpMesHead->set_opflag(enMessageCmd::MESS_REGIST);
	SendToGameBuff(&tmpMessage);
	return;
}

// 发送心跳到Proxy
void CServerManager::SendKeepAlive2Game()
{
	static CMessage tmpMessage;
	CMesHead *tmpMesHead = tmpMessage.mutable_msghead();
	tmpMesHead->set_opflag(enMessageCmd::MESS_KEEPALIVE);
	SendToGameBuff(&tmpMessage);
}

int CServerManager::SendToGameBuff(CMessage *pMessage)
{
	int iRet = CClientCommEngine::ConvertToGameStream(m_pSendBuff.get(), pMessage);
	if (iRet != 0) {
		LOG_ERROR("default", "ConvertMsgToStream failed, iRet = {}.", iRet);
		return iRet;
	}
	return FlushToGame();
}

int CServerManager::FlushToGame()
{
	int iRet = 0;
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	CConnector *pTmpConn = m_pNetWork->FindConnector(rTmpGame->m_iServerId);
	if (pTmpConn) {
		iRet = pTmpConn->Send(m_pSendBuff->CanReadData(), m_pSendBuff->ReadableDataLen());
		if (iRet < 0) {
			LOG_ERROR("default", "Send to game failed,failed reason {]", strerror(errno));
			//断开重新连接
			if (ReconnectToGame(pTmpConn)) {
				iRet = pTmpConn->Send(m_pSendBuff->CanReadData(), m_pSendBuff->ReadableDataLen());
			}
		}
	}
	else {
		LOG_ERROR("default", "Connection to game has gone");
	}
	return iRet;
}

void CServerManager::ConnectToGame()
{
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	if (!m_pNetWork->Connect(rTmpGame->m_sHost.c_str(),
							 rTmpGame->m_iPort,
							 rTmpGame->m_iServerId,
							 CServerManager::lcb_OnCnsSomeDataSend,
							 &CServerManager::lcb_OnCnsSomeDataRecv,
							 &CServerManager::lcb_OnCnsDisconnected,
							 &CServerManager::lcb_OnConnectFailed,
							 &CServerManager::lcb_OnConnected,
							 &CServerManager::lcb_OnPingServer,
							 CServerConfig::GetSingletonPtr()->GetTcpKeepAlive() / 1000)
		) {
		LOG_ERROR("default", "[{} : {} : {}] Connect to Game({}:{})(id={}) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  rTmpGame->m_sHost.c_str(), rTmpGame->m_iPort, rTmpGame->m_iServerId);
		return;
	}
	LOG_INFO("default", "Connect to Game({}:{})(id={}) succeed.",
			 rTmpGame->m_sHost.c_str(), rTmpGame->m_iPort, rTmpGame->m_iPort);
}

bool CServerManager::ReconnectToGame(CConnector *tmpConnector)
{
	//断开重新连接,尝试3次
	for (int i = 0; i < 3; i++) {
		if (tmpConnector->ReConnect()) {
			LOG_INFO("default", "Reconnect to game done");
			return true;
		}
		else {
			LOG_ERROR("default", "Reconnect to game failed,failed reason {}", strerror(errno));
		}
	}
	return false;
}

shared_ptr<CByteBuff> &CServerManager::GetSendBuff() const
{
	return m_pSendBuff;
}

shared_ptr<CByteBuff> &CServerManager::GetRecvBuff() const
{
	return m_pRecvBuff;
}

time_t CServerManager::GetLastSendKeepAlive() const
{
	return m_tLastSendKeepAlive;
}

time_t CServerManager::GetLastRecvKeepAlive() const
{
	return m_tLastRecvKeepAlive;
}

void CServerManager::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
}

void CServerManager::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{

}

void CServerManager::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[pBufferEvent]
			{
				MY_ASSERT(pBufferEvent != NULL, return);
				LOG_WARN("default", "Disconnected from game,try to reconnect to it");
				// 断开连接重新连接到game
				shared_ptr<CServerManager>
					&tmpServerManager = CGateCtrl::GetSingletonPtr()->GetServerManager();
				tmpServerManager->ReconnectToGame((CConnector *) pBufferEvent);
			});
}
void CServerManager::lcb_OnConnectFailed(CConnector *pConnector)
{

}
void CServerManager::lcb_OnConnected(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[pConnector]
			{
				MY_ASSERT(pConnector != NULL, return);
				CGateCtrl::GetSingletonPtr()->GetServerManager()->Register2Game();
			});
}

void CServerManager::lcb_OnPingServer(int fd, short what, CConnector *pConnector)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[pConnector]
			{
				MY_ASSERT(pConnector != NULL, return);
				time_t tNow = GetMSTime();
				std::shared_ptr<CServerConfig> &tmpConfig = CServerConfig::GetSingletonPtr();
				std::shared_ptr<CServerManager> &tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetServerManager();
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
					LOG_WARN("default", "Connection to game is timeout,try to reconnect to it");
					pConnector->SetState(CConnector::eCS_Disconnected);
					// 断开连接重新连接到game
					shared_ptr<CServerManager> &tmpServerManager = CGateCtrl::GetSingletonPtr()->GetServerManager();
					tmpServerManager->ReconnectToGame(pConnector);
				}
			});
}