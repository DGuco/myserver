//
//  CListener.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LISTENER_H
#define _LISTENER_H

#include "event_reactor.h"
#include "net_addr.h"
#include "socket.h"
#include "network_interface.h"
#include <event.h>
#include <event2/listener.h>

enum eListenerState {
  eLS_UnListen = 0,
  eLS_Listened,
};

class CListener : public IReactorHandler {
public:
  CListener(IEventReactor *pReactor);
  virtual ~CListener(void);

  bool Listen(CNetAddr &addr, FuncListenerOnAccept pFunc);
  IEventReactor *GetReactor();

  void ShutDown();
  void Release();

  bool IsListened();

private:

  void SetState(eListenerState eState);

  bool RegisterToReactor();
  bool UnRegisterFromReactor();

  static void lcb_Accept(struct evconnlistener *listener,
						 evutil_socket_t fd,
						 struct sockaddr *sa,
						 int socklen, void *arg);

  void HandleInput(int Socket,struct sockaddr *sa);

private:
  FuncListenerOnAccept m_pFuncOnAccept;
  eListenerState m_eState;
  CNetAddr m_ListenAddress;
  IEventReactor *m_pEventReactor;
  struct evconnlistener *m_pListener;
  CSocket m_Socket;
  event m_event;
};
#endif
