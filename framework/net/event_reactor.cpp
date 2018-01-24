#include <my_assert.h>
#include "event_reactor.h"

CEventReactor::CEventReactor(eNetModule netModule)
	: m_pEventBase(NULL),
	  m_uReactorHandlerCounter(0) {
  Init(netModule);
}

CEventReactor::~CEventReactor(void) {
  Release();
}

void CEventReactor::Init(eNetModule netModule) {
  m_pEventConfig = event_config_new();
  MY_ASSERT_STR(m_pEventConfig != NULL, exit(0), "Create event_config failed");
  for (int i = 0; i < eNetModule::NET_INVALID; ++i) {
	if (netModule != i) {
	  event_config_avoid_method(m_pEventConfig, NET_MODULE[i].c_str());
	}
  }

  m_pEventBase = event_base_new();
  MY_ASSERT_STR(NULL != m_pEventBase, exit(0), "Create Event Base error Init error");
  LOG_INFO("default", "Create event with net module %s", event_base_get_method(m_pEventBase));
}

event_base *CEventReactor::GetEventBase() {
  return m_pEventBase;
}

void CEventReactor::Release() {
  event_base_free(m_pEventBase);
  event_config_free(m_pEventConfig);
}

void CEventReactor::DispatchEvents() {
  event_base_loop(m_pEventBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
}

bool CEventReactor::Register(IReactorHandler *pHandler) {
  m_uReactorHandlerCounter++;
  pHandler->RegisterToReactor();
  return true;
}

bool CEventReactor::UnRegister(IReactorHandler *pHandler) {
  m_uReactorHandlerCounter--;
  pHandler->UnRegisterFromReactor();
  return true;
}