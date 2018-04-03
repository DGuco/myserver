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

class CC2sHandle: public CMyThread
{
public:
	//构造函数
	CC2sHandle();
	//析构函数
	virtual ~CC2sHandle();
	//获取CNetWork
	CNetWork *GetNetWork();
public:
	//准备run
	int PrepareToRun() override;
	int RunFunc() override;
	bool IsToBeBlocked() override;
public:
	//清除socket
	static void ClearSocket(IBufferEvent *pAcceptor, short iError);
	//通知gameserver client 断开连接
	static void DisConnect(IBufferEvent *pAcceptor, short iError);
	//发送数据给gameserver
	static void SendToGame(IBufferEvent *pAcceptor, unsigned short len);

private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, IBufferEvent *pBufferEvent);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	//发送胡据回调
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
public:
	CNetWork *m_pNetWork;
	static CCodeQueue *m_pC2SPipe;
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
	static char m_acSendBuff[MAX_PACKAGE_LEN];
};
#endif //SERVER_C2S_THREAD_H
