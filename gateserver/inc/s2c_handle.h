//
// Created by dguco on 18-1-30.
// 发送客户端消息线程
//

#ifndef SERVER_S2C_THREAD_H
#define SERVER_S2C_THREAD_H

#include "codequeue.h"
#include "mythread.h"

class CS2cHandle: CMyThread
{
public:
	//构造函数
	CS2cHandle();
	//析构函数
	virtual ~CS2cHandle();
public:
	//准备run
	int PrepareToRun() override;
	//run
	int Run() override;
	//线程阻塞条件
	bool IsToBeBlocked() override;
	//是否有数据
	bool CheckData();
	//接收gameserver 数据
	int RecvServerData();
private:
	// game --> tcp通信共享内存管道
	CCodeQueue *m_pS2CPipe;
	char m_szSCMsgBuf[MAX_PACKAGE_LEN];                 // 下行客户端发送消息缓冲区
};


#endif //SERVER_S2C_THREAD_H
