//
//  EventReactor.h
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#ifndef dpsg_EventReactor_h
#define dpsg_EventReactor_h

#include "net_Inc.h"
#include "network_interface.h"


#include "event.h"


namespace Net
{
	class CEventReactor : public IEventReactor
	{
	public:
		CEventReactor(void);
		virtual ~CEventReactor(void);
        
		bool Register(IReactorHandler* pHandler);
		bool UnRegister(IReactorHandler* pHandler);
        
		void*	GetEventBase();
        
		void Init();
		void DispatchEvents();
		void	Release();
	private:
		uint32	m_uReactorHandlerCounter;
		event_base * m_pEventBase;
	};
}


#endif
