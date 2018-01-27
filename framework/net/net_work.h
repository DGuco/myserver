//
//  net_work.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _NET_WORK_H_
#define _NET_WORK_H_

#include <map>
#include <queue>
#include <bits/unordered_map.h>
#include <servertool.h>
#include "network_interface.h"
#include "event_reactor.h"

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
                   FuncAcceptorOnNew pOnNew,
                   FuncAcceptorOnDisconnected pOnDisconnected,
                   FuncAcceptorOnSomeDataSend pOnSomeDataSend,
                   FuncAcceptorOnSomeDataRecv pOnSomeDataRecv,
                   uint32 uCheckPingTickTime = 0);
  //结束监听
  void EndListen();
  uint32 Connect(const char *szNetAddr,
                 uint16 uPort,
                 FuncConnectorOnDisconnected pOnDisconnected,
                 FuncConnectorOnConnectFailed pOnConnectFailed,
                 FuncConnectorOnConnectted pOnConnectted,
                 FuncConnectorOnSomeDataSend pOnSomeDataSend,
                 FuncConnectorOnSomeDataRecv pOnSomeDataRecv,
                 FuncConnectorOnPingServer pOnPingServer,
                 uint32 uPingTick = 4500,
                 uint32 uTimeOut = 30);

  bool ShutDownAcceptorEx(uint32 uId);
  void SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);
  PipeResult ConnectorExSendData(uint32 uId, const void *pData, uint32 uSize);
  bool ShutDownConnectorEx(uint32 uId);
  void DispatchEvents();
  uint32 GetConnectorExPingValue(uint32 uId);
  CConnectorEx *FindConnectorEx(uint32 uId);
  CAcceptorEx *FindAcceptorEx(uint32 uId);

private:
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

  typedef std::unordered_map<unsigned int, CConnector *> Map_Connector;
  typedef std::unordered_map<unsigned int, CAcceptor *> Map_Acceptor;
  typedef std::queue<CSystemSignal *> Queue_SystemSignals;

  Map_Connector m_mapConnector;
  Map_Acceptor m_mapAcceptor;
  Queue_SystemSignals m_quSystemSignals;

  typedef std::queue<CConnectorEx *> Queue_IdleConnectorExs;
  Queue_IdleConnectorExs m_quIdleConnectorExs;

  CListener *m_pListener;
  FuncAcceptorOnNew m_pOnNew;
  FuncAcceptorOnDisconnected m_pOnDisconnected;
  FuncAcceptorOnSomeDataSend m_pOnSomeDataSend;
  FuncAcceptorOnSomeDataRecv m_pOnSomeDataRecv;
};

#endif
