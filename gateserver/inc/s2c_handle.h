//
// Created by dguco on 18-1-30.
// 发送客户端消息线程
//

#ifndef SERVER_S2C_THREAD_H
#define SERVER_S2C_THREAD_H

#include <message.pb.h>
#include "codequeue.h"
#include "mythread.h"

class CS2cHandle: public CMyThread
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
	void CheckData();
	//检测发送队列
	void CheckWaitSendData();
	//向client下行数据
	int SendClientData();

private:
	//接收gameserver 数据
	int RecvServerData();
private:
	// game --> tcp通信共享内存管道
	CCodeQueue *m_pS2CPipe;
	int m_iSendIndex;
	bool m_bHasRecv;
	unsigned short m_iSCIndex;                 // 去掉nethead头的实际发送给客户端的数据在m_szSCMsgBuf中的数组下标
	short m_nSCLength;                         // 实际发送的数据长度
	char m_acSCMsgBuf[MAX_PACKAGE_LEN];        // 客户端下行发送消息缓冲区
	MesHead m_oMesHead;                        // 需要转发的消息头
};


#endif //SERVER_S2C_THREAD_H
