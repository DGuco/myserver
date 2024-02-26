/*****************************************************************
* FileName:gameplayer.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "tcp_conn.h"

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
