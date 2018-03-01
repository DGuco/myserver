//
//  net_work_interface.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_


#include <functional>
#include <event.h>
#include "base.h"
#include "net_inc.h"

class IEventReactor;
class IReactorHandler;
class CConnector;
class CNetAddr;
class CAcceptor;
class CSystemSignal;
class CListener;

enum PipeResult
{
	ePR_OK = 0,
	ePR_Disconnected = -1,
	ePR_OutPipeBuf = -2,
	ePR_BufNull = -3,
};

/** Interface for IEventReactor
 */
class IEventReactor
{
public:
	virtual ~IEventReactor() = default;
	virtual bool Register(IReactorHandler *pHandler) = 0;
	virtual bool UnRegister(IReactorHandler *pHandler) = 0;
	virtual event_base *GetEventBase()=0;
	virtual void DispatchEvents() = 0;
	virtual void Release() = 0;
};

class IReactorHandler
{
public:
	virtual ~IReactorHandler() = default;
	virtual bool RegisterToReactor() = 0;
	virtual bool UnRegisterFromReactor() = 0;
	virtual IEventReactor *GetReactor() = 0;
};

/** CallBack for TimerOut
*/
typedef std::function<void(int, short, void *)> FuncOnTimeOut;

/** CallBack for TimerOut Or
*/
typedef std::function<void(uint32, void *)> FuncOnSignal;

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

typedef std::function<void(CConnector *)> FuncConnectorOnPingServer;

/** CallBack for CAcceptor
 */
typedef std::function<void(uint32, CAcceptor *)> FuncAcceptorOnNew;

typedef std::function<void(CAcceptor *)> FuncAcceptorOnDisconnected;

typedef std::function<void(CAcceptor *)> FuncAcceptorOnSomeDataRecv;

#endif
