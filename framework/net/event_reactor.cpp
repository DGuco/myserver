#include <my_assert.h>
#include <cstring>
#include "event_reactor.h"
#ifdef EVENT_THREAD_SAVE
#include <event2/thread.h>
#endif

CEventReactor::CEventReactor(eNetModule netModule)
	: m_uReactorHandlerCounter(0),
	  m_pEventBase(NULL)
{
	Init(netModule);
}

CEventReactor::~CEventReactor()
{
	Release();
}

void CEventReactor::Init(eNetModule netModule)
{
#ifdef EVENT_THREAD_SAVE
	evthread_use_pthreads();
#endif
	m_pEventConfig = event_config_new();
	MY_ASSERT_STR(m_pEventConfig != NULL, exit(0), "Create event_config failed");
	if (netModule == eNetModule::NET_SYSTEM) {
		m_pEventBase = event_base_new();
	}
	else {
		for (int i = 0; i < eNetModule::NET_INVALID; ++i) {
			if (netModule != i) {
				event_config_avoid_method(m_pEventConfig, NET_MODULE[i].c_str());
			}
		}
		m_pEventBase = event_base_new_with_config(m_pEventConfig);
	}
	MY_ASSERT_STR(NULL != m_pEventBase, exit(0), "Create Event Base error Init error");
}

event_base *CEventReactor::GetEventBase()
{
	return m_pEventBase;
}

void CEventReactor::Release()
{
	event_base_loopexit(m_pEventBase, NULL);
	event_base_free(m_pEventBase);
	event_config_free(m_pEventConfig);
}

void CEventReactor::DispatchEvents()
{
	int iRet = event_base_dispatch(m_pEventBase);
	MY_ASSERT_STR(iRet == 0, exit(0), "Event loop failed,error msg %s,", strerror(errno))
}

bool CEventReactor::Register(IReactorHandler *pHandler)
{
	m_uReactorHandlerCounter++;
	pHandler->RegisterToReactor();
	return true;
}

bool CEventReactor::UnRegister(IReactorHandler *pHandler)
{
	m_uReactorHandlerCounter--;
	pHandler->UnRegisterFromReactor();
	return true;
}