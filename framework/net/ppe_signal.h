//
//  Signal.h
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#ifndef dpsg_Signal_h
#define dpsg_Signal_h
#include "network_interface.h"


#include "event.h"

namespace Net
{
	class CSystemSignal : public ISystemSignal
	{
	public:
		CSystemSignal(IEventReactor* pReactor);
		~CSystemSignal();
        
		
		void SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop = false);
		void Release();
        
	private:
		bool							RegisterToReactor();
		bool							UnRegisterFromReactor();
		IEventReactor*			GetReactor();
		void							OnSignalReceive();
        
		static void lcb_OnSignal(int fd, short event, void *arg);
        
        
	private:
		IEventReactor*				m_pReactor;
		event							m_EvSignal;
		uint32							m_uSignal;
		void*							m_pContext;
		OnSignal						m_pFuncOnSignal;
		bool								m_bLoop;
	};
}
#endif
