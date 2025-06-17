/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include "tcp_socket.h"
#include <string>


using namespace std;

/**
 * 发起的tcp连接
 */
class CTCPClient : public CTCPSocket
{	
public:
	//
	CTCPClient(int RecvBufLen_,int SendBufLen_, int MaxRecvBufLen_, int MaxSendBufLen_,int sendHeartbeatCD_ = KEEP_ALIVE_TIME);
	//
	virtual ~CTCPClient();
public:
	virtual int				DoRecvLogic() = 0;
	virtual int				DoWriteLogic() = 0;
	virtual void			DoTick(time_t now) = 0;
	virtual std::string	    ClientInfo() {return "Null";};
public:
	time_t  	 	GetLastSendHeartbeatTime() { return m_nLastSendHeartbeatTime; }
	void  			SetLastSendHeartbeatTime(time_t time) { m_nLastSendHeartbeatTime = time; }
	time_t  	 	GetLastRecvHeartbeatTime() { return m_nLastRecvHeartbeatTime; }
	void  			SetLastRecvHeartbeatTime(time_t time) { m_nLastRecvHeartbeatTime = time; }
private:
	int 			m_nSendHeartbeatCD;
	time_t 			m_nLastSendHeartbeatTime;  //上次发送心跳的时间
	time_t 			m_nLastRecvHeartbeatTime;  //上次收到心跳恢复的时间
};
#endif //__TCP_CLIENT_H__
