//
//  connector.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "network_interface.h"
#include "net_inc.h"
#include "socket.h"
#include "event_reactor.h"
#include "net_addr.h"

class CConnector {
  enum eConnectorState {
	eCS_Disconnected = 0,
	eCS_Connecting,
	eCS_Connected,
  };
public:
  CConnector(IEventReactor *pReactor);
  virtual ~CConnector(void);
  void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
  bool Connect(const CNetAddr &addr, const timeval *time = NULL);
  void SetCallbackFunc(FuncConnectorOnDisconnected pOnDisconnected,
					   FuncConnectorOnConnectFailed pOnConnectFailed,
					   FuncConnectorOnConnectted pOnConnectted,
					   FuncConnectorOnSomeDataSend pOnSomeDataSend,
					   FuncConnectorOnSomeDataRecv pOnSomeDataRecv);

  void *GetContext(void) const;
  void SetContext(void *pContext);
  PipeResult Send(const void *pData, uint32 uSize);
  void ShutDown();
  void *GetRecvData() const;
  uint32 GetRecvDataSize();
  uint32 GetSendDataSize();
  void PopRecvData(uint32 uSize);
  void SetMaxSendBufSize(uint32 uSize);
  uint32 GetMaxSendBufSize();
  void SetMaxRecvBufSize(uint32 uSize);
  uint32 GetMaxRecvBufSize();
  bool IsConnected();
  bool IsConnecting();
  bool IsDisconnected();
  void Release();

private:
  IEventReactor *GetReactor();
  bool RegisterToReactor();
  bool UnRegisterFromReactor();
  static void lcb_OnConnectResult(int Socket, short nEventMask, void *arg);
  static void lcb_OnPipeRead(struct bufferevent *bev, void *arg);
  static void lcb_OnPipeWrite(bufferevent *bev, void *arg);
  static void lcb_OnPipeError(bufferevent *bev, int16 nWhat, void *arg);
  eConnectorState GetState();
  void SetState(eConnectorState eState);
  void OnConnectted();
  void HandleInput(int32 Socket, int16 nEventMask, void *arg);
  void ProcessSocketError();

private:
  IEventReactor *m_pReactor;

  FuncConnectorOnDisconnected m_pFuncOnDisconnected;
  FuncConnectorOnConnectFailed m_pFuncOnConnectFailed;
  FuncConnectorOnConnectted m_pFuncOnConnectted;
  FuncConnectorOnSomeDataSend m_pFuncOnSomeDataSend;
  FuncConnectorOnSomeDataRecv m_pFuncOnSomeDataRecv;

  void *m_pContext;
  CNetAddr m_Addr;
  CSocket m_Socket;
  eConnectorState m_eState;
  bufferevent *m_pStBufEv;
  event m_ConnectEvent;
  uint32 m_uMaxOutBufferSize;
  uint32 m_uMaxInBufferSize;
};

#endif
