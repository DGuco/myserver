#include "game_player.h"
#include "game_server.h"

CGamePlayer::CGamePlayer(CSocket socket)
	:CTCPConn(socket,
		GAMEPLAYER_RECV_BUFF_LEN, 
		GAMEPLAYER_SEND_BUFF_LEN,
		GAMEPLAYER_RECV_BUFF_LEN, 
		GAMEPLAYER_SEND_BUFF_LEN)
{}

int CGamePlayer::DoRecvLogic()
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
	CGameServer::GetSingletonPtr()->ProcessClientMessage(this);
	return ERR_RECV_OK;
}

int CGamePlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CGamePlayer::DoClosingLogic(int errcode)
{
	return 0;
}

void CGamePlayer::DoTick(time_t now)
{

}