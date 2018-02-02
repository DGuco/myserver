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
class CNetWork: public CSingleton<CNetWork>
{
public:
	//构造函数
	CNetWork(eNetModule netModule);
	//析构函数
	virtual ~CNetWork();
	//开始监听
	bool BeginListen(const char *szNetAddr,
					 unsigned int uPort,
					 FuncAcceptorOnNew pOnNew,
					 FuncAcceptorOnDisconnected pOnDisconnected,
					 FuncAcceptorOnSomeDataRecv pOnSomeDataRecv,
					 int listenQueue = -1,
					 unsigned int uCheckPingTickTime = 0);
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
				unsigned int uPingTick = 4500,
				unsigned int uTimeOut = 30);
	//关闭acceptor
	bool ShutDownAcceptor(unsigned int uId);
	//设置信号回调
	void SetCallBackSignal(unsigned int uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);
	//发送数据
	int ConnectorSendData(unsigned int uId, const void *pData, unsigned int uSize);
	//关闭connector
	bool ShutDownConnectorEx(unsigned int uId);
	//启动
	void DispatchEvents();
	//获取ping
	unsigned int GetConnectorExPingValue(unsigned int uId);
	//查找connector
	CConnector *FindConnector(unsigned int uId);
	//查找acceptor
	CAcceptor *FindAcceptor(unsigned int uId);
	//添加新的acceptor
	void InsertNewAcceptor(unsigned int socket, CAcceptor *pCAcceptor);
	//获取event
	IEventReactor *GetEventReactor();
private:
	void OnTick();
	//新的连接 accept回调
	static void OnAccept(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);
	//创建acceptor
	void NewAcceptor(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);
private:
	IEventReactor *m_pEventReactor;
	unsigned int m_uGcTime;
	unsigned int m_uCheckPingTickTime;

	typedef unordered_map<unsigned int, CConnector *> Map_Connector;
	typedef unordered_map<unsigned int, CAcceptor *> Map_Acceptor;
	typedef std::queue<CSystemSignal *> Queue_SystemSignals;

	Map_Connector m_mapConnector;
	Map_Acceptor m_mapAcceptor;
	Queue_SystemSignals m_quSystemSignals;

	typedef std::queue<CConnector *> Queue_IdleConnectorExs;
	Queue_IdleConnectorExs m_quIdleConnectorExs;

	CListener *m_pListener;
	FuncAcceptorOnNew m_pOnNew;
	FuncAcceptorOnDisconnected m_pOnDisconnected;
	FuncAcceptorOnSomeDataRecv m_pOnSomeDataRecv;
};

#endif
