//
// Created by dguco on 18-1-30.
//

#include "mes_handle.h"
#include "common_def.h"
#include "shm_api.h"
#include "gate_server.h"

CMessHandle::CMessHandle()
{
	m_C2SCodeQueue = NULL;
	m_S2CCodeQueue = NULL;
	m_pRecvBuff = NULL;
}

CMessHandle::~CMessHandle()
{
}

bool CMessHandle::PrepareToRun()
{
	m_pRecvBuff = new CByteBuff(MAX_PACKAGE_LEN);
	return CreatePipe( );
}

void CMessHandle::Run()
{
	RecvGameData();
	DealMsg();
}

bool CMessHandle::CreatePipe()
{
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
	return true;
}

int CMessHandle::SendClientData(CMessG2G& tmpMes)
{
	int nTmpSocket;
	auto tmpSendList = tmpMes.socketinfos( );
	for (int i = 0; i < tmpSendList.size( ); ++i) 
	{
		const CSocketInfo &tmpSocketInfo = tmpSendList.Get(i);
        CGateServer::GetSingletonPtr()->SendToClient(tmpSocketInfo, tmpMes.messerial().c_str(), tmpMes.messerial().length());
	}
	return 0;
}

void CMessHandle::DealMsg()
{
	while (!m_S2CCodeQueue->IsEmpty( ))
	{
        RecvGameData( );
	}
}

void CMessHandle::RecvGameData()
{
	m_pRecvBuff->Clear( );
	int iTmpLen = 0;
	//获取成功
	int iRet = 0;

	//没有数据可读
	if (m_S2CCodeQueue->IsEmpty())
	{
		return;
	}
	if ((iTmpLen = m_S2CCodeQueue->ReadHeadMessage((BYTE *) (m_pRecvBuff->GetData( )))) > 0)
	{
		m_pRecvBuff->WriteLen(iTmpLen);
	}
	else
	{
		DISK_LOG(TCP_ERROR, "CMessHandle::m_S2CCodeQueue->GetHeadCode failed,error code {}", iRet);
		return;
	}

	if (iTmpLen > GAMEPLAYER_RECV_BUFF_LEN)
	{
		DISK_LOG(TCP_ERROR, "CMessHandle::m_S2CCodeQueue->GetHeadCode len illegal,len {}", iTmpLen);
		return;
	}
	int iTmpRet = 0;
	iTmpRet = m_pRecvBuff->ReadBytes(m_CacheData, iTmpLen);
	if (iTmpLen != 0)
	{
		DISK_LOG(TCP_ERROR, "CMessHandle::m_pRecvBuff->ReadBytes failed,iTmpRet {}", iTmpRet);
		return;
	}

	CMessG2G msgG2g;
	iTmpRet = msgG2g.ParseFromArray(m_CacheData, iTmpLen);
	if (iTmpLen == false)
	{
		DISK_LOG(TCP_ERROR, "CMessHandle::msgG2g.ParseFromArray failed,iTmpRet {}", iTmpRet);
		return;
	}
	SendClientData(msgG2g);
}

int CMessHandle::SendToGame(char *data, int iTmpLen)
{
	return m_C2SCodeQueue->SendMessage((BYTE *) data, iTmpLen);
}
