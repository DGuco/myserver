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

class CServerManager
{
public:
	//构造函数
	CServerManager(shared_ptr<CNetWork> pNetWork);
	//析构函数
	virtual ~CServerManager();
public:
	//准备run
	int PrepareToRun();
	//向game发送消息
	int SendToGameBuff(CMessage *pMessage);
	int FlushToGame();
	//连接game
	void ConnectToGame();
	//重新连接
	bool ReconnectToGame(CConnector *tmpConnector);
	shared_ptr<CByteBuff> &GetSendBuff() ;
	shared_ptr<CByteBuff> &GetRecvBuff() ;
	time_t GetLastSendKeepAlive() ;
	time_t GetLastRecvKeepAlive() ;
private:
	//检测发送队列
	void RecvGameData(CConnector* tmpConnector);
	//向client下行数据包
	int SendClientData( CMessage &tmpMes, char *data, int len);
	//向gameserver注册
	void Register2Game();
	//向game发送心跳
	void SendKeepAlive2Game();
private:
	//客户端上行数据回调（无用）
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//连接失败（无用）
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnConnected(CConnector *pConnector);
	static void lcb_OnPingServer(int fd, short what, CConnector *pConnector);
private:
	shared_ptr<CNetWork> m_pNetWork;
	std::shared_ptr<CByteBuff> m_pSendBuff;      //上行gameserver buff
	std::shared_ptr<CByteBuff> m_pRecvBuff;      //gameserver 返回数据buff
	time_t m_tLastSendKeepAlive;        //最后发送gameServer心跳消息时间
	time_t m_tLastRecvKeepAlive;        //最后接收gameServer心跳消息时间
};


#endif //SERVER_S2C_THREAD_H
