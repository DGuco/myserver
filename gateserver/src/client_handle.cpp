//
// Created by dguco on 18-1-30.
//

#include <config.h>
#include <client_comm_engine.h>
#include <my_macro.h>
#include <message.pb.h>
#include "my_assert.h"
#include "../inc/client_handle.h"
#include "../inc/gate_def.h"
#include "../inc/gate_ctrl.h"

CByteBuff *CClientHandle::m_oRecvBuff = new CByteBuff;

CByteBuff *CClientHandle::m_oSendBuff = new CByteBuff;

CClientHandle::CClientHandle(shared_ptr<CNetWork> pNetWork)
	: m_pNetWork(pNetWork)
{
}

CClientHandle::~CClientHandle()
{
	SAFE_DELETE(m_pNetWork);
}

int CClientHandle::PrepareToRun()
{
	BeginListen();
	return 0;
}

void CClientHandle::Run()
{
	LOG_INFO("default", "Libevent run with net module {}",
			 event_base_get_method(reinterpret_cast<const event_base *>(CNetWork::GetSingletonPtr()
				 ->GetEventReactor()->GetEventBase())));
	//libevent事件循环
	m_pNetWork->DispatchEvents();
}

bool CClientHandle::BeginListen()
{
	CServerConfig *tmpConfig = CServerConfig::GetSingletonPtr();
	ServerInfo *gateInfo = tmpConfig->GetServerInfo(enServerType::FE_GATESERVER);
	bool iRet = m_pNetWork->BeginListen(gateInfo->m_sHost.c_str(),
										gateInfo->m_iPort,
										&CClientHandle::lcb_OnAcceptCns,
										&CClientHandle::lcb_OnCnsSomeDataSend,
										&CClientHandle::lcb_OnCnsSomeDataRecv,
										&CClientHandle::lcb_OnCnsDisconnected,
										&CClientHandle::lcb_OnCheckAcceptorTimeOut,
										RECV_QUEUQ_MAX,
										tmpConfig->GetTcpKeepAlive());
	if (iRet) {
		LOG_INFO("default", "Server listen success at {} : {}", gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
		return true;
	}
	else {
		exit(0);
	}
}

void CClientHandle::lcb_OnAcceptCns(unsigned int uId, IBufferEvent *pBufferEvent)
{
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([uId, pBufferEvent]
					   {
						   MY_ASSERT(pBufferEvent != NULL && typeid(*pBufferEvent) == typeid(CAcceptor), return);
						   LOG_DEBUG("default",
									 "New acceptor,socket id {}",
									 pBufferEvent->GetSocket().GetSocket());
						   CNetWork::GetSingletonPtr()->InsertNewAcceptor(uId, (CAcceptor *) pBufferEvent);
					   }
		);
}

void CClientHandle::lcb_OnCnsDisconnected(IBufferEvent *pAcceptor)
{
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack(CClientHandle::ClearSocket, pAcceptor, Err_ClientClose);
}

void CClientHandle::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([pBufferEvent]
					   {
						   MY_ASSERT(pBufferEvent != NULL, return);
						   //数据不完整
						   if (!pBufferEvent->IsPackageComplete()) {
							   return;
						   }
						   int iTmpLen = pBufferEvent->GetRecvPackLen() - sizeof(unsigned short);
						   //读取数据
						   iTmpLen = pBufferEvent->RecvData(m_oRecvBuff->GetData(), iTmpLen);
						   m_oRecvBuff->WriteLen(iTmpLen);
						   //当前数据包已全部读取，清除当前数据包缓存长度
						   pBufferEvent->CurrentPackRecved();
						   //发送数据包到game server
						   CClientHandle::SendToGame(pBufferEvent, iTmpLen);
					   });
}

void CClientHandle::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{

}

void CClientHandle::lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param)
{
	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()
		->PushTaskBack([param]
					   {
						   CNetWork *tmpNetWork = (CNetWork *) param;
						   if (tmpNetWork != NULL) {
							   CServerConfig *tmpConfig = CServerConfig::GetSingletonPtr();
							   int tmpPingTime = tmpConfig->GetTcpKeepAlive();
							   CNetWork::MAP_ACCEPTOR &tmpMap = tmpNetWork->GetAcceptorMap();
							   auto it = tmpMap.begin();
							   time_t tNow = GetMSTime();
							   for (; it != tmpMap.end();) {
								   CAcceptor *tmpAcceptor = it->second;
								   if (tNow - tmpAcceptor->GetLastKeepAlive() > tmpPingTime) {
									   DisConnect(tmpAcceptor, Err_ClientTimeout);
									   SAFE_DELETE(tmpAcceptor);
									   it = tmpMap.erase(it);
								   }
								   else {
									   it++;
								   }
							   }
						   }
					   });

}

