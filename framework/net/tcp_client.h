/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140929@qq.com)
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
	CTCPClient(int RecvBufLen_,int SendBufLen_, int MaxRecvBufLen_, int MaxSendBufLen_);
	//
	virtual ~CTCPClient();
public:
	virtual int		DoRecvLogic() = 0;
	virtual int		DoWriteLogic() = 0;
	virtual int		DoClosingLogic(int errcode) = 0;
	virtual void	DoTick(time_t now) = 0;
	virtual int	    ClientKey();
	uint64			MapKey();
	static uint64   MapKey(int socket, int clientKey);
public:
};
#endif //__TCP_CLIENT_H__
