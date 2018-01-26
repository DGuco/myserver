//
//  net_work_interface.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include "base.h"
#include "net_inc.h"
#include <functional>
#include <event.h>

class IEventReactor;
class IReactorHandler;
class CConnector;
class CNetAddr;
class CAcceptor;
class CSystemSignal;
class CListener;
class CConnectorEx;
class CAcceptorEx;

enum PipeResult {
  ePR_OK = 0,
  ePR_Disconnected,
  ePR_OutPipeBuf,
};

/** Interface for IEventReactor
 */
INTER_FACE IEventReactor {
  virtual bool Register(IReactorHandler *pHandler) = 0;
  virtual bool UnRegister(IReactorHandler *pHandler) = 0;
  virtual event_base *GetEventBase()=0;
  virtual void Init() = 0;
  virtual void DispatchEvents() = 0;
  virtual void Release() = 0;
};

INTER_FACE IReactorHandler {
  virtual bool RegisterToReactor() = 0;
  virtual bool UnRegisterFromReactor() = 0;
  virtual IEventReactor *GetReactor() = 0;
};

//CAcceptorEx相关的函数回调
typedef std::function<void(CAcceptorEx *, void *)> FuncEnumAcceptorExCallback;
typedef std::function<void(uint32, CAcceptorEx *)> FuncAcceptorExOnNew;
typedef std::function<void(CAcceptorEx *)> FuncAcceptorExOnDisconnected;
typedef std::function<void(CAcceptorEx *)> FuncAcceptorExOnSomeDataSend;
typedef std::function<void(CAcceptorEx *)> FuncAcceptorExOnSomeDataRecv;
typedef std::function<void(CAcceptorEx *)> FuncAcceptorExOnClientLost;

//CConnectorEx相关的函数回调
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnDisconnected;
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnConnectFailed;
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnConnectted;
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnSomeDataSend;
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnSomeDataRecv;
typedef std::function<void(CConnectorEx *)> FuncConnectorExOnPingServer;

/** CallBack for Accept
*/
typedef std::function<void(IEventReactor *, SOCKET, sockaddr *sa)> FuncListenerOnAccept;

/** CallBack for Connector
 */
typedef std::function<void(CConnector *)> FuncConnectorOnDisconnected;
typedef std::function<void(CConnector *)> FuncConnectorOnConnectFailed;
typedef std::function<void(CConnector *)> FuncConnectorOnConnectted;
typedef std::function<void(CConnector *)> FuncConnectorOnSomeDataSend;
typedef std::function<void(CConnector *)> FuncConnectorOnSomeDataRecv;

/** CallBack for CAcceptor
 */
typedef std::function<void(CAcceptor *)> FuncAcceptorOnDisconnected;
typedef std::function<void(CAcceptor *)> FuncAcceptorOnSomeDataSend;
typedef std::function<void(CAcceptor *)> FuncAcceptorOnSomeDataRecv;

/** CallBack for Signal
*/
typedef std::function<void(uint32, void *)> FuncOnSignal;

#endif
