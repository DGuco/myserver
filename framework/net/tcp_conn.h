/*****************************************************************
* FileName:tcp_conn.h
* Summary :
* Date	  :2023-8-15
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__
#include "base.h"
#include "tcp_socket.h"
#include <string>

using namespace std;

/**
 * 接收的tcp连接
 */
class CTCPConn : public CTCPSocket
{
public:
	CTCPConn(CSocket socket,
			int RecvBufLen_,
			int SendBufLen_,
			int MaxRecvBufLen_,
			int MaxSendBufLen_);
	virtual ~CTCPConn();
public:
	virtual int			DoRecvLogic() = 0;
	virtual int			DoWriteLogic() = 0;
	virtual void		DoTick(time_t now) = 0;
	virtual int     	ConnKey()  {return 0;}
	virtual string		ConnInfo() 	 {return "Null";};
public:
	//int 	 SendHeartbeatCallBack();
	time_t   GetLastRecvHeartbeatTime() { return m_nLastRecvHeartbeatTime; }
	void  	 SetLastRecvHeartbeatTime(time_t time) { m_nLastRecvHeartbeatTime = time; }
private:
    time_t m_nLastRecvHeartbeatTime;
};
#endif //__TCP_CONN_H__
