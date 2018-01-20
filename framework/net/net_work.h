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
#include "connectorex.h"

class CNetWork : public CSingleton<CNetWork> {
public:
  //构造函数
  CNetWork();
  //析构函数
  virtual ~CNetWork();
  //初始化
  void Init(eNetModule netModule);
  //开始监听
  bool BeginListen(const char *szNetAddr,
				   uint16 uPort,
				   FuncAcceptorExOnNew pOnNew,
				   FuncAcceptorExOnDisconnected pOnDisconnected,
				   FuncAcceptorExOnSomeDataSend pOnSomeDataSend,
				   FuncAcceptorExOnSomeDataRecv pOnSomeDataRecv,
				   uint32 uCheckPingTickTime = 0);
  //结束监听
  void EndListen();
  uint32 Connect(const char *szNetAddr, uint16 uPort,
				 FuncConnectorExOnDisconnected pOnDisconnected,
				 FuncConnectorExOnConnectFailed pOnConnectFailed,
				 FuncConnectorExOnConnectted pOnConnectted,
				 FuncConnectorExOnSomeDataSend pOnSomeDataSend,
				 FuncConnectorExOnSomeDataRecv pOnSomeDataRecv,
				 FuncConnectorExOnPingServer pOnPingServer,
				 uint32 uPingTick = 4500,
				 uint32 uTimeOut = 30);

  bool ShutDownAcceptorEx(uint32 uId);

  void SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);

  PipeResult ConnectorExSendData(uint32 uId, const void *pData, uint32 uSize);

  void EnumAcceptorEx(FuncEnumAcceptorExCallback pFunc, void *pContext);

  bool ShutDownConnectorEx(uint32 uId);

  void DispatchEvents();

  uint32 GetConnectorExPingValue(uint32 uId);

  CConnectorEx *FindConnectorEx(uint32 uId);
  CAcceptorEx *FindAcceptorEx(uint32 uId);

private:

  struct EnumContex {
	FuncEnumAcceptorExCallback pFunc;
	void *pContext;
  };

  void OnTick();
  const char *GetTickName() { return "NetWork Tick"; };

  static void OnAccept(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);
  void NewAcceptor(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa);

protected:
  virtual void Release();

private:
  uint32 m_uGcTime;

  uint32 m_uCheckPingTickTime;

  IEventReactor *m_pEventReactor;

  typedef std::unordered_map<unsigned int, CConnectorEx *> Map_ConnectorExs;
  typedef std::unordered_map<unsigned int, CAcceptorEx *> Map_AcceptorExs;
  typedef std::queue<CSystemSignal *> Queue_SystemSignals;

  Map_ConnectorExs m_mapConnectorExs;
  Map_AcceptorExs m_mapAcceptorExs;
  Queue_SystemSignals m_quSystemSignals;

  typedef std::queue<CConnectorEx *> Queue_IdleConnectorExs;
  Queue_IdleConnectorExs m_quIdleConnectorExs;

  CListener *m_pListener;
  FuncAcceptorExOnNew m_pOnNew;
  FuncAcceptorExOnDisconnected m_pOnDisconnected;
  FuncAcceptorExOnSomeDataSend m_pOnSomeDataSend;
  FuncAcceptorExOnSomeDataRecv m_pOnSomeDataRecv;
};

#endif
