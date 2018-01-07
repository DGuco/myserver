//
//  EventReactor.cpp
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#include "event_reactor.h"


namespace Net
{
	IEventReactor* GetEventReactor()
	{
		IEventReactor*  pReactor = GH_NEW CEventReactor();
		pReactor->Init();
		return pReactor;
	}
    
	CEventReactor::CEventReactor(void)
    :m_pEventBase(NULL)
    ,m_uReactorHandlerCounter(0)
	{
	}
    
	CEventReactor::~CEventReactor(void)
	{
	}
    
	void*	CEventReactor::GetEventBase()
	{
		return m_pEventBase;
	}
    
	void CEventReactor::Init()
	{
#ifdef _WIN32
		WORD wVersion;
		WSADATA wsaData;
		wVersion = MAKEWORD(2, 2);
		int nResult= WSAStartup(wVersion, &wsaData);
		if(nResult)
		{
			GH_DEBUG_ASSERT(false, "WSAStartup failed with error code: %d\n", nResult);
		}
#endif
        
		m_pEventBase = event_base_new();
        GH_DEBUG_ASSERT(m_pEventBase, "CEventReactor Init error");

	}
    
	void CEventReactor::Release()
	{

		GH_DEBUG_ASSERT(0 == m_uReactorHandlerCounter, "0 != m_uReactorHandlerCounter");

        
		event_base_free(m_pEventBase);
#ifdef _WIN32//WIN32
		int nResult= WSACleanup();
        
        GH_DEBUG_ASSERT(0 == nResult, "WSACleanup failed with error code: %d\n", nResult);
        
#endif
		GH_DELETE this;
	}
    
	void CEventReactor::DispatchEvents()
	{
		event_base_loop(m_pEventBase,EVLOOP_ONCE|EVLOOP_NONBLOCK);
	}
    
	bool CEventReactor::Register(IReactorHandler* pHandler)
	{
		m_uReactorHandlerCounter++;
        
		pHandler->RegisterToReactor();
		return true;
	}
    
	bool CEventReactor::UnRegister(IReactorHandler* pHandler)
	{
		GH_ASSERT(0<m_uReactorHandlerCounter);
		m_uReactorHandlerCounter--;

		pHandler->UnRegisterFromReactor();
		return true;
	}
}