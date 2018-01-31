//
// Created by dguco on 18-1-30.
//

#include <config.h>
#include <sharemem.h>
#include <client_comm_engine.h>
#include "../inc/c2s_handle.h"
#include "../inc/gate_def.h"

CC2sHandle::CC2sHandle()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);
	system("touch ./cspipefile");
	char *pcTmpCSPipeID = getenv("CS_PIPE_ID");
	int iTmpCSPipeID = 0;
	if (pcTmpCSPipeID) {
		iTmpCSPipeID = atoi(pcTmpCSPipeID);
	}
	key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
	BYTE *pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
	MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize, EIMode::SHM_INIT);
	m_pC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);
}

CC2sHandle::~CC2sHandle()
{
}

bool CC2sHandle::BeginListen()
{
	ServerInfo *gateInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GATESERVER);
	return CNetWork::GetSingletonPtr()->BeginListen(gateInfo->m_sHost.c_str(),
													(short) gateInfo->m_iPort,
													&OnAcceptCns,
													&OnCnsDisconnected,
													&OnCnsSomeDataSend,
													&OnCnsSomeDataRecv);
}

void CC2sHandle::ClearSocket(CAcceptor *pAcceptor, short iError)
{
	//非gameserver 主动请求关闭
	if (TCP_SUCCESS != iError) {
		DisConnect(pAcceptor, iError);
	}
	CNetWork::GetSingletonPtr()->ShutDownAcceptor(pAcceptor->GetSocket().GetSocket());
}

void CC2sHandle::DisConnect(CAcceptor *pAcceptor, short iError)
{
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

static void CC2sHandle::OnAcceptCns(uint32 uId, CAcceptor *pAcceptor)
{
}

static void CC2sHandle::OnCnsDisconnected(CAcceptor *pAcceptor)
{
	//客户端主动断开连接
	ClearSocket(pAcceptor, Err_ClientClose);
}

static void CC2sHandle::OnCnsSomeDataSend(CAcceptor *pAcceptor)
{
}

static void CC2sHandle::OnCnsSomeDataRecv(CAcceptor *pAcceptor)
{
	PACK_LEN tmpPackLen = pAcceptor->GetRecvPackLen();
	//如果当前包长度为0，则为新的数据包，重新读取数据包总长度
	if (tmpPackLen <= 0) {
		tmpPackLen = pAcceptor->GetRecvPackLen();
	}
	if (tmpPackLen <= 0) {
		//数据包不完整继续等其他数据到来
		return;
	}

	PACK_LEN tmpLastLen = tmpPackLen - sizeof(PACK_LEN);
	unsigned int tmpDataLen = pAcceptor->GetRecvDataSize();
	//数据包不完整继续等其他数据到来
	if (tmpDataLen < tmpLastLen) {
		return;
	}
	//读取数据
	pAcceptor->RecvData(m_acRecvBuff, tmpLastLen);
	pAcceptor->CurrentPackRecved();
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
	bool iRet = BeginListen();
	if (!iRet) {
		LOG_ERROR("default", "Listen failed...");
	}
	return 0;
}

int CC2sHandle::Run()
{
	//libevent事件循环
	CNetWork::GetSingletonPtr()->DispatchEvents();
	return 0;
}

bool CC2sHandle::IsToBeBlocked()
{
	return false;
}
