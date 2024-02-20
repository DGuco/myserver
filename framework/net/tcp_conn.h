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
	CTCPConn(CSocket tmSocket, int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPConn();
public:
	virtual int DoRecvLogic() = 0;
	virtual int DoWriteLogic() = 0;
	virtual int DoFdErrorLogic() = 0;
private:
	int m_nType;
	int m_nId;
};
#endif //__TCP_CONN_H__
