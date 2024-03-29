#include "server_client.h"
#include "common_def.h"
#include "game_server.h"


CServerClient::CServerClient()
	:CTCPClient(GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{}

CServerClient::~CServerClient()
{}

int CServerClient::DoRecvLogic()
{
	msize_t tmCurPacketLen = m_pReadBuff->CanReadLen();
	//��ͷǰ�����ֽ�Ϊ�����ܳ��ȣ�������ݳ���С�������ֽڷ���0
	if (tmCurPacketLen < sizeof(msize_t))
	{
		return ERR_RECV_OK;
	}
	msize_t tmPacketLen = m_pReadBuff->ReadUnInt(true);
	msize_t tmFullPacketLen = tmPacketLen + sizeof(msize_t);
	//���ݲ�����
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_RECV_OK;
	}
	//�����������ݰ�����ȡ����
	CGameServer::GetSingletonPtr()->ProcessServerMessage(this);
	return ERR_RECV_OK;
}

int CServerClient::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CServerClient::DoErrorLogic(int errcode)
{
	return 0;
}

void CServerClient::DoTick(time_t nNow)
{
	return;
}

void CServerClient::SetLastRecvKeepLive(time_t nKeepAliveTime)
{
	m_nLastRecvKeepLiveAnswer = nKeepAliveTime;
}

time_t CServerClient::GetLastRecvKeepLive()
{
	return m_nLastRecvKeepLiveAnswer;
}

void CServerClient::SetLastSendKeepLive(time_t nKeepAliveTime)
{
	m_nLastSendKeepLive = nKeepAliveTime;
}

time_t CServerClient::GetLastSendKeepLive()
{
	return m_nLastSendKeepLive;
}