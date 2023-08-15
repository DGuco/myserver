/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include "tcp_socket.h"

class CTCPClient : public CTCPSocket
{	
public:
	//
	CTCPClient(unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPClient();

private:
	int m_nType;
	int m_nId;
};
#endif //__TCP_CLIENT_H__
