//
// Created by dguco on 18-1-30.
// 接收客户端信息线程
//

#ifndef SERVER_C2S_THREAD_H
#define SERVER_C2S_THREAD_H

#include <acceptor.h>
#include <codequeue.h>
#include <net_work.h>
#include "mythread.h"

class CC2sHandle: public CMyThread
{
public:
	//构造函数
	CC2sHandle();
	//析构函数
	virtual ~CC2sHandle();
	//准备run
	int PrepareToRun() override;
	//run
	int Run() override;
	//线程阻塞条件
	bool IsToBeBlocked() override;
private:
	//开始监听
	bool BeginListen();
protected:
	//客户端连接还回调
	static void OnAcceptCns(uint32 uId, CAcceptor *pAcceptorEx);
	//客户端断开连接回调
	static void OnCnsDisconnected(CAcceptor *pAcceptorEx);
	//发送数据回调
	static void OnCnsSomeDataSend(CAcceptor *pAcceptorEx);
	//客户端上行数据回调
	static void OnCnsSomeDataRecv(CAcceptor *pAcceptorEx);
private:
	CCodeQueue *m_pC2SPipe;
	CNetWork *m_pNetWork;
};
#endif //SERVER_C2S_THREAD_H
