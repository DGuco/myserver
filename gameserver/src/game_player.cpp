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
	//包头前两个字节为数据总长度，如果数据长度小于两个字节返回0
	if (tmCurPacketLen < sizeof(mshead_size))
	{
		return ERR_RECV_OK;
	}
	mshead_size tmPacketLen = m_pReadBuff->ReadT<mshead_size>(true);
	int tmFullPacketLen = tmPacketLen + sizeof(mshead_size);
	//数据不完整
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_RECV_OK;
	}
	//有完整的数据包，读取处理
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