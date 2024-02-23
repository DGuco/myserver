//
// Created by dguco on 18-1-30.
// 发送客户端消息线程
//

#ifndef SERVER_S2C_THREAD_H
#define SERVER_S2C_THREAD_H

#include "byte_buff.h"
#include "code_queue.h"
#include "message.pb.h"
#include "server_tool.h"

class CMessHandle : public CSingleton<CMessHandle>
{
public:
	//构造函数
	CMessHandle();
	//析构函数
	virtual ~CMessHandle();
public:
	//准备run
	bool PrepareToRun();
	//创建pipe
	bool CreatePipe();
	//向client下行数据包
	int SendClientData(CMessage &tmpMes, char *data, int len);
	//接收game消息
	void RecvGameData();
	//向game 发送消息
	int SendToGame(char *data, int iTmpLen);
	//发送消息给client
	void DealMsg();
private:
	//gateserver ==> gameserver
	SafePointer<CCodeQueue> m_C2SCodeQueue;
	//gameserver ==> gateserver
	SafePointer<CCodeQueue> m_S2CCodeQueue;
	SafePointer<CByteBuff> m_pRecvBuff;      //gameserver 返回数据buff
};


#endif //SERVER_S2C_THREAD_H
