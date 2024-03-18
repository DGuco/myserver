//
// Created by dguco on 18-1-30.
//
#include "config.h"
#include "my_assert.h"
#include "gate_server.h"
#include "game_player.h"
#include "gate_def.h"
#include "time_helper.h"

CGateServer::CGateServer() : CTCPServer()
{
	m_C2SCodeQueue.Reset();
	m_S2CCodeQueue.Reset();
	m_pRecvBuff.Reset();
}

CGateServer::~CGateServer()
{

}

bool CGateServer::PrepareToRun()
{
	m_pRecvBuff = new CByteBuff(MAX_PACKAGE_LEN);
	// create c2spipe
	m_C2SCodeQueue = CShmMessQueue::CreateInstance(C2S_SHM_KEY, PIPE_SIZE);
	if (m_C2SCodeQueue == NULL)
	{
		return false;
	}

	// create s2cpipe
	m_S2CCodeQueue = CShmMessQueue::CreateInstance(S2C_SHM_KEY, PIPE_SIZE);
	if (m_S2CCodeQueue == NULL)
	{
		return false;
	}

	SafePointer<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
	SafePointer<ServerInfo> gateInfo = tmpConfig->GetServerInfo(enServerType::FE_GATESERVER);
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
	}
	return false;
}

void CGateServer::ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError) 
	{
		DisConnect(pGamePlayer, iError);
	}
}

void CGateServer::DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	static CMessage tmpMessage;
	tmpMessage.Clear( );
	CMesHead *tmpHead = tmpMessage.mutable_msghead( );
	CSocketInfo *pSocketInfo = tmpHead->mutable_socketinfos( )->Add( );
	if (pSocketInfo == NULL) 
	{
		return;
	}
	pSocketInfo->set_socketid(pGamePlayer->GetSocketFD());
	pSocketInfo->set_createtime(pGamePlayer->GetCreateTime( ));
	pSocketInfo->set_state(iError);

// 	int iRet =
// 		CClientCommEngine::ConvertToGameStream(m_pSendBuff, &tmpMessage);
// 	if (iRet != 0) {
// 		LOG_ERROR("default", "[{}: {} : {}] ConvertMsgToStream failed,iRet = {} ",
// 				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
// 		return;
// 	}
// 	shared_ptr<CMessHandle> &tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetMesManager();
// 	iRet = tmpServerHandle->SendToGame(m_pSendBuff->CanReadData( ), m_pSendBuff->ReadableDataLen( ));
// 	if (iRet == 0) {
// 		LOG_DEBUG("default",
// 				  "Client disconnected ,socket {},reason {}",
// 				  tmpAcceptor->GetSocket( ).GetSocket( ),
// 				  iError);
// 	}
// 	else {
// 		LOG_ERROR("defalut", "CNetManager::DisConnect to game error, error code {}", iRet);
// 		ClearSocket(tmpAcceptor, Err_SendToMainSvrd);
// 	}
	return;
}

