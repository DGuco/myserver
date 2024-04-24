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
	m_DB2SCodeQueue.Reset();
	m_S2DBCodeQueue.Reset();
	m_pRecvBuff.Reset();
}

CGameServer::~CGameServer()
{
	if (m_S2DBCodeQueue != NULL)
	{
		m_S2DBCodeQueue.Free();
	}
	if (m_S2DBCodeQueue != NULL)
	{
		m_S2DBCodeQueue.Free();
	}
	if (m_pRecvBuff != NULL)
	{
		m_pRecvBuff.Free();
	}
}


// 运行准备
bool CGameServer::PrepareToRun()
{
	CSafePtr<CServerConfig> pConfig = CServerConfig::GetSingletonPtr();
	CSafePtr<ServerInfo> pGameInfo = pConfig->GetServerInfo(enServerType::FE_GAMESERVER);
	int nRet = InitTcpServer(eTcpEpoll, pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
	if (nRet == 0)
	{
		DISK_LOG(DEBUG_DISK, "CGameServer InitTcpServer success at {} : {}", pGameInfo->m_sHost.c_str(), pGameInfo->m_iPort);
		return true;
	}
	else
	{
		DISK_LOG(DEBUG_DISK, "CGameServer PrepareToRun at {} : {} failed,failed reason {]", pGameInfo->m_sHost.c_str(),
			pGameInfo->m_iPort, strerror(errno));
		return false;
	}

	m_DB2SCodeQueue = CShmMessQueue::CreateInstance(DB2S_SHM_KEY, PIPE_SIZE);
	if (m_DB2SCodeQueue == NULL)
	{
		return false;
	}

	m_S2DBCodeQueue = CShmMessQueue::CreateInstance(S2DB_SHM_KEY, PIPE_SIZE);
	if (m_S2DBCodeQueue == NULL)
	{
		return false;
	}

	m_pRecvBuff = new CByteBuff(MAX_PACKAGE_LEN);
	if (m_pRecvBuff == NULL)
	{
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
	{}
}

//
CSafePtr<CTCPConn> CGameServer::CreateTcpConn(CSocket socket)
{
	CSafePtr<CGamePlayer> pConn = new CGamePlayer(socket);
	return pConn.DynamicCastTo<CTCPConn>();
}

// 退出
void CGameServer::Exit()
{
	exit(0);
}

//读取客户端上行数据
void CGameServer::ProcessClientMessage(CSafePtr<CGamePlayer> pGamePlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt();
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	int nCmd = pRecvBuff->ReadInt();
	int nSeq = pRecvBuff->ReadInt();
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nCmd);
	if (pFactory == NULL)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
		//断开连接
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

//处理读取服务器数据
void CGameServer::ProcessServerMessage(CSafePtr<CServerClient> pServerPlayer)
{
	CSafePtr<CByteBuff> pRecvBuff = pServerPlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt();
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//断开连接
		return;
	}
	int nCmd = pRecvBuff->ReadInt();
	if (nCmd != ProxyMessage::Msg::ProxyMessage_Msg_MsgID)
	{
		//断开连接
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
		//断开连接
		return;
	}

	shared_ptr<ProtoMess> pMessage = pFactory->CreateMessage();
	ASSERT(pMessage != NULL);
	if (!pMessage->ParseFromArray(m_CacheData, packLen))
	{
		//断开连接
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
	//非gameserver 主动请求关闭
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

// 通过消息ID获取模块类型
int CGameServer::GetModuleClass(int iMsgID)
{
//    return ((iMsgID >> 16) & 0xFF);
	return iMsgID / 100;
}

void CGameServer::RecvDBMessage()
{
	while (!m_S2DBCodeQueue->IsEmpty())
	{
		m_pRecvBuff->Clear();
		int iTmpLen = 0;
		//获取成功
		int iRet = 0;

		//没有数据可读
		if (m_S2DBCodeQueue->IsEmpty())
		{
			return;
		}
		if ((iTmpLen = m_S2DBCodeQueue->ReadHeadMessage((BYTE*)(m_pRecvBuff->GetData()))) > 0)
		{
			m_pRecvBuff->WriteLen(iTmpLen);
		}
		else
		{
			CACHE_LOG(TCP_ERROR, "CProxyServer::m_S2CCodeQueue->GetHeadCode failed,error code {}", iRet);
			return;
		}

		if (iTmpLen > GAMEPLAYER_RECV_BUFF_LEN)
		{
			CACHE_LOG(TCP_ERROR, "CProxyServer::m_S2CCodeQueue->GetHeadCode len illegal,len {}", iTmpLen);
			return;
		}
		int iTmpRet = 0;
		iTmpRet = m_pRecvBuff->ReadBytes(m_CacheData, iTmpLen);
		if (iTmpLen != 0)
		{
			CACHE_LOG(TCP_ERROR, "CProxyServer::m_pRecvBuff->ReadBytes failed,iTmpRet {}", iTmpRet);
			return;
		}

		// 		CMessG2G msgG2g;
		// 		iTmpRet = msgG2g.ParseFromArray(m_CacheData, iTmpLen);
		// 		if (iTmpLen == false)
		// 		{
		// 			CACHE_LOG(TCP_ERROR, "CProxyServer::msgG2g.ParseFromArray failed,iTmpRet {}", iTmpRet);
		// 			return;
		// 		}
		// 		SendToClient(msgG2g);
	}
}

int CGameServer::SendMessageToDB(char* data, int iTmpLen)
{
	return m_DB2SCodeQueue->SendMessage((BYTE*)data, iTmpLen);
}