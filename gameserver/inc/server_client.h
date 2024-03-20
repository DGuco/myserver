/*****************************************************************
* FileName:server_client.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __SERVER_CLIENT_H__
#define __SERVER_CLIENT_H__

#include "tcp_client.h"

class CServerClient : public CTCPClient
{
public:
	CServerClient();
	virtual int DoRecvLogic();
	virtual int DoWriteLogic();
	virtual int DoErrorLogic(int errcode);
private:
};

#endif //__SERVER_CLIENT_H__


