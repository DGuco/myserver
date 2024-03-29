/*****************************************************************
* FileName:game_player.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "tcp_conn.h"

class CGamePlayer : public CTCPConn
{
public:
	CGamePlayer(CSocket socket);
	virtual int		DoRecvLogic();
	virtual int		DoWriteLogic();
	virtual int		DoErrorLogic(int errcode);
	virtual void	DoTick(time_t now);
private:
};

#endif //__GAME_PLAYER_H__

