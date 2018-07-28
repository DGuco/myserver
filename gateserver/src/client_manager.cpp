//
// Created by dguco on 18-1-30.
//

#include <config.h>
#include <client_comm_engine.h>
#include "my_assert.h"
#include "../inc/client_manager.h"
#include "../inc/gate_def.h"
#include "../inc/gate_ctrl.h"

CClientManager::CClientManager(shared_ptr<CNetWork> pNetWork)
	: m_pNetWork(pNetWork),
	  m_pSendBuff(std::make_shared<CByteBuff>()),
	  m_pRecvBuff(std::make_shared<CByteBuff>())
{
}

CClientManager::~CClientManager()
{

}

int CClientManager::PrepareToRun()
{
	BeginListen();
	return 0;
}

void CClientManager::DealClientData(CAcceptor *tmpAcceptor,
									unsigned short tmpLastLen)
{
	MY_ASSERT(tmpAcceptor != NULL, return)
	static CMessage tmpMessage;
	tmpMessage.Clear();
	CMesHead *tmpHead = tmpMessage.mutable_msghead();
	int iTmpRet = CClientCommEngine::ParseClientStream(m_pRecvBuff.get(), tmpMessage.mutable_msghead());
	if (iTmpRet < 0) {
		//断开连接
		ClearSocket(tmpAcceptor, Err_PacketError);
		return;
	}
	time_t tNow = GetMSTime();
	tmpAcceptor->SetLastKeepAlive(tNow);
	//组织转发消息
	if (0 == iTmpRet && tmpHead->cmd() != 103 && tmpLastLen >= 0) {
		CSocketInfo *tmpSocketInfo = tmpHead->mutable_socketinfos()->Add();
		tmpSocketInfo->Clear();
		tmpSocketInfo->set_createtime(tmpAcceptor->GetCreateTime());
		tmpSocketInfo->set_socketid(tmpAcceptor->GetSocket().GetSocket());
		tmpSocketInfo->set_state(0);

		shared_ptr<CServerManager> &tmpHandle = CGateCtrl::GetSingletonPtr()->GetServerManager();
		CByteBuff *sendBuff = tmpHandle->GetSendBuff().get();
		iTmpRet = CClientCommEngine::ConvertToGameStream(sendBuff,
														 m_pRecvBuff->CanReadData(),
														 tmpLastLen,
														 tmpHead);
		if (iTmpRet != 0) {
			//断开连接
			ClearSocket(tmpAcceptor, Err_PacketError);
			return;
		}

		shared_ptr<CServerManager> &tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetServerManager();
		tmpServerHandle->FlushToGame();
		LOG_DEBUG("default", "gate ==>game succeed");
	}
	else {
		//心跳信息不做处理
	}
}

void CClientManager::SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len)
{
	CAcceptor *pAcceptor = m_pNetWork->FindAcceptor(socketInfo.socketid());
	if (pAcceptor == NULL) {
		LOG_ERROR("default", "CAcceptor has gone, socket = {}", socketInfo.socketid());
		return;
	}

	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送l
	*/
	if (pAcceptor->GetCreateTime() != socketInfo.createtime()) {
		LOG_ERROR("default",
				  "sokcet[{}] already closed(tcp createtime:{}:gate createtime:{}) : gate ==> client failed",
				  socketInfo.socketid(),
				  pAcceptor->GetCreateTime(),
				  socketInfo.createtime());
		return;
	}
	int iTmpCloseFlag = socketInfo.state();
	int iRet = pAcceptor->Send(data, len);
	if (iRet != 0) {
		//发送失败
		CClientManager::ClearSocket(pAcceptor, Err_ClientClose);
		LOG_ERROR("default",
				  "send to client {} Failed due to error {}",
				  pAcceptor->GetSocket().GetSocket(),
				  errno);
		return;
	}

	//gameserver 主动关闭
	if (iTmpCloseFlag < 0) {
		CClientManager::ClearSocket(pAcceptor, Client_Succeed);
	}
}

shared_ptr<CByteBuff> &CClientManager::GetRecvBuff()
{
	return m_pRecvBuff;
}

shared_ptr<CByteBuff> &CClientManager::GetSendBuff()
{
	return m_pSendBuff;
}

shared_ptr<CNetWork> &CClientManager::GetNetWork()
{
	return m_pNetWork;
}

void CClientManager::ClearSocket(CAcceptor *tmpAcceptor, short iError)
{
	MY_ASSERT(tmpAcceptor != NULL, return)
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError) {
		DisConnect(tmpAcceptor, iError);
	}
	shared_ptr<CNetWork> &netWork = CGateCtrl::GetSingletonPtr()->GetNetWork();
	netWork->ShutDownAcceptor(tmpAcceptor->GetSocket().GetSocket());
}

