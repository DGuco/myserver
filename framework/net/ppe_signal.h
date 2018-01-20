//
//  net_inc.h
//  Created by DGuco on 18-1-7.
//
//

#ifndef _PPE_SIGNAL_H_
#define _PPE_SIGNAL_H_

#include "network_interface.h"
#include "event.h"

class CSystemSignal : public IReactorHandler {
public:
  CSystemSignal(IEventReactor *pReactor);

  ~CSystemSignal();

  void SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);

  void Release();

private:
  bool RegisterToReactor();

  bool UnRegisterFromReactor();

  IEventReactor *GetReactor();

  void OnSignalReceive();

  static void lcb_OnSignal(int fd, short event, void *arg);

private:
  IEventReactor *m_pReactor;
  event m_EvSignal;
  uint32 m_uSignal;
  void *m_pContext;
  FuncOnSignal m_pFuncOnSignal;
  bool m_bLoop;
};

#endif
