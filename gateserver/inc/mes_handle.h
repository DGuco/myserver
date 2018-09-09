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

class CMessHandle final: CMyThread
{
public:
	//构造函数
	CMessHandle(const string &threadName, long timeOut);
	//析构函数
	virtual ~CMessHandle();
public:
	//准备run
	int PrepareToRun();
	//创建pipe
	void CreatePipe();
	//向client下行数据包
	int SendClientData(CMessage &tmpMes, char *data, int len);
	//接收game消息
	void RecvGameData();
	//向game 发送消息
	int SendToGame(char *data, int iTmpLen);
private:
	void RunFunc() override;
	bool IsToBeBlocked() override;
private:
	//gateserver ==> gameserver
	std::shared_ptr<CCodeQueue> m_C2SCodeQueue;
	//gameserver ==> gateserver
	std::shared_ptr<CCodeQueue> m_S2CCodeQueue;
	std::shared_ptr<CByteBuff> m_pRecvBuff;      //gameserver 返回数据buff
};


#endif //SERVER_S2C_THREAD_H
