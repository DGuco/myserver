//
// Created by dguco on 18-1-30.
// 发送客户端消息线程
//

#ifndef SERVER_S2C_THREAD_H
#define SERVER_S2C_THREAD_H

#include <message.pb.h>
#include <net_work.h>
#include <byte_buff.h>
#include "code_queue.h"
#include "mythread.h"
#include "message.pb.h"

class CServerHandle
{
public:
	//构造函数
	CServerHandle(shared_ptr<CNetWork> pNetWork);
	//析构函数
	virtual ~CServerHandle();
public:
	//准备run
	int PrepareToRun();
	//向game发送消息
	int SendToGame(shared_ptr<CMessage> &pMessage);
private:
	//检测发送队列
	void CheckWaitSendData();
	//向client下行数据包
	int SendClientData(CMessage &tmpMes, std::shared_ptr<CByteBuff> tmpBuff);
	//向gameserver注册
	void Register2Game();
private:
	//客户端上行数据回调（无用）
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//连接失败（无用）
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnConnected(CConnector *pConnector);
	static void lcb_OnPingServer(int fd, short what, CConnector *pConnector);
public:
	static shared_ptr<CByteBuff> m_pSendBuff;
private:
	shared_ptr<CNetWork> m_pNetWork;
	time_t m_tLastSendKeepAlive;        // 最后发送gameServer心跳消息时间
	time_t m_tLastRecvKeepAlive;        // 最后接收gameServer心跳消息时间
};


#endif //SERVER_S2C_THREAD_H
