//
// Created by dguco on 18-1-30.
//
#include "proxy_server.h"
#include "proxy_def.h"
#include "server_config.h"
#include "my_assert.h"
#include "time_helper.h"
#include "mfactory_manager.h"

CProxyServer::CProxyServer() : CTCPServer()
{
	m_ConnMap.clear();
}

CProxyServer::~CProxyServer()
{
	m_ConnMap.clear();
}

bool CProxyServer::PrepareToRun()
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

void CProxyServer::ProcessServerMessage(CSafePtr<CProxyPlayer> pGamePlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt();
	if (packLen > MAX_PACKAGE_LEN)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	int nPacketId = pRecvBuff->ReadInt();
	int nSeq = pRecvBuff->ReadInt();
	if (nPacketId != ProxyMessage::Msg::ProxyMessage_Msg_MsgID)
	{
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nPacketId);
	if (pFactory == NULL)
	{
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
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

void CProxyServer::RegisterNewConn(CSafePtr<CProxyPlayer> pGamePlayer)
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
CSafePtr<CProxyPlayer> CProxyServer::FindProxyPlayer(int servertype, int serverid)
{
	ConnMap::iterator it = m_ConnMap.find(CProxyPlayer::ConnKey(servertype,serverid));
	if (it != m_ConnMap.end())
	{
		return it->second;
	}
	return NULL;
}

//
void CProxyServer::TransferMessage(CSafePtr<CProxyPlayer> pGamePlayer,int servertype, int serverid, shared_ptr<ProxyMessage> pMessage)
{
	ASSERT(pGamePlayer != NULL && pMessage != NULL);
	//转发消息
	CSafePtr<CProxyPlayer> pDestPlayer = CProxyServer::GetSingletonPtr()->FindProxyPlayer(servertype, serverid);
	if (pDestPlayer == NULL)
	{
		CACHE_LOG(TCP_ERROR, "Transform packet target not find from {}:{} to {}:{},packetid {}", 
			pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), servertype, serverid, pMessage->packetid());
		return;
	}
	bool bRet = pMessage->SerializePartialToArray(m_CacheData, MAX_PACKAGE_LEN);
	if (!bRet)
	{
		CACHE_LOG(TCP_ERROR, "Transform packet SerializePartialToArray failed from {}:{} to {}:{},packetid {}",
			pGamePlayer->GetServerType(), pGamePlayer->GetServerId(), servertype, serverid, pMessage->packetid());
		//连接踢掉
		pGamePlayer->SetProxyState(eProKicking);
		return;
	}
	pDestPlayer->Write(m_CacheData, pMessage->GetCachedSize());
}

void CProxyServer::CheckKickConn()
{
	ConnMap::iterator it = m_ConnMap.begin();
	for (; it != m_ConnMap.end();)
	{
		if (it->second->GetProxyState() == eProKicking)
		{
			it->second->Close(false);
			it = m_ConnMap.erase(it);
			continue;
		}
		it++;
	}
}

void CProxyServer::ClearSocket(CSafePtr<CProxyPlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError) 
	{
		DisConnect(pGamePlayer, iError);
	}
}

void CProxyServer::DisConnect(CSafePtr<CProxyPlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
// 	static ProxyMessage tmpMessage;
// 	tmpMessage.Clear( );
// 	ProxyMessage* tmpHead = tmpMessage.mutable_msghead( );
	return;
}

void CProxyServer::OnNewConnect(CSafePtr<CTCPConn> pConnn)
{
	CSafePtr<CProxyPlayer> pConn = pConnn.DynamicCastTo<CProxyPlayer>();
	if (pConnn != NULL)
	{

	}
}

//
CSafePtr<CTCPConn> CProxyServer::CreateTcpConn(CSocket socket)
{
	CSafePtr<CProxyPlayer> pConn = new CProxyPlayer(socket);
	pConn->SetProxyState(eProConnected);
	return pConn.DynamicCastTo<CTCPConn>();
}