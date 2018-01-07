//
//  TNetWork.h
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#ifndef dpsg_TNetWork_h
#define dpsg_TNetWork_h

#include "network_interface.h"
#include <map>
#include <queue>
#include "TickMgr.h"

namespace Net
{


	INTER_FACE IConnector;

	
	class CConnectorEx;
    class CAcceptorEx;
    
    typedef void(*AcceptorExFuncOnNew)(uint32 uId, CAcceptorEx* pAcceptorEx);
    typedef void(*AcceptorExFuncOnDisconnected)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnSomeDataSend)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnSomeDataRecv)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnClientLost)(CAcceptorEx* pAcceptorEx);

    
	typedef void(*ConnectorExFuncOnDisconnected)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnConnectFailed)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnConnectted)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnSomeDataSend)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnSomeDataRecv)(CConnectorEx* pConnectorEx);
	
	typedef void(*ConnectorExFuncOnPingServer)(CConnectorEx* pConnectorEx);
    
    typedef bool(*EnumAcceptorExCallbackFunc)(CAcceptorEx* pConnectorEx, void* pContext);

	class CNetWork : public GameHub::CTick
	{
	public:
		CNetWork();
		virtual ~CNetWork();
        
        static CNetWork& GetSingleton();
		static void ReleaseSingleton();
        
        
        bool BeginListen(const char* szNetAddr, uint16 uPort,
                         AcceptorExFuncOnNew pOnNew,
                         AcceptorExFuncOnDisconnected pOnDisconnected,
                         AcceptorExFuncOnSomeDataSend pOnSomeDataSend,
                         AcceptorExFuncOnSomeDataRecv pOnSomeDataRecv,
                         uint32 uCheckPingTickTime = 0);
		void EndListen();
        


		uint32 Connect(const char* szNetAddr, uint16 uPort,	ConnectorExFuncOnDisconnected		pOnDisconnected,
                       ConnectorExFuncOnConnectFailed		pOnConnectFailed,
                       ConnectorExFuncOnConnectted			pOnConnectted,
                       ConnectorExFuncOnSomeDataSend		pOnSomeDataSend,
                       ConnectorExFuncOnSomeDataRecv		pOnSomeDataRecv,
                       ConnectorExFuncOnPingServer			pOnPingServer,
                       uint32 uPingTick = 4500, uint32 uTimeOut = 30);
        
        
        bool ShutDownAcceptorEx(uint32 uId);
        
		void	SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop = false);
        
        
		PipeResult ConnectorExSendData(uint32 uId, const void* pData, uint32 uSize);
        
        
        void EnumAcceptorEx(EnumAcceptorExCallbackFunc pFunc, void* pContext);

        
		bool ShutDownConnectorEx(uint32 uId);

		void DispatchEvents();
        
		void BegineGc(uint32 uGcTime);
		void StopGc();
        
		uint32 GetConnectorExPingValue(uint32 uId);
        
		CConnectorEx*	FindConnectorEx(uint32 uId);
        CAcceptorEx*	FindAcceptorEx(uint32 uId);
        
	private:
        
        struct EnumContex
		{
			EnumAcceptorExCallbackFunc pFunc;
			void* pContext;
		};
        
        
		void OnTick();
		const char* GetTickName() { return "NetWork Tick"; };

		uint32 GetConnectorId();
        uint32 GetAcceptorId();
        
        static void OnAccept(IEventReactor* pReactor, SOCKET Socket);
		void NewAcceptor(IEventReactor* pReactor, SOCKET Socket);
        
        
    protected:
		virtual void Release();
        
        
	private:
		uint32											m_uGcTime;

		uint32											m_uConnectorExCount;
        uint32											m_uAcceptorExCount;
        uint32                                          m_uCheckPingTickTime;
        
		IEventReactor*								m_pEventReactor;
        
		typedef std::map<uint32, CConnectorEx*>		Map_ConnectorExs;
        typedef std::map<uint32, CAcceptorEx*>		Map_AcceptorExs;
		typedef std::queue<ISystemSignal*>			Queue_SystemSignals;
        
		Map_ConnectorExs							m_mapConnectorExs;
        Map_AcceptorExs                             m_mapAcceptorExs;
		Queue_SystemSignals                         m_quSystemSignals;
        
		typedef std::queue<CConnectorEx*>       Queue_IdleConnectorExs;
        
		Queue_IdleConnectorExs	m_quIdleConnectorExs;
        
        static  CNetWork* m_pNetWork;
        IListener*                                  m_pListener;
        
        AcceptorExFuncOnNew                         m_pOnNew;
        AcceptorExFuncOnDisconnected                m_pOnDisconnected;
        AcceptorExFuncOnSomeDataSend                m_pOnSomeDataSend;
        AcceptorExFuncOnSomeDataRecv                m_pOnSomeDataRecv;

	};
}

#endif
