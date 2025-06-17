/*****************************************************************
* FileName:proxy_conn.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __PROXY_CONN_H__
#define __PROXY_CONN_H__

#include "tcp_conn.h"
#include "message.pb.h"

enum enProxyState
{
	eProInvalid = -1,  //无效
	eProConnected = 0, //连接
	eProRegistered = 1,//注册
	eProKicking = 2,   //非法连接等待踢掉
};

using namespace std;

class CProxyConn : public CTCPConn
{
public:
	CProxyConn(CSocket socket);
	virtual ~CProxyConn();
	virtual int		DoRecvLogic();
	virtual int		DoWriteLogic();
	virtual void	DoTick(time_t now);
	void			SetProxyState(short state);
	void			SetServerId(int serverid);
	void			SetServerType(enServerType servertype);
	short			GetProxyState();
	int				GetServerId();
	enServerType	GetServerType();
	virtual int		ConnKey();
	virtual string	ConnInfo() 	 {return std::to_string(m_nServerType) + "_" + std::to_string(m_nServerId);};
	static int		ConnKey(int servertype, int serverid);
private:
	short			m_nProxyState;
	int				m_nServerId;
	enServerType	m_nServerType;
};

#endif //__PROXY_CONN_H__
