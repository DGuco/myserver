//
//  Signal.cpp
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#include "ppe_signal.h"

namespace Net
{
	ISystemSignal*	GetSystemSignal(IEventReactor* pReactor)
	{
		return GH_NEW CSystemSignal(pReactor);
	}
    
	CSystemSignal::CSystemSignal(IEventReactor* pReactor)
    :m_pFuncOnSignal(NULL)
    ,m_pContext(NULL)
    ,m_pReactor(pReactor)
    ,m_bLoop(false)
	{
        
	}
    
	CSystemSignal::~CSystemSignal()
	{
        
	}
    
	IEventReactor*	 CSystemSignal::GetReactor()
	{
		return m_pReactor;
	}
    
	void CSystemSignal::SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop /* = false */)
	{
		m_pFuncOnSignal = pFunc;
		m_uSignal = uSignal;
		m_pContext = pContext;
		m_bLoop = bLoop;
		GetReactor()->Register(this);
	}
    
	bool	CSystemSignal::RegisterToReactor()
	{
		event_set(&m_EvSignal, m_uSignal, EV_SIGNAL, lcb_OnSignal,this);
		event_base_set (static_cast<event_base *>(GetReactor()->GetEventBase()), &m_EvSignal);
		event_add(&m_EvSignal, NULL);
        
		return true;
	}
    
	void CSystemSignal::lcb_OnSignal(int fd, short event, void *arg)
	{
		CSystemSignal* pSignal = static_cast<CSystemSignal*>(arg);
		pSignal->OnSignalReceive();
	}
    
	void CSystemSignal::OnSignalReceive()
	{
		if(!m_bLoop)
		{
			UnRegisterFromReactor();
		}
        
		GH_ASSERT( m_uSignal == EVENT_SIGNAL(&m_EvSignal));
		m_pFuncOnSignal(m_uSignal, m_pContext);
	}
	
	bool CSystemSignal::UnRegisterFromReactor()
	{
		event_del(&m_EvSignal);
        
		return true;
	}
    
	void CSystemSignal::Release()
	{
		GetReactor()->UnRegister(this);
		GH_DELETE this;
	}
}