void CClientHandle::ClearSocket(IBufferEvent *pAcceptor, short iError)
{
	MY_ASSERT(pAcceptor != NULL, return)
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError) {
		DisConnect(pAcceptor, iError);
	}
	CNetWork::GetSingletonPtr()->ShutDownAcceptor(pAcceptor->GetSocket().GetSocket());
}

void CClientHandle::DisConnect(IBufferEvent *pAcceptor, short iError)
{
	MY_ASSERT(pAcceptor != NULL, return)

	CAcceptor *tmpAcceptor = (CAcceptor *) pAcceptor;
	CMesHead tmpHead;
	CSocketInfo *pSocketInfo = tmpHead.mutable_socketinfos()->Add();
	if (pSocketInfo == NULL) {
		LOG_ERROR("default", "CTcpCtrl::DisConnect add_socketinfos ERROR");
		return;
	}
	pSocketInfo->set_socketid(tmpAcceptor->GetSocket().GetSocket());
	pSocketInfo->set_createtime(tmpAcceptor->GetCreateTime());
	pSocketInfo->set_state(iError);

	int iRet =
		CClientCommEngine::ConvertToGameStream(m_oSendBuff, &tmpHead);
	if (iRet != 0) {
		LOG_ERROR("default", "[{}: {} : {}] ConvertMsgToStream failed,iRet = {} ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}

	iRet = m_pC2SPipe->AppendOneCode((BYTE *) m_oSendBuff->CanReadData(), m_oSendBuff->ReadableDataLen());
	if (iRet < 0) {
		LOG_ERROR("default", "[{}: {} : {}] Send data to GateServer failed,iRet = {} ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}
}

void CClientHandle::SendToGame(IBufferEvent *pAcceptor, unsigned short tmpLastLen)
{
	MY_ASSERT(pAcceptor != NULL, return)
	CAcceptor *tmpAcceptor = (CAcceptor *) pAcceptor;

	CMesHead tmpHead;
	int iTmpRet = CClientCommEngine::ParseClientStream(m_oRecvBuff, &tmpHead);
	if (iTmpRet < 0) {
		//断开连接
		ClearSocket(pAcceptor, Err_PacketError);
		return;
	}
	time_t tNow = GetMSTime();
	tmpAcceptor->SetLastKeepAlive(tNow);
	//组织转发消息
	if (0 == iTmpRet && tmpHead.cmd() != 103 && tmpLastLen >= 0) {
		CSocketInfo *tmpSocketInfo = tmpHead.mutable_socketinfos()->Add();
		tmpSocketInfo->Clear();
		tmpSocketInfo->set_createtime(tmpAcceptor->GetCreateTime());
		tmpSocketInfo->set_socketid(pAcceptor->GetSocket().GetSocket());
		tmpSocketInfo->set_state(0);

		iTmpRet = CClientCommEngine::ConvertToGameStream(m_oSendBuff,
														 m_oRecvBuff->CanReadData(),
														 tmpLastLen,
														 &tmpHead);
		if (iTmpRet != 0) {
			//断开连接
			ClearSocket(pAcceptor, Err_PacketError);
			return;
		}

		//发送给game server
		iTmpRet = m_pC2SPipe->AppendOneCode((const BYTE *) m_oSendBuff->CanReadData(), m_oSendBuff->ReadableDataLen());
		if (iTmpRet < 0) {
			//断开连接
			ClearSocket(pAcceptor, Err_SendToMainSvrd);
			return;
		}
		LOG_DEBUG("default", "gate ==>game [{} bytes]", m_oSendBuff->ReadableDataLen());
		//检测是否有下行数据需要转发
		CServerHandle *tmpHandle = CGateCtrl::GetSingletonPtr()->GetCS2cHandle();
		if (!tmpHandle->IsToBeBlocked() && tmpHandle->GetStatus() != rt_running) {
			tmpHandle->WakeUp();
		}
	}
	else {
		//心跳信息不做处理
	}
}

CNetWork *CClientHandle::GetNetWork()
{
	return m_pNetWork;
}