void CClientManager::DisConnect(CAcceptor *tmpAcceptor, short iError)
{
	MY_ASSERT(tmpAcceptor != NULL, return);
	static CMessage tmpMessage;
	tmpMessage.Clear();
	CMesHead *tmpHead = tmpMessage.mutable_msghead();
	CSocketInfo *pSocketInfo = tmpHead->mutable_socketinfos()->Add();
	if (pSocketInfo == NULL) {
		LOG_ERROR("default", "CTcpCtrl::DisConnect add_socketinfos ERROR");
		return;
	}
	pSocketInfo->set_socketid(tmpAcceptor->GetSocket().GetSocket());
	pSocketInfo->set_createtime(tmpAcceptor->GetCreateTime());
	pSocketInfo->set_state(iError);

	int iRet =
		CClientCommEngine::ConvertToGameStream(m_pRecvBuff.get(), &tmpMessage);
	if (iRet != 0) {
		LOG_ERROR("default", "[{}: {} : {}] ConvertMsgToStream failed,iRet = {} ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}

	shared_ptr<CServerManager> &tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetServerManager();
	tmpServerHandle->SendToGameBuff(&tmpMessage);
	return;
}

void CClientManager::RecvClientData(CAcceptor *tmpAcceptor)
{
	MY_ASSERT(tmpAcceptor != NULL, return);
	//数据不完整
	if (!tmpAcceptor->IsPackageComplete()) {
		return;
	}
	int packLen = tmpAcceptor->GetRecvPackLen();
	int iTmpLen = packLen - sizeof(unsigned short);
	//读取数据
	m_pRecvBuff->Clear();
	m_pRecvBuff->WriteUnShort(packLen);
	iTmpLen = tmpAcceptor->RecvData(m_pRecvBuff->GetData(), iTmpLen);
	m_pRecvBuff->WriteLen(iTmpLen);
	//当前数据包已全部读取，清除当前数据包缓存长度
	tmpAcceptor->CurrentPackRecved();
	//发送数据包到game server
	shared_ptr<CClientManager> &tmpHandle = CGateCtrl::GetSingletonPtr()->GetClientManager();
	tmpHandle->DealClientData(tmpAcceptor, iTmpLen);
}

bool CClientManager::BeginListen()
{
	shared_ptr<CServerConfig> tmpConfig = CServerConfig::GetSingletonPtr();
	ServerInfo *gateInfo = tmpConfig->GetServerInfo(enServerType::FE_GATESERVER);
	bool iRet = m_pNetWork->BeginListen(gateInfo->m_sHost.c_str(),
										gateInfo->m_iPort,
										&CClientManager::lcb_OnAcceptCns,
										&CClientManager::lcb_OnCnsSomeDataSend,
										&CClientManager::lcb_OnCnsSomeDataRecv,
										&CClientManager::lcb_OnCnsDisconnected,
										&CClientManager::lcb_OnCheckAcceptorTimeOut,
										RECV_QUEUQ_MAX,
										tmpConfig->GetTcpKeepAlive());
	if (iRet) {
		LOG_INFO("default", "Server listen success at {} : {}", gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
		return true;
	}
	else {
		LOG_ERROR("default""Server listen at {} : {} failed,failed reason {]", gateInfo->m_sHost.c_str(),
				  gateInfo->m_iPort, strerror(errno));
		exit(0);
	}
}

void CClientManager::lcb_OnAcceptCns(unsigned int uId, IBufferEvent *tmpAcceptor)
{
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([uId, &tmpAcceptor]
					   {
						   MY_ASSERT(tmpAcceptor != NULL, return);
						   LOG_DEBUG("default", "New acceptor,socket id {}", tmpAcceptor->GetSocket().GetSocket());
						   CGateCtrl::GetSingletonPtr()->GetNetWork()
							   ->InsertNewAcceptor(uId, (CAcceptor *) (tmpAcceptor));
					   }
		);
}

void CClientManager::lcb_OnCnsDisconnected(IBufferEvent *tmpAcceptor)
{
	MY_ASSERT(tmpAcceptor != NULL, return);
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[tmpAcceptor]
			{
				std::shared_ptr<CClientManager> tmpClientManager = CGateCtrl::GetSingletonPtr()->GetClientManager();
				tmpClientManager->ClearSocket((CAcceptor *) tmpAcceptor, Err_ClientClose);
			});
}

void CClientManager::lcb_OnCnsSomeDataRecv(IBufferEvent *tmpAcceptor)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[tmpAcceptor]
			{
				std::shared_ptr<CClientManager> tmpClientManager = CGateCtrl::GetSingletonPtr()->GetClientManager();
				tmpClientManager->RecvClientData((CAcceptor *) tmpAcceptor);
			});
}

void CClientManager::lcb_OnCnsSomeDataSend(IBufferEvent *tmpAcceptor)
{

}

void CClientManager::lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(
			[]
			{
				shared_ptr<CClientManager> tmpClientManager = CGateCtrl::GetSingletonPtr()->GetClientManager();
				shared_ptr<CNetWork> &tmpNet = tmpClientManager->GetNetWork();
				std::shared_ptr<CServerConfig> &tmpConfig = CServerConfig::GetSingletonPtr();
				int tmpPingTime = tmpConfig->GetTcpKeepAlive();
				MAP_ACCEPTOR &tmpMap = tmpNet->GetAcceptorMap();
				auto it = tmpMap.begin();
				time_t tNow = GetMSTime();
				for (; it != tmpMap.end();) {
					CAcceptor *tmpAcceptor = it->second;
					if (tNow - tmpAcceptor->GetLastKeepAlive() > tmpPingTime) {
						tmpClientManager->DisConnect(tmpAcceptor, Err_ClientTimeout);
						it = tmpMap.erase(it);
					}
					else {
						it++;
					}
				}

			});

}


