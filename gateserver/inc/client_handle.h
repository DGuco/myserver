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

class CClientHandle
{
public:
	//构造函数
	CClientHandle(shared_ptr<CNetWork> pNetWork);
	//析构函数
	virtual ~CClientHandle();
	//获取CNetWork
	CNetWork *GetNetWork();
public:
	//准备run
	int PrepareToRun();
	//发送数据给gameserver
	void DealClientData(CAcceptor* tmpAcceptor, unsigned short len);
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
public:
	//清除socket
	static void ClearSocket(CAcceptor* tmpAcceptor, short iError);
	//通知gameserver client 断开连接
	static void DisConnect(CAcceptor* tmpAcceptor, short iError);
	static CByteBuff *GetRecvBuff();
	static CByteBuff *GetSendBuff();
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, CAcceptor* tmpAcceptor);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(CAcceptor* tmpAcceptor);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(CAcceptor* tmpAcceptor);
	//发送数据回调
	static void lcb_OnCnsSomeDataSend(CAcceptor* tmpAcceptor);
	//检测连接超时
	static void lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param);
private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
private:
	shared_ptr<CNetWork> m_pNetWork;
	static CByteBuff *m_pRecvBuff; //客户端上行数据buff
	static CByteBuff *m_pSendBuff; //客户端下行数据buff
};
#endif //SERVER_C2S_THREAD_H
