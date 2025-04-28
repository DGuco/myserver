/*****************************************************************
* FileName:db_client.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __DB_CLIENT_H__
#define __DB_CLIENT_H__

#include "tcp_client.h"
#include "time_helper.h"

class CDBClient : public CTCPClient
{
public:
	CDBClient();
	virtual ~CDBClient();
	void SetLastRecvKeepLive(time_t nKeepAliveTime);
	time_t GetLastRecvKeepLive();
	void SetLastSendKeepLive(time_t nKeepAliveTime);
	time_t GetLastSendKeepLive();
public:
	virtual int		DoRecvLogic();
	virtual int		DoWriteLogic();
	virtual int		DoClosingLogic(int errcode);
	virtual void	DoTick(time_t nNow);
private:
	CMyTimer m_SendKeepAliveTimer;	//发送心跳包定时器
	time_t	 m_nLastSendKeepLive; //
	time_t	 m_nLastRecvKeepLiveAnswer; //
};

#endif //__SERVER_CLIENT_H__


