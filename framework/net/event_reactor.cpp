#include <my_assert.h>
#include "event_reactor.h"
#ifdef EVENT_THREAD_SAFE
#include <event2/thread.h>
#endif

CEventReactor::CEventReactor()
	: m_uReactorHandlerCounter(0),
	  m_pEventBase(NULL)
{
	Init();
}

CEventReactor::~CEventReactor()
{
	Release();
}

void CEventReactor::Init()
{
#ifdef EVENT_THREAD_SAFE
	evthread_use_pthreads();
#endif
	m_pEventBase = event_base_new();
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
}

void CEventReactor::DispatchEvents()
{
	int iRet = event_base_dispatch(m_pEventBase);
	MY_ASSERT_STR(iRet == 0, exit(0), "Event loop failed,error msg %s", strerror(errno))
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