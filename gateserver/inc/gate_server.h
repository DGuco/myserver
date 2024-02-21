//
// Created by dguco on 18-1-30.
//

#ifndef __NET_MANAGER_H__
#define __NET_MANAGER_H__

#include "code_queue.h"
#include "byte_buff.h"
#include "mythread.h"

class CGateServer : public CTCPServer
{
public:
	//构造函数
	CGateServer();
	//析构函数
	virtual ~CGateServer();
public:
	//准备run
	bool PrepareToRun();
	//发送数据给gameserver
	void DealClientData(SafePointer<CGamePlayer> pGamePlayer, unsigned short len);
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
	shared_ptr<CByteBuff> &GetRecvBuff();
	shared_ptr<CByteBuff> &GetSendBuff();
	shared_ptr<CNetWork> &GetNetWork();
private:
	//清除socket
	void ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//通知gameserver client 断开连接
	void DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//接受客户端数据
	void RecvClientData(SafePointer<CGamePlayer> pGamePlayer);
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
    //检测是否又数据要发送
    static void lcb_OnCheckSendMsg(int fd, short what, void *param);

	virtual void OnNewConnect(SafePointer<CTCPConn> pConnn);
	//
	virtual SafePointer<CTCPConn> CreateTcpConn(CSocket tmSocket);
	//
	virtual SafePointer<CTCPClient> CreateTcpClient(CSocket tmSocket);
private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
private:
	shared_ptr<CNetWork> m_pNetWork;
	shared_ptr<CByteBuff> m_pRecvBuff; //客户端上行数据buff
	shared_ptr<CByteBuff> m_pSendBuff; //客户端下行数据buff
    shared_ptr<CTimerEvent> m_pSendMsgTimer;
};
#endif //__NET_MANAGER_H__
