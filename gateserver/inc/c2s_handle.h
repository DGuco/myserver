//
// Created by dguco on 18-1-30.
// 接收客户端信息线程
//

#ifndef SERVER_C2S_THREAD_H
#define SERVER_C2S_THREAD_H

#include <acceptor.h>
#include <code_queue.h>
#include <net_work.h>
#include "mythread.h"

class CC2sHandle
{
public:
	//构造函数
	CC2sHandle(eNetModule netModule);
	//析构函数
	virtual ~CC2sHandle();
	//准备run
	int PrepareToRun();
	//run
	int Run();
	//获取CNetWork
	CNetWork *GetNetWork();
public:
	//清除socket
	static void ClearSocket(CAcceptor *pAcceptor, short iError);
	//通知gameserver client 断开连接
	static void DisConnect(CAcceptor *pAcceptor, short iError);
	//发送数据给gameserver
	static void SendToGame(CAcceptor *pAcceptor, PACK_LEN len);
private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
protected:
	//客户端连接还回调
	static void OnAcceptCns(uint32 uId, CAcceptor *pAcceptor);
	//客户端断开连接回调
	static void OnCnsDisconnected(CAcceptor *pAcceptor);
	//客户端上行数据回调
	static void OnCnsSomeDataRecv(CAcceptor *pAcceptor);
public:
	CNetWork *m_pNetWork;
	static CCodeQueue *m_pC2SPipe;
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
	static char m_acSendBuff[MAX_PACKAGE_LEN];
};
#endif //SERVER_C2S_THREAD_H
