//
//  acceptorex.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _ACCEPTOR_EX_H_
#define _ACCEPTOR_EX_H_

#include "network_interface.h"
#include "net_work.h"
#include "acceptor.h"

class CAcceptorEx {
public:
  CAcceptorEx(SOCKET uId, CAcceptor *pAcceptor);
  virtual ~CAcceptorEx(void);
  uint32 GetId() const;
  void SetCallbackFunc(FuncAcceptorExOnDisconnected pOnDisconnected,
					   FuncAcceptorExOnSomeDataSend pOnSomeDataSend,
					   FuncAcceptorExOnSomeDataRecv pOnSomeDataRecv,
					   uint32 uCheckClientTick);
  void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
  PipeResult Send(const void *pData, uint32 uSize);
  PipeResult Send(const void *pData1, uint32 uSize1, const void *pData2, uint32 uSize2);
  PipeResult Send(const void *pData1,
				  uint32 uSize1,
				  const void *pData2,
				  uint32 uSize2,
				  const void *pData3,
				  uint32 uSize3);
  void *GetRecvData() const;
  uint32 GetRecvDataSize();
  uint32 GetSendDataSize();
  void PopRecvData(uint32 uSize);
  void SetMaxSendBufSize(uint32 uSize);
  uint32 GetMaxSendBufSize();
  void SetMaxRecvBufSize(uint32 uSize);
  uint32 GetMaxRecvBufSize();
  bool IsConnected();
  void SetClientLastPingTime();
  void OnTick();
  const char *GetTickName() { return "CAcceptorEx Tick"; };
  void SetDecodeKey(uint8 uKey);
  uint8 GetDecodeKey();
  void SetEncodeKey(uint8 uKey);
  uint8 GetEncodeKey();

public:
  void ShutDown();
  void Release();
private:
  static void CbOnDisconnected(CAcceptorEx *pAcceptor);
  static void CbOnSomeDataSend(CAcceptorEx *pAcceptor);
  static void CbOnSomeDataRecv(CAcceptorEx *pAcceptor);

private:
  SOCKET m_uId;
  CAcceptor *m_pAcceptor;
  time_t m_uClientLastPingTime;
  time_t m_uCheckClientTick;
  uint8 m_uDecodeKey;
  uint8 m_uEncodeKey;

  FuncAcceptorExOnDisconnected m_pFuncOnDisconnected;
  FuncAcceptorExOnSomeDataSend m_pFuncOnSomeDataSend;
  FuncAcceptorExOnSomeDataRecv m_pFuncOnSomeDataRecv;
};

#endif