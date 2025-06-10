#include "proxy_transfer.h"
#include "proxy_def.h"
#include "server_config.h"
#include "my_assert.h"
#include "time_helper.h"
#include "mfactory_manager.h"

CProxyTransfer::CProxyTransfer() : CTCPServer()
{
	m_ConnMap.clear();
}

CProxyTransfer::~CProxyTransfer()
{
	m_ConnMap.clear();
}

bool CProxyTransfer::PrepareToRun()
{
	CSafePtr<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
	CSafePtr<ServerInfo> gateInfo = tmpConfig->GetServerInfo(enServerType::FE_PROXYSERVER);
	int nRet = InitTcpServer(eTcpEpoll, gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
	if (nRet == 0)
	{
		DISK_LOG(DEBUG_DISK, "Server PrepareToRun success at {} : {}", gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
		return true;
	}
	else
	{
		DISK_LOG(DEBUG_DISK,"Server PrepareToRun at {} : {} failed,failed reason {]", gateInfo->m_sHost.c_str(),
			gateInfo->m_iPort, strerror(errno));
		return false;
	}
	return true;
}

void CProxyTransfer::ProcessServerMessage(CSafePtr<CProxyConn> pGamePlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	mshead_size packLen = pRecvBuff->ReadT<mshead_size>();
	if (packLen > MAX_PACKAGE_LEN)
	{
		CACHE_LOG(TCP_ERROR, "Packet len illegal from {}:{} packetlen = {} ",pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), packLen);
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	int nPacketId = pRecvBuff->ReadInt();
	int nSeq = pRecvBuff->ReadInt();
	if (nPacketId != ProxyMessage::Msg::ProxyMessage_Msg_MsgID)
	{
		CACHE_LOG(TCP_ERROR, "packetId illegal from {}:{} packetId = {} ", pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), nPacketId);
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		CACHE_LOG(TCP_ERROR, "ReadBytes failed from {}:{} packetId = {} ", pGamePlayer->GetServerType(), pGamePlayer->GetServerId());
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nPacketId);
	if (pFactory == NULL)
	{
		CACHE_LOG(TCP_ERROR, "GetFactory failed from {}:{} packetId = {} ", pGamePlayer->GetServerType(), pGamePlayer->GetServerId());
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
		CACHE_LOG(TCP_ERROR, "ParseFromArray failed from {}:{} packetId = {} ", pGamePlayer->GetServerType(), pGamePlayer->GetServerId());
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}

	try
	{
		pFactory->Execute(pGamePlayer.DynamicCastTo<CTCPSocket>());
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE, "Message execute failed,msg = {},msgid = {}", e.what(), pFactory->MessId());
	}
	return;
}

void CProxyTransfer::RegisterNewConn(CSafePtr<CProxyConn> pGamePlayer)
{
	ASSERT(pGamePlayer != NULL);
	ConnMap::iterator it = m_ConnMap.find(pGamePlayer->ConnKey());
	if (it != m_ConnMap.end())
	{
		CACHE_LOG(TCP_ERROR, "RegisterNewConn the conn has register conninfo {}:{}", pGamePlayer->GetServerType(), pGamePlayer->GetServerId());
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	m_ConnMap.insert(std::make_pair(pGamePlayer->ConnKey(), pGamePlayer));
}

//
CSafePtr<CProxyConn> CProxyTransfer::FindProxyPlayer(int servertype, int serverid)
{
	ConnMap::iterator it = m_ConnMap.find(CProxyConn::ConnKey(servertype,serverid));
	if (it != m_ConnMap.end())
	{
		return it->second;
	}
	return NULL;
}

//
void CProxyTransfer::TransferMessage(CSafePtr<CProxyConn> pGamePlayer,int servertype, int serverid, shared_ptr<ProxyMessage> pMessage)
{
	// ASSERT(pGamePlayer != NULL && pMessage != NULL);
	// //转发消息
	// CSafePtr<CProxyConn> pDestPlayer = CProxyTransfer::GetSingletonPtr()->FindProxyPlayer(servertype, serverid);
	// if (pDestPlayer == NULL)
	// {
	// 	CACHE_LOG(TCP_ERROR, "Transform packet target not find from {}:{} to {}:{},packetid {}", 
	// 		pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), servertype, serverid, pMessage->packetid());
	// 	return;
	// }
	// bool bRet = pMessage->SerializePartialToArray(m_CacheData, MAX_PACKAGE_LEN);
	// if (!bRet)
	// {
	// 	CACHE_LOG(TCP_ERROR, "Transform packet SerializePartialToArray failed from {}:{} to {}:{},packetid {}",
	// 		pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), servertype, serverid, pMessage->packetid());
	// 	//连接踢掉
	// 	pGamePlayer->SetProxyState(eProKicking);
	// 	return;
	// }
	// pDestPlayer->Write(m_CacheData, pMessage->GetCachedSize());
}

void CProxyTransfer::CheckKickConn(time_t now)
{
	ConnMap::iterator it = m_ConnMap.begin();
	for (; it != m_ConnMap.end();)
	{
		if (it->second->GetProxyState() == eProKicking)
		{
			CACHE_LOG(TCP_DEBUG, "Kick tcp conn {}:{} to {}:{}", it->second->GetServerType(), it->second->GetServerId());
			it->second->Close(false);
			it = m_ConnMap.erase(it);
			continue;
		}
		if (now - it->second->GetLastRecvHeartbeatTime() > TCP_CONN_TIME_OUT)
		{
			CACHE_LOG(TCP_DEBUG, "Kick tcp timeount conn {}:{} to {}:{}", it->second->GetServerType(), it->second->GetServerId());
			it->second->Close(false);
			it = m_ConnMap.erase(it);
			continue;
		}
		it++;
	}
}

void CProxyTransfer::RemoveConnect(CSafePtr<CProxyConn> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	ConnMap::iterator it = m_ConnMap.find(pGamePlayer->ConnKey());
	if (it != m_ConnMap.end())
	{
		m_ConnMap.erase(it);
	}
	return;
}

void CProxyTransfer::OnNewConnect(CSafePtr<CTCPConn> pConnn)
{
	CSafePtr<CProxyConn> pConn = pConnn.DynamicCastTo<CProxyConn>();
	if (pConnn != NULL)
	{}
}

//
CSafePtr<CTCPConn> CProxyTransfer::CreateTcpConn(CSocket socket)
{
	CSafePtr<CProxyConn> pConn = new CProxyConn(socket);
	pConn->SetProxyState(eProConnected);
	return pConn.DynamicCastTo<CTCPConn>();
}