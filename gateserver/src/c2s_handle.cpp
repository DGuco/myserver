//
// Created by dguco on 18-1-30.
//

#include <config.h>
#include <client_comm_engine.h>
#include <my_macro.h>
#include "../inc/c2s_handle.h"
#include "../inc/gate_def.h"
#include "../inc/gate_ctrl.h"

CCodeQueue *CC2sHandle::m_pC2SPipe = NULL;

char CC2sHandle::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

char CC2sHandle::m_acSendBuff[MAX_PACKAGE_LEN] = {0};

CC2sHandle::CC2sHandle(eNetModule netModule)
	: m_pNetWork(new CNetWork(netModule))
{
}

CC2sHandle::~CC2sHandle()
{
	SAFE_DELETE(m_pNetWork);
}

bool CC2sHandle::BeginListen()
{
	ServerInfo *gateInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GATESERVER);
	bool iRet = m_pNetWork->BeginListen(gateInfo->m_sHost.c_str(),
										gateInfo->m_iPort,
										&lcb_OnAcceptCns,
										&lcb_OnCnsDisconnected,
										&lcb_OnCnsSomeDataRecv,
										RECV_QUEUQ_MAX);
	if (iRet) {
		LOG_INFO("default", "Server listen success at %s : %d", gateInfo->m_sHost.c_str(), gateInfo->m_iPort);
		return true;
	}
	else {
		exit(0);
	}
}

void CC2sHandle::lcb_OnAcceptCns(unsigned int uId, CAcceptor *pAcceptor)
{
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingThreadPool()
		->PushTaskBack([uId, pAcceptor]
					   {
						   CNetWork::GetSingletonPtr()->InsertNewAcceptor(uId, pAcceptor);
					   }
		);
}

void CC2sHandle::lcb_OnCnsDisconnected(CAcceptor *pAcceptor)
{
	//客户端主动断开连接
	CGateCtrl::GetSingletonPtr()->GetSingThreadPool()
		->PushTaskBack(CC2sHandle::ClearSocket, pAcceptor, Err_ClientClose);
}

void CC2sHandle::lcb_OnCnsSomeDataRecv(CAcceptor *pAcceptor)
{
	MY_ASSERT(pAcceptor != NULL, return);
	//数据不完整
	if (!pAcceptor->IsPackageComplete()) {
		return;
	}
	int iTmpLen =  pAcceptor->GetRecvPackLen() - sizeof(PACK_LEN);
	//读取数据
	pAcceptor->RecvData(m_acRecvBuff,iTmpLen);
	//当前数据包已全部读取，清除当前数据包缓存长度
	pAcceptor->CurrentPackRecved();
	//发送数据包到game server
	CGateCtrl::GetSingletonPtr()->GetSingThreadPool()->PushTaskBack(&CC2sHandle::SendToGame, pAcceptor, iTmpLen);
}

void CC2sHandle::ClearSocket(CAcceptor *pAcceptor, short iError)
{
	MY_ASSERT(pAcceptor != NULL, return)
	//非gameserver 主动请求关闭
	if (Client_Succeed != iError) {
		DisConnect(pAcceptor, iError);
	}
	CNetWork::GetSingletonPtr()->ShutDownAcceptor(pAcceptor->GetSocket().GetSocket());
}

void CC2sHandle::DisConnect(CAcceptor *pAcceptor, short iError)
{
	MY_ASSERT(pAcceptor != NULL, return)
	MesHead tmpHead;
	CSocketInfo *pSocketInfo = tmpHead.mutable_socketinfos()->Add();
	if (pSocketInfo == NULL) {
		LOG_ERROR("default", "CTcpCtrl::DisConnect add_socketinfos ERROR");
		return;
	}
	pSocketInfo->set_socketid(pAcceptor->GetSocket().GetSocket());
	pSocketInfo->set_createtime(pAcceptor->GetCreateTime());
	pSocketInfo->set_state(iError);

	PACK_LEN unTmpMsgLen = sizeof(m_acSendBuff);
	int iRet = CClientCommEngine::ConvertToGameStream(m_acSendBuff, unTmpMsgLen, &tmpHead);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s: %d : %s] ConvertMsgToStream failed,iRet = %d ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}

	iRet = m_pC2SPipe->AppendOneCode((BYTE *) m_acSendBuff, unTmpMsgLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s: %d : %s] Send data to GateServer failed,iRet = %d ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}
}

void CC2sHandle::SendToGame(CAcceptor *pAcceptor, PACK_LEN tmpLastLen)
{
	MesHead tmpHead;
	char *pTmp = m_acRecvBuff;
	int iTmpRet = CClientCommEngine::ParseClientStream((const void **) &pTmp, tmpLastLen, &tmpHead);
	if (iTmpRet < 0) {
		//断开连接
		ClearSocket(pAcceptor, Err_PacketError);
		return;
	}
	//组织转发消息
	if (0 == iTmpRet /*&& tmpHead.cmd() != CMsgPingRequest::MsgID */&& tmpLastLen >= 0) {
		CSocketInfo *tmpSocketInfo = tmpHead.mutable_socketinfos()->Add();
		tmpSocketInfo->Clear();
		tmpSocketInfo->set_createtime(pAcceptor->GetCreateTime());
		tmpSocketInfo->set_socketid(pAcceptor->GetSocket().GetSocket());
		tmpSocketInfo->set_state(0);

		PACK_LEN tmpSendLen = sizeof(m_acSendBuff);
		iTmpRet = CClientCommEngine::ConverToGameStream(m_acSendBuff,
														tmpSendLen,
														pTmp,
														tmpLastLen,
														&tmpHead);
		if (iTmpRet != 0) {
			//断开连接
			ClearSocket(pAcceptor, Err_PacketError);
			return;
		}

		//发送给game server
		iTmpRet = m_pC2SPipe->AppendOneCode((const BYTE *) m_acSendBuff, tmpSendLen);
		if (iTmpRet < 0) {
			//断开连接
			ClearSocket(pAcceptor, Err_SendToMainSvrd);
			return;
		}
#ifdef _DEBUG_
		LOG_DEBUG("defalut", "tcp ==>gate [%d bytes]", tmpSendLen);
#endif
	}
	else {
		//心跳信息不做处理
	}
}

int CC2sHandle::PrepareToRun()
{
	BeginListen();
	return 0;
}

int CC2sHandle::Run()
{
	LOG_INFO("default", "Libevent run with net module %s",
			 event_base_get_method(reinterpret_cast<const event_base *>(CNetWork::GetSingletonPtr()
				 ->GetEventReactor()->GetEventBase())));
	//libevent事件循环
	m_pNetWork->DispatchEvents();
}

CNetWork *CC2sHandle::GetNetWork()
{
	return m_pNetWork;
}
