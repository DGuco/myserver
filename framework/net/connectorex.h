//
//  connectorex.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _CONNECTOR_EX_H_
#define _CONNECTOR_EX_H_

#include "network_interface.h"
#include "net_work.h"

class CConnectorEx {
  friend class CNetWork;
public:
  CConnectorEx(uint32 uId, CConnector *pConnector);
  virtual ~CConnectorEx(void);
  uint32 GetId() const;
  void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
  void SetCallbackFunc(FuncConnectorExOnDisconnected pOnDisconnected,
					   FuncConnectorExOnConnectFailed pOnConnectFailed,
					   FuncConnectorExOnConnectted pOnConnectted,
					   FuncConnectorExOnSomeDataSend pOnSomeDataSend,
					   FuncConnectorExOnSomeDataRecv pOnSomeDataRecv,
					   FuncConnectorExOnPingServer pOnPingServer,
					   uint32 uPingTick);

  PipeResult Send(const void *pData, uint32 uSize);
  void *GetRecvData() const;
  uint32 GetRecvDataSize();
  uint32 GetSendDataSize();
  void PopRecvData(uint32 uSize);
  void SetMaxSendBufSize(uint32 uSize);
  uint32 GetMaxSendBufSize();
  void SetMaxRecvBufSize(uint32 uSize);
  uint32 GetMaxRecvBufSize();
  bool IsConnected();
  uint32 GetPingValue() const;
  void OnPingBack();
  void OnTick();
  const char *GetTickName() { return "CConnectorEx Tick"; };

protected:
  void ShutDown();
  void Release();

public:
  static void ConnectorFuncOnDisconnected(CConnectorEx *pConnector);
  static void ConnectorFuncOnConnectFailed(CConnectorEx *pConnector);
  static void ConnectorFuncOnConnectted(CConnectorEx *pConnector);
  static void ConnectorFuncOnSomeDataSend(CConnectorEx *pConnector);
  static void ConnectorFuncOnSomeDataRecv(CConnectorEx *pConnector);

private:

  uint32 m_uId;
  CConnector *m_pConnector;
  uint32 m_uPingTick;
  uint32 m_uLastPingTime;
  uint32 m_uPingValue;

  FuncConnectorExOnDisconnected m_pFuncOnDisconnected;
  FuncConnectorExOnConnectFailed m_pOnConnectFailed;
  FuncConnectorExOnConnectted m_pOnConnectted;
  FuncConnectorExOnSomeDataSend m_pOnSomeDataSend;
  FuncConnectorExOnSomeDataRecv m_pOnSomeDataRecv;
  FuncConnectorExOnPingServer m_pOnPingServer;
};

#endif
