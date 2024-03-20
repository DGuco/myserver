#include "game_player.h"
#include "common_def.h"

CGamePlayer::CGamePlayer()
	:CTCPConn(GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{

}

int CGamePlayer::DoRecvLogic()
{}

int CGamePlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CGamePlayer::DoErrorLogic(int errcode)
{
	return 0;
}