#include "server_client.h"
#include "game_server.h"


CServerClient::CServerClient()
	:CTCPClient(SERVER_CLIENT_RECV_BUFF,
				SERVER_CLIENT_SEND_BUFF,
				SERVER_CLIENT_RECV_BUFF_MAX,
				SERVER_CLIENT_SEND_BUFF_MAX)
{}

CServerClient::~CServerClient()
{}

int CServerClient::DoRecvLogic()
{
	int tmCurPacketLen = m_pReadBuff->CanReadLen();
	//��ͷǰ�����ֽ�Ϊ�����ܳ��ȣ�������ݳ���С�������ֽڷ���0
	if (tmCurPacketLen < sizeof(mshead_size))
	{
		return ERR_RECV_OK;
	}
	mshead_size tmPacketLen = m_pReadBuff->ReadT<mshead_size>(true);
	int tmFullPacketLen = tmPacketLen + sizeof(mshead_size);
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

int CServerClient::DoClosingLogic(int errcode)
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