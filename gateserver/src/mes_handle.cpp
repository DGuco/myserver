//
// Created by dguco on 18-1-30.
//

#include <share_mem.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include <my_assert.h>
#include "net_work.h"
#include "../inc/mes_handle.h"
#include "../inc/gate_ctrl.h"

CMessHandle::CMessHandle(const string &threadName, long timeOut)
	: CMyThread(threadName, timeOut),
	  m_pRecvBuff(std::make_shared<CByteBuff>( ))
{
}

CMessHandle::~CMessHandle()
{
}

int CMessHandle::PrepareToRun()
{
	CreatePipe( );
	return true;
}

void CMessHandle::CreatePipe()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

	// create s2cpipe
	system("touch ./scpipefile");

	char *pcTmpSCPipeID = getenv("SC_PIPE_ID");
	int iTmpSCPipeID = 0;
	if (pcTmpSCPipeID) {
		iTmpSCPipeID = atoi(pcTmpSCPipeID);
	}

	key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
	BYTE *pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
	MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));

	CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize);
	m_S2CCodeQueue = shared_ptr<CCodeQueue>(CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C));

	// create c2spipe
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
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize);
	m_S2CCodeQueue = shared_ptr<CCodeQueue>(CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S));
}

int CMessHandle::SendClientData(CMessage &tmpMes, char *data, int len)
{
	int nTmpSocket;
	auto tmpSendList = tmpMes.msghead( ).socketinfos( );
	for (int i = 0; i < tmpSendList.size( ); ++i) {
		//向后移动socket索引
		const CSocketInfo &tmpSocketInfo = tmpSendList.Get(i);
		nTmpSocket = tmpSocketInfo.socketid( );
		//socket 非法
		if (nTmpSocket <= 0 || MAX_SOCKET_NUM <= nTmpSocket) {
			LOG_ERROR("default", "Invalid socket index {}", nTmpSocket);
			continue;
		}
		CGateCtrl::GetSingletonPtr( )->GetClientManager( )->SendToClient(tmpSocketInfo, data, len);
	}
	return 0;
}

void CMessHandle::RunFunc()
{
	while (!m_S2CCodeQueue->IsQueueEmpty( )) {
		CGateCtrl::GetSingletonPtr( )->GetSingleThreadPool( )->PushTaskBack(
			[this]
			{
				RecvGameData( );
			});
	}
}

void CMessHandle::RecvGameData()
{
	m_pRecvBuff->Clear( );
	int iTmpLen = 0;
	//获取成功
	int iRet = 0;
	if ((iRet = m_S2CCodeQueue->GetHeadCode((BYTE *) (m_pRecvBuff->CanWriteData( )), iTmpLen)) == 0) {
		m_pRecvBuff->WriteLen(iTmpLen);
	}
	else {
		LOG_ERROR("default", "CMessHandle::m_S2CCodeQueue->GetHeadCode failed,error code {}", iRet);
		return;
	}
	int iTmpRet = 0;

	CMessage tmpMes;
	iTmpRet = CClientCommEngine::ConvertStreamToMessage(m_pRecvBuff.get( ),
														iTmpLen,
														&tmpMes,
														NULL);
	//序列化失败继续发送
	if (iTmpRet < 0) {
		LOG_ERROR("default",
				  "CMessHandle::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return {}.",
				  iTmpRet);
		return;
	}

	SendClientData(tmpMes, m_pRecvBuff->CanReadData( ), m_pRecvBuff->ReadableDataLen( ));
}

bool CMessHandle::IsToBeBlocked()
{
	return !m_S2CCodeQueue->IsQueueEmpty( );
}

int CMessHandle::SendToGame(char *data, int iTmpLen)
{
	return m_C2SCodeQueue->AppendOneCode((BYTE *) data, iTmpLen);
}
