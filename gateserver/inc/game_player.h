/*****************************************************************
* FileName:gameplayer.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "tcp_conn.h"

#define  GAMEPLAYER_RECV_BUFF_LEN (1024 * 1024 * 2)  //2mb
#define  GAMEPLAYER_SEND_BUFF_LEN (1024 * 1024 * 2)  //2mb

class CGamePlayer : public CTCPConn
{
public:
	CGamePlayer();
	virtual int DoRecvLogic();
	virtual int DoWriteLogic();
	virtual int DoErrorLogic(int errcode);
private:
};

#endif //__GAMEPLAYER_H__
