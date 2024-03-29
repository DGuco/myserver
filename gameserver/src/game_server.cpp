//
// Created by DGuco on 17-3-1.
//
#include "my_assert.h"
#include "common_def.h"
#include "server_config.h"
#include "game_player.h"
#include "game_server.h"
#include "server_client.h"
#include "time_helper.h"
#include "mfactory_manager.h"
#include "safe_pointer.h"

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}


// ����׼��
bool CGameServer::PrepareToRun()
{
	CSafePtr<CServerConfig> pConfig = CServerConfig::GetSingletonPtr();
	CSafePtr<ServerInfo> pGameInfo = pConfig->GetServerInfo(enServerType::FE_GAMESERVER);
	int nRet = InitTcpServer(eTcpEpoll, pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
	if (nRet == 0)
	{
		DISK_LOG(DEBUG_DISK, "CGameServer PrepareToRun success at {} : {}", pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
		return true;
	}
	else
	{
		DISK_LOG(DEBUG_DISK, "CGameServer PrepareToRun at {} : {} failed,failed reason {]", pGameInfo->m_sHost.c_str(),
			pGameInfo->m_iPort, strerror(errno));
		return false;
	}
	CSafePtr<ServerInfo> pRroxyInfo = pConfig->GetServerInfo(enServerType::FE_PROXYSERVER);
	CSafePtr<CServerClient> pConn = new CServerClient();
	if (!ConnectTo(pConn.DynamicCastTo<CTCPClient>(), pRroxyInfo->m_sHost.c_str(), pRroxyInfo->m_iPort, false))
	{
		pConn.Free();
	}

	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	return true;
}

void CGameServer::OnNewConnect(CSafePtr<CTCPConn> pConnn)
{
	CSafePtr<CGamePlayer> pConn = pConnn.DynamicCastTo<CGamePlayer>();
	if (pConnn != NULL)
	{
	}
}

//
CSafePtr<CTCPConn> CGameServer::CreateTcpConn(CSocket socket)
{
	CSafePtr<CGamePlayer> pConn = new CGamePlayer(socket);
	return pConn.DynamicCastTo<CTCPConn>();
}

// �˳�
void CGameServer::Exit()
{
	exit(0);
}

//��ȡ�ͻ�����������
void CGameServer::ProcessClientMessage(CSafePtr<CGamePlayer> pGamePlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt();
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//�Ͽ�����
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	int nCmd = pRecvBuff->ReadInt();
	int nSeq = pRecvBuff->ReadInt();
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		//�Ͽ�����
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nCmd);
	if (pFactory == NULL)
	{
		//�Ͽ�����
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
		//�Ͽ�����
		ClearSocket(pGamePlayer, Err_PacketError);
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

//�����ȡ����������
void CGameServer::ProcessServerMessage(CSafePtr<CServerClient> pServerPlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pServerPlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt();
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//�Ͽ�����
		return;
	}
	int nCmd = pRecvBuff->ReadInt();
	if (nCmd != ProxyMessage::Msg::ProxyMessage_Msg_MsgID)
	{
		//�Ͽ�����
		return;
	}
	int nSeq = pRecvBuff->ReadInt();
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		return;
	}
	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nCmd);
	if (pFactory == NULL)
	{
		//�Ͽ�����
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
		//�Ͽ�����
		return;
	}

	try
	{
		pFactory->Execute(pServerPlayer.DynamicCastTo<CTCPSocket>());
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE, "Message execute failed,msg = {},msgid = {}", e.what(), pFactory->MessId());
	}
	return;
}

void CGameServer::ClearSocket(CSafePtr<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	//��gameserver ��������ر�
	if (Client_Succeed != iError)
	{
		DisConnect(pGamePlayer, iError);
	}
}

void CGameServer::DisConnect(CSafePtr<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	return;
}

// bool CGameServer::SendMessageToDB(CSafePtr<CProxyMessage> pMsg)
// {
//     //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return 0,"Do SendMsg must be in logic thread:m_pLogicThread");
//     return true;
// }

// ͨ����ϢID��ȡģ������
int CGameServer::GetModuleClass(int iMsgID)
{
//    return ((iMsgID >> 16) & 0xFF);
	return iMsgID / 100;
}