#include "game_player.h"
#include "common_def.h"
#include "game_server.h"

CGamePlayer::CGamePlayer(CSocket socket)
	:CTCPConn(socket,GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{

}

int CGamePlayer::DoRecvLogic()
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