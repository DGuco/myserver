/*****************************************************************
* FileName:tcp_conn.h
* Summary :
* Date	  :2023-8-15
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__
#include "tcp_socket.h"

class CTCPConn : public CTCPSocket
{
public:
	//
	CTCPConn(unsigned int RecvBufLen_, unsigned int SendBufLen_);
	CTCPConn(CSocket socket,unsigned int RecvBufLen_, unsigned int SendBufLen_);
	virtual ~CTCPConn();
	void SetLastRecvKeepLive(time_t nKeepAliveTime);
	time_t GetLastRecvKeepLive();
	void SetCreateTime(time_t nCreateTime);
	time_t GetCreateTime();
public:
	virtual int DoRecvLogic() = 0;
	virtual int DoWriteLogic() = 0;
	virtual int DoErrorLogic(int errcode) = 0;
protected:
	time_t m_nLastRecvKeepLive;
	time_t m_nCreateTime;
};
#endif //__TCP_CONN_H__
