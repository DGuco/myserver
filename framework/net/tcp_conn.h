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
	//
	virtual ~CTCPConn();
public:
	virtual int Key();
	virtual int DoRecvLogic();
	virtual int DoWriteLogic();
	virtual int DoFdErrorLogic();
private:
	int m_nType;
	int m_nId;
};
#endif //__TCP_CONN_H__
