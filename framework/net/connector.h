//
//  connector.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "network_interface.h"
#include "socket.h"
#include "event_reactor.h"
#include "net_addr.h"
#include "buffev_interface.h"
#include "timer_event.h"

class CConnector: public IBufferEvent
{
	enum eConnectorState
	{
		eCS_Disconnected = 0,
		eCS_Connecting,
		eCS_Connected,
	};
public:
	//构造函数
	CConnector(IEventReactor *pReactor);
	//析构函数
	virtual ~CConnector(void);
	//获取连接ip
	void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
	//连接
	bool Connect(const CNetAddr &addr, const timeval time);
	//设置相关回调
	void SetCallbackFunc(FuncConnectorOnDisconnected pOnDisconnected,
						 FuncConnectorOnConnectFailed pOnConnectFailed,
						 FuncConnectorOnConnectted pOnConnectted,
						 FuncConnectorOnSomeDataSend pOnSomeDataSend,
						 FuncConnectorOnSomeDataRecv pOnSomeDataRecv);
	//关闭连接
	void ShutDown();
	//是否连接成功
	bool IsConnected();
	//是否正在连接
	bool IsConnecting();
	//是否断开连接
	bool IsDisconnected();

private:
	//bufferEvent 无效处理
	void BuffEventUnavailableCall() override;
	//event buffer 创建成功后处理
	void AfterBuffEventCreated() override;

private:
	//连接回调
	static void lcb_OnConnectResult(int Socket, short nEventMask, void *arg);
	//读回调
	static void lcb_OnPipeRead(struct bufferevent *bev, void *arg);
	//写回调
	static void lcb_OnPipeWrite(bufferevent *bev, void *arg);
	//错误回调
	static void lcb_OnPipeError(bufferevent *bev, int16 nWhat, void *arg);
	//当前连接状态
	eConnectorState GetState();
	//设置当前抓状态
	void SetState(eConnectorState eState);
	//连接成功
	void OnConnectted();
	//处理
	void HandleInput(int32 socket, int16 nEventMask, void *arg);
	//socket error
	void ProcessSocketError();

private:
	CNetAddr m_oAddr;
	eConnectorState m_eState;
	event m_oConnectEvent;
	FuncConnectorOnDisconnected m_pFuncOnDisconnected;
	FuncConnectorOnConnectFailed m_pFuncOnConnectFailed;
	FuncConnectorOnConnectted m_pFuncOnConnectted;
	FuncConnectorOnSomeDataSend m_pFuncOnSomeDataSend;
	FuncConnectorOnSomeDataRecv m_pFuncOnSomeDataRecv;
};

#endif
