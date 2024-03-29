/*****************************************************************
* FileName:proxy_player.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PROXY_PLAYER_H__
#define __PROXY_PLAYER_H__

#include "tcp_conn.h"

class CProxyPlayer : public CTCPConn
{
public:
	CProxyPlayer(CSocket socket);
	virtual ~CProxyPlayer();
	virtual int		DoRecvLogic();
	virtual int		DoWriteLogic();
	virtual int		DoErrorLogic(int errcode);
	virtual void	DoTick(time_t now);
private:
};

#endif //__GAMEPLAYER_H__
