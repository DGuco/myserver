//
// Created by dguco on 18-1-30.
//

#include "mes_handle.h"
#include "common_def.h"
#include "shm_api.h"

CMessHandle::CMessHandle()
{

}

CMessHandle::~CMessHandle()
{
}

bool CMessHandle::PrepareToRun()
{
	return CreatePipe( );
}

bool CMessHandle::CreatePipe()
{
	// create c2spipe
	int iTempSize = CCodeQueue::CountQueueSize(PIPE_SIZE);
	CSharedMem c2sSharedMem;
	bool ret = c2sSharedMem.Init(SHM_INIT, C2S_SHM_KEY, iTempSize);
	if (!ret)
	{
		return false;
	}
	CCodeQueue::pCurrentShm = &c2sSharedMem;
	m_C2SCodeQueue = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);
	CCodeQueue::pCurrentShm = NULL;

	// create s2cpipe
	iTempSize = CCodeQueue::CountQueueSize(PIPE_SIZE);
	CSharedMem s2cSharedMem;
	bool ret = s2cSharedMem.Init(SHM_INIT, S2C_SHM_KEY, iTempSize);
	if (!ret)
	{
		return false;
	}

	CCodeQueue::pCurrentShm = &s2cSharedMem;
	m_S2CCodeQueue = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);
	CCodeQueue::pCurrentShm = NULL;
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
        CGateCtrl::GetSingletonPtr()->GetNetManager()->SendToClient(tmpSocketInfo, data, len);
	}
	return 0;
}

void CMessHandle::DealMsg()
{
	while (!m_S2CCodeQueue->IsQueueEmpty( )) {
        RecvGameData( );
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


int CMessHandle::SendToGame(char *data, int iTmpLen)
{
	return m_C2SCodeQueue->AppendOneCode((BYTE *) data, iTmpLen);
}