void CGateServer::RecvClientData(SafePointer<CGamePlayer> pGamePlayer)
{
	SafePointer<CByteBuff> pRecvBuff = pGamePlayer->GetReadBuff();
	int packLen = pRecvBuff->ReadUnInt(true);
	if (packLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	if (pRecvBuff->ReadBytes(m_CacheData, packLen) != 0)
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	time_t tNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	pGamePlayer->SetLastRecvKeepLive(tNow);

	CMessG2G msgG2g;
	CSocketInfo* tmpSocketInfo = msgG2g.mutable_socketinfos()->Add();
	tmpSocketInfo->Clear();
	tmpSocketInfo->set_createtime(pGamePlayer->GetCreateTime());
	tmpSocketInfo->set_socketid(pGamePlayer->GetSocketFD());
	tmpSocketInfo->set_state(0);
	msgG2g.mutable_messerial()->reserve(packLen);
	if (!msgG2g.SerializeToArray(m_CacheData, GAMEPLAYER_RECV_BUFF_LEN))
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	msgG2g.mutable_messerial()->assign((char*)m_CacheData, packLen);
	int iTmRet = SendToGame((char*)m_CacheData, msgG2g.GetCachedSize());
	if (iTmRet != 0)
	{
		CACHE_LOG(TCP_ERROR, "CNetManager::DealClientData to game error, error code {}", iTmRet);
		ClearSocket(pGamePlayer, Err_SendToMainSvrd);
	}
	else 
	{
		CACHE_LOG(TCP_ERROR, "gate ==>game succeed");
	}
	return;
}


int CGateServer::SendToClient(CMessG2G& tmpMes)
{
	int nTmpSocket;
	auto tmpSendList = tmpMes.socketinfos();
	for (int i = 0; i < tmpSendList.size(); ++i)
	{
		const CSocketInfo& tmpSocketInfo = tmpSendList.Get(i);
		SendToClient(tmpSocketInfo, tmpMes.messerial().c_str(), tmpMes.messerial().length());
	}
	return 0;
}

void CGateServer::RecvGameData()
{
	while (!m_S2CCodeQueue->IsEmpty())
	{
		m_pRecvBuff->Clear();
		int iTmpLen = 0;
		//获取成功
		int iRet = 0;

		//没有数据可读
		if (m_S2CCodeQueue->IsEmpty())
		{
			return;
		}
		if ((iTmpLen = m_S2CCodeQueue->ReadHeadMessage((BYTE*)(m_pRecvBuff->GetData()))) > 0)
		{
			m_pRecvBuff->WriteLen(iTmpLen);
		}
		else
		{
			CACHE_LOG(TCP_ERROR, "CMessHandle::m_S2CCodeQueue->GetHeadCode failed,error code {}", iRet);
			return;
		}

		if (iTmpLen > GAMEPLAYER_RECV_BUFF_LEN)
		{
			CACHE_LOG(TCP_ERROR, "CMessHandle::m_S2CCodeQueue->GetHeadCode len illegal,len {}", iTmpLen);
			return;
		}
		int iTmpRet = 0;
		iTmpRet = m_pRecvBuff->ReadBytes(m_CacheData, iTmpLen);
		if (iTmpLen != 0)
		{
			CACHE_LOG(TCP_ERROR, "CMessHandle::m_pRecvBuff->ReadBytes failed,iTmpRet {}", iTmpRet);
			return;
		}

		CMessG2G msgG2g;
		iTmpRet = msgG2g.ParseFromArray(m_CacheData, iTmpLen);
		if (iTmpLen == false)
		{
			CACHE_LOG(TCP_ERROR, "CMessHandle::msgG2g.ParseFromArray failed,iTmpRet {}", iTmpRet);
			return;
		}
		SendToClient(msgG2g);
	}
}

int CGateServer::SendToGame(char* data, int iTmpLen)
{
	return m_C2SCodeQueue->SendMessage((BYTE*)data, iTmpLen);
}

void CGateServer::OnNewConnect(SafePointer<CTCPConn> pConnn)
{
	SafePointer<CGamePlayer> pConn = pConnn.DynamicCastTo<CGamePlayer>();
	if (pConnn != NULL)
	{

	}
}

//
SafePointer<CTCPConn> CGateServer::CreateTcpConn(CSocket tmSocket)
{
	SafePointer<CGamePlayer> pConn = new CGamePlayer();
	return pConn.DynamicCastTo<CTCPConn>();
}

//
SafePointer<CTCPClient> CGateServer::CreateTcpClient(CSocket tmSocket)
{
	return NULL;
}

void CGateServer::SendToClient(const CSocketInfo& socketInfo, const char* data, unsigned int len)
{
	SafePointer<CGamePlayer> pGamePlayer = FindTcpConn(socketInfo.socketid()).DynamicCastTo<CGamePlayer>();
	if (pGamePlayer == NULL)
	{
		CACHE_LOG(TCP_ERROR, "CAcceptor has gone, socket = {}", socketInfo.socketid());
		return;
	}

	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送l
	*/
	if (pGamePlayer->GetCreateTime() != socketInfo.createtime())
	{
		CACHE_LOG(TCP_ERROR,
			"sokcet[{}] already closed(tcp createtime:{}:gate createtime:{}) : gate ==> client failed",
			socketInfo.socketid(),
			pGamePlayer->GetCreateTime(),
			socketInfo.createtime());
		return;
	}
	int iTmpCloseFlag = socketInfo.state();
	int iRet = pGamePlayer->Write((BYTE*)data, len);
	if (iRet != 0)
	{
		//发送失败
		ClearSocket(pGamePlayer, Err_ClientClose);
		CACHE_LOG(TCP_ERROR,
			"send to client {} Failed due to error {}",
			pGamePlayer->GetSocketFD(),
			errno);
		return;
	}

	//gameserver 主动关闭
	if (iTmpCloseFlag < 0)
	{
		ClearSocket(pGamePlayer, Client_Succeed);
	}
}