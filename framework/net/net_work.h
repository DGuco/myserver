//
//  net_work.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _NET_WORK_H_
#define _NET_WORK_H_

#include <map>
#include <queue>
#include <unordered_map>
#include <servertool.h>
#include "network_interface.h"
#include "event_reactor.h"
#include "mythread.h"
#include "listener.h"

using namespace std;
class CNetWork: CSingleton<CNetWork>
{
public:
	//构造函数
	CNetWork(eNetModule netModule);
	//析构函数
	virtual ~CNetWork();
	//开始监听
	bool BeginListen(const char *szNetAddr,
					 uint16 uPort,
					 FuncAcceptorOnNew pOnNew,
					 FuncAcceptorOnDisconnected pOnDisconnected,
					 FuncAcceptorOnSomeDataSend pOnSomeDataSend,
					 FuncAcceptorOnSomeDataRecv pOnSomeDataRecv,
					 uint32 uCheckPingTickTime = 0);
	//结束监听
	void EndListen();
	//连接
	int Connect(const char *szNetAddr,
				uint16 uPort,
				FuncConnectorOnDisconnected pOnDisconnected,
				FuncConnectorOnConnectFailed pOnConnectFailed,
				FuncConnectorOnConnectted pOnConnectted,
				FuncConnectorOnSomeDataSend pOnSomeDataSend,
				FuncConnectorOnSomeDataRecv pOnSomeDataRecv,
				FuncConnectorOnPingServer pOnPingServer,
				uint32 uPingTick = 4500,
				uint32 uTimeOut = 30);
	//关闭acceptor
	bool ShutDownAcceptor(uint32 uId);
	//设置信号回调
	void SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);
	//发送数据
	PipeResult ConnectorSendData(uint32 uId, const void *pData, uint32 uSize);
	//关闭connector
	bool ShutDownConnectorEx(uint32 uId);
	//启动
	void DispatchEvents();
	//获取ping
	uint32 GetConnectorExPingValue(uint32 uId);
	//查找connector
	CConnector *FindConnector(uint32 uId);
	//查找acceptor
	CAcceptor *FindAcceptor(uint32 uId);
private:
	void OnTick();
	//新的连接 accept回调
	static void OnAccept(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);
	//创建acceptor
	void NewAcceptor(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);
private:
	IEventReactor *m_pEventReactor;
	uint32 m_uGcTime;
	uint32 m_uCheckPingTickTime;

	typedef unordered_map<int, CConnector *> Map_Connector;
	typedef unordered_map<int, CAcceptor *> Map_Acceptor;
	typedef std::queue<CSystemSignal *> Queue_SystemSignals;

	Map_Connector m_mapConnector;
	Map_Acceptor m_mapAcceptor;
	Queue_SystemSignals m_quSystemSignals;

	typedef std::queue<CConnector *> Queue_IdleConnectorExs;
	Queue_IdleConnectorExs m_quIdleConnectorExs;

	CListener *m_pListener;
	FuncAcceptorOnNew m_pOnNew;
	FuncAcceptorOnDisconnected m_pOnDisconnected;
	FuncAcceptorOnSomeDataSend m_pOnSomeDataSend;
	FuncAcceptorOnSomeDataRecv m_pOnSomeDataRecv;
};

#endif
