//
// Created by dguco on 18-1-30.
// 接收客户端信息线程
//

#ifndef SERVER_C2S_THREAD_H
#define SERVER_C2S_THREAD_H

#include <acceptor.h>
#include <code_queue.h>
#include <net_work.h>
#include <byte_buff.h>
#include "mythread.h"

class CNetManager
{
public:
	//构造函数
	CNetManager(shared_ptr<CNetWork> pNetWork);
	//析构函数
	virtual ~CNetManager();
public:
	//准备run
	int PrepareToRun();
	//发送数据给gameserver
	void DealClientData(CAcceptor *tmpAcceptor, unsigned short len);
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
	shared_ptr<CByteBuff> &GetRecvBuff();
	shared_ptr<CByteBuff> &GetSendBuff();
	shared_ptr<CNetWork> &GetNetWork();
private:
	//清除socket
	void ClearSocket(CAcceptor *tmpAcceptor, short iError);
	//通知gameserver client 断开连接
	void DisConnect(CAcceptor *tmpAcceptor, short iError);
	//接受客户端数据
	void RecvClientData(CAcceptor *tmpAcceptor);
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, IBufferEvent *tmpAcceptor);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *tmpAcceptor);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *tmpAcceptor);
	//发送数据回调
	static void lcb_OnCnsSomeDataSend(IBufferEvent *tmpAcceptor);
	//检测连接超时
	static void lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param);
private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
private:
	shared_ptr<CNetWork> m_pNetWork;
	shared_ptr<CByteBuff> m_pRecvBuff; //客户端上行数据buff
	shared_ptr<CByteBuff> m_pSendBuff; //客户端下行数据buff
};
#endif //SERVER_C2S_THREAD_H
