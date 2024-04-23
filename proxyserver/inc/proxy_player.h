/*****************************************************************
* FileName:proxy_player.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PROXY_PLAYER_H__
#define __PROXY_PLAYER_H__

#include "tcp_conn.h"
#include "message.pb.h"

enum enProxyState
{
	eProInvalid = -1,  //无效
	eProConnected = 0, //连接
	eProRegistered = 1,//注册
	eProKicking = 2,   //非法连接等待踢掉
};

class CProxyPlayer : public CTCPConn
{
public:
	CProxyPlayer(CSocket socket);
	virtual ~CProxyPlayer();
	virtual int		DoRecvLogic();
	virtual int		DoWriteLogic();
	virtual int		DoErrorLogic(int errcode);
	virtual void	DoTick(time_t now);
	void			SetProxyState(short state);
	void			SetServerId(int serverid);
	void			SetServerType(enServerType servertype);
	short			GetProxyState();
	int				GetServerId();
	enServerType	GetServerType();
	time_t			GetLastRecvKeepAlive();
	void			SetLastRecvKeepAlive(time_t value);
	int				ConnKey();
	static int		ConnKey(int servertype, int serverid);
private:
	short			m_nProxyState;
	int				m_nServerId;
	enServerType	m_nServerType;
	time_t			m_nLastRecvKeepAlive;
};

#endif //__GAMEPLAYER_H__
