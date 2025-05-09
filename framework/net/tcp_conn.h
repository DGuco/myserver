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
	virtual int		DoRecvLogic() = 0;
	virtual int		DoWriteLogic() = 0;
	virtual int		DoClosingLogic(int errcode) = 0;
	virtual void	DoTick(time_t now) = 0;
	int   GetLastRecvHeartbeatTime() { return m_nLastRecvHeartbeatTime; }
	void  SetLastRecvHeartbeatTime(int time) { m_nLastRecvHeartbeatTime = time; }
private:
    int m_nLastRecvHeartbeatTime;
};
#endif //__TCP_CONN_H__
