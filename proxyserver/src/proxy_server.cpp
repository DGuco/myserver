//
// Created by dguco on 18-1-30.
//
#include "proxy_server.h"
#include "proxy_def.h"
#include "server_config.h"
#include "my_assert.h"
#include "time_helper.h"
#include "mfactory_manager.h"
#include "proxy_ctrl.h"

CProxyServer::CProxyServer() : CTCPServer()
{
}

CProxyServer::~CProxyServer()
{
}

bool CProxyServer::InitTcp()
{
	CSafePtr<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
	CSafePtr<ServerInfo> gateInfo = tmpConfig->GetServerInfo(enServerType::FE_PROXYSERVER);
	int nRet = InitTcpServer(eTcpEpoll, gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
	if (nRet == 0)
	{
		DISK_LOG(DEBUG_DISK, "CProxyServer InitTcp success at {} : {}", gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
		return true;
	}
	else
	{
		DISK_LOG(DEBUG_DISK,"CProxyServer InitTcp at {} : {} failed,failed reason {]", gateInfo->m_sHost.c_str(),
			gateInfo->m_iPort, strerror(socket_error));
		return false;
	}
	return true;
}

void CProxyServer::ProcessServerMessage(CSafePtr<CProxyConn> pGamePlayer)
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

void CProxyServer::RegisterNewProxyConn(CSafePtr<CProxyConn> pProxyConn)
{
	ASSERT(pProxyConn != NULL);
	if(FindRegisterConn(pProxyConn->ConnKey()) != NULL)
	{
		CACHE_LOG(TCP_ERROR, "RegisterNewConn the conn has register conninfo {}:{}", pProxyConn->GetServerType(), pProxyConn->GetServerId());
		//连接踢掉
		pProxyConn->SetProxyState(eProKicking);
		return;
	}
	RegisterNewConn(pProxyConn.DynamicCastTo<CTCPConn>());
	CACHE_LOG(TCP_DEBUG, "RegisterNewProxyConn new conn conninfo {}:{}", pProxyConn->GetServerType(), pProxyConn->GetServerId());
}

//
CSafePtr<CProxyConn> CProxyServer::FindProxyPlayer(int servertype, int serverid)
{
	return FindRegisterConn(CProxyConn::ConnKey(servertype, serverid)).DynamicCastTo<CProxyConn>();
}

//
void CProxyServer::TransferMessage(CSafePtr<CProxyConn> pGamePlayer,int servertype, int serverid, shared_ptr<ProxyMessage> pMessage)
{
	ASSERT(pGamePlayer != NULL && pMessage != NULL);
	//转发消息
	CSafePtr<CProxyConn> pDestPlayer = CProxyServer::GetSingletonPtr()->FindProxyPlayer(servertype, serverid);
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

void CProxyServer::OnAccept(CSocket newSocket)
{
	CProxyCtrl::GetSingletonPtr()->SelectTransferThread(newSocket);
}

void CProxyServer::OnNewConnect(CSafePtr<CTCPConn> pConnn)
{
	CSafePtr<CProxyConn> pConn = pConnn.DynamicCastTo<CProxyConn>();
	if (pConnn != NULL)
	{}
}

//
CSafePtr<CTCPConn> CProxyServer::CreateTcpConn(CSocket socket)
{
	CSafePtr<CProxyConn> pConn = new CProxyConn(socket);
	pConn->SetProxyState(eProConnected);
	return pConn.DynamicCastTo<CTCPConn>();
}