//
// Created by DGuco on 17-3-1.
//
#include "my_assert.h"
#include "common_def.h"
#include "server_config.h"
#include "base.h"
#include "my_assert.h"
#include "game_player.h"
#include "game_server.h"
#include "server_client.h"
#include "time_helper.h"
#include "module_manager.h"

CGameServer::CGameServer()
{
}

CGameServer::~CGameServer()
{
}


// 运行准备
bool CGameServer::PrepareToRun()
{
	SafePointer<CServerConfig> pConfig = CServerConfig::GetSingletonPtr();
	SafePointer<ServerInfo> pGameInfo = pConfig->GetServerInfo(enServerType::FE_GAMESERVER);
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
	SafePointer<ServerInfo> pRroxyInfo = pConfig->GetServerInfo(enServerType::FE_PROXYSERVER);
	SafePointer<CServerClient> pConn = new CServerClient();
	if (!ConnectTo(pConn.DynamicCastTo<CTCPClient>(), pRroxyInfo->m_sHost.c_str(), pRroxyInfo->m_iPort, false))
	{
		pConn.Free();
	}
	return true;
}

void CGameServer::OnNewConnect(SafePointer<CTCPConn> pConnn)
{
	SafePointer<CGamePlayer> pConn = pConnn.DynamicCastTo<CGamePlayer>();
	if (pConnn != NULL)
	{
	}
}

//
SafePointer<CTCPConn> CGameServer::CreateTcpConn(CSocket tmSocket)
{
	SafePointer<CGamePlayer> pConn = new CGamePlayer();
	return pConn.DynamicCastTo<CTCPConn>();
}

// 运行
void CGameServer::Run()
{

}

// 退出
void CGameServer::Exit()
{
	exit(0);
}

//读取客户端上行数据
void CGameServer::RecvClientData(SafePointer<CGamePlayer> pGamePlayer)
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
	CMessage tmMessage;
	if (!tmMessage.ParseFromArray(m_CacheData, packLen))
	{
		//断开连接
		ClearSocket(pGamePlayer, Err_PacketError);
		return;
	}
	ProcessClientMessage(&tmMessage, pGamePlayer);
	return;
}

void CGameServer::ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError)
	{
		DisConnect(pGamePlayer, iError);
	}
}

void CGameServer::DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError)
{
	ASSERT(pGamePlayer != NULL);
	static CMessage tmpMessage;
	tmpMessage.Clear();
	CMesHead* tmpHead = tmpMessage.mutable_msghead();
	CSocketInfo* pSocketInfo = tmpHead->mutable_socketinfos()->Add();
	if (pSocketInfo == NULL)
	{
		return;
	}
	pSocketInfo->set_socketid(pGamePlayer->GetSocketFD());
	pSocketInfo->set_createtime(pGamePlayer->GetCreateTime());
	pSocketInfo->set_state(iError);


	return;
}

void CGameServer::ProcessClientMessage(SafePointer<CMessage> pMsg, SafePointer<CGamePlayer> pPlayer)
{
	ASSERT(pMsg != NULL && pPlayer != NULL);
	int iTmpType = GetModuleClass(pMsg->msghead( ).cmd( ));
	CModuleManager::GetSingletonPtr()->OnClientMessage(iTmpType, pPlayer, pMsg);
}

void CGameServer::ProcessServerMessage(SafePointer<CProxyMessage> pMsg)
{
	ASSERT(pMsg != NULL);
	int iTmpType = GetModuleClass(pMsg->msghead( ).messageid( ));
	CModuleManager::GetSingletonPtr()->OnRouterMessage(iTmpType, pPlayer, pMsg);
}

bool CGameServer::SendMessageToDB(SafePointer<CProxyMessage> pMsg)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return 0,"Do SendMsg must be in logic thread:m_pLogicThread");
    m_pServerHandle->SendMessageToDB(pMsg);
    return true;
}

// 通过消息ID获取模块类型
int CGameServer::GetModuleClass(int iMsgID)
{
//    return ((iMsgID >> 16) & 0xFF);
	return iMsgID / 100;
}

void CGameServer::DisconnectClient(CPlayer *pPlayer)
{
	if (!pPlayer) {
		return;
	}
	m_pClientHandle->DisconnectClient(pPlayer->GetPlayerBase( )->GetSocket( ),
                                      pPlayer->GetPlayerBase( )->GetCreateTime( ));
}

// 设置服务器运行状态
void CGameServer::SetRunFlag(ERunFlag eRunFlag)
{
	m_oRunFlag.SetRunFlag(eRunFlag);
}

// 广播消息给玩家，广播时，发起人一定放第一个
int CGameServer::BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, stPointList *pTeamList)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    m_pClientHandle->BroadCastMsg( iMsgID, pMsgPara, pTeamList);
	return 0;
}

// 发送消息给单个玩家
int CGameServer::BroadcastMsg(unsigned int iMsgID, std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(pPlayer != NULL && pMsgPara != NULL, return -1);
	stPointList tmpList;
	tmpList.push_back(pPlayer);
    m_pClientHandle->SendResToPlayer( pMsgPara, pPlayer);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<CGooMess> pMsgPara, CPlayer *pPlayer)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(pPlayer != NULL && pMsgPara != NULL,
			  return -1);
	m_pClientHandle->SendResToPlayer( pMsgPara, pPlayer);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<CGooMess> pMsgPara, std::shared_ptr<CMesHead> mesHead)
{
    //MY_ASSERT_STR(m_pLogicThread->IsInThisThread(),return -1,"Do SendMsg must be in logic thread:m_pLogicThread");
    MY_ASSERT(mesHead != NULL && pMsgPara != NULL, return -1);
    m_pClientHandle->SendResponse(pMsgPara, mesHead);
}