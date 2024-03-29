/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include "tcp_socket.h"

/**
 * 发起的tcp连接
 */
class CTCPClient : public CTCPSocket
{	
public:
	//
	CTCPClient(unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPClient();
public:
	virtual int		DoRecvLogic() = 0;
	virtual int		DoWriteLogic() = 0;
	virtual int		DoErrorLogic(int errcode) = 0;
	virtual void	DoTick(time_t now) = 0;
public:
};
#endif //__TCP_CLIENT_H__
