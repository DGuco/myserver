//
//  acceptor.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "base.h"
#include "network_interface.h"
#include "net_inc.h"
#include "socket.h"
#include "event_reactor.h"
#include "event.h"
#include "net_addr.h"
#include "acceptorex.h"

class CAcceptor : public IReactorHandler {
  enum eAcceptorState {
	eAS_Disconnected = 0,
	eAS_Connected,
  };

public:
  CAcceptor(SOCKET socket,
			IEventReactor *pReactor,
			CNetAddr *netAddr);
  virtual ~CAcceptor();

  void SetCallbackFunc(FuncAcceptorOnDisconnected pOnDisconnected,
					   FuncAcceptorOnSomeDataSend pOnSomeDataSend,
					   FuncAcceptorOnSomeDataRecv pOnSomeDataRecv);

  void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
  PipeResult Send(const void *pData, uint32 uSize);
  PipeResult Send(const void *pData1, uint32 uSize1, const void *pData2, uint32 uSize2);
  PipeResult Send(const void *pData1,
				  uint32 uSize1,
				  const void *pData2,
				  uint32 uSize2,
				  const void *pData3,
				  uint32 uSize3);
  CAcceptorEx *GetContext(void) const;
  void SetContext(CAcceptorEx *pContext);
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
  void Release();
  CSocket GetSocket() const;

private:
  IEventReactor *GetReactor();
  bool RegisterToReactor();
  bool UnRegisterFromReactor();
  static void lcb_OnPipeRead(struct bufferevent *bev, void *arg);
  static void lcb_OnPipeWrite(bufferevent *bev, void *arg);
  static void lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg);

  eAcceptorState GetState();

  void SetState(eAcceptorState eState);
  void ProcessSocketError();

private:
  CAcceptorEx *m_pContext;
  FuncAcceptorOnDisconnected m_pFuncOnDisconnected;
  FuncAcceptorOnSomeDataSend m_pFuncOnSomeDataSend;
  FuncAcceptorOnSomeDataRecv m_pFuncOnSomeDataRecv;

  IEventReactor *m_pReactor;
  CSocket m_Socket;
  CNetAddr *m_pNetAddr;
  eAcceptorState m_eState;
  bufferevent *m_pStBufEv;
  uint32 m_uMaxOutBufferSize;
  uint32 m_uMaxInBufferSize;
};

#endif