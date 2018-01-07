//
//  NetWork.cpp
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//


#include "net_addr.h"
#include "network_interface.h"

#include "connectorex.h"
#include "acceptorex.h"
#include "net_work.h"
#include "BaseTickMgr.h"

namespace Net
{

	CNetWork* CNetWork::m_pNetWork = NULL;
	
	CNetWork& CNetWork::GetSingleton()
	{
		if(NULL == m_pNetWork)
		{
			m_pNetWork = GH_NEW_T(CNetWork, MEMCATEGORY_GENERAL, "")();
		}
		return * m_pNetWork;
	}
    
	void CNetWork::ReleaseSingleton()
	{
		SAFE_RELEASE(m_pNetWork);
	}
    
    
	CNetWork::CNetWork(void)
    :m_uConnectorExCount(0)
    ,m_uAcceptorExCount(0)
    ,m_pEventReactor(NULL)
    ,m_uGcTime(0)
    ,m_uCheckPingTickTime(0)
    ,m_pListener(NULL)
    ,m_pOnNew(NULL)
    ,m_pOnDisconnected(NULL)
    ,m_pOnSomeDataSend(NULL)
    ,m_pOnSomeDataRecv(NULL)
	{
		BegineGc(1000 * 5);
		m_pEventReactor = GetEventReactor();
	}
    
	
	CNetWork::~CNetWork(void)
	{
        
	}
    
    
	
	void CNetWork::Release(void)
	{

		StopGc();
        
		Map_ConnectorExs::iterator ConnectoriterBegin = m_mapConnectorExs.begin();
		Map_ConnectorExs::iterator ConnectoriterEnd = m_mapConnectorExs.end();
		for(;ConnectoriterBegin!=ConnectoriterEnd;++ConnectoriterBegin)
		{
			CConnectorEx* pConnectorEx = ConnectoriterBegin->second;
			pConnectorEx->ShutDown();
			SAFE_RELEASE(pConnectorEx);
		}
		m_mapConnectorExs.clear();
        
		while(!m_quSystemSignals.empty())
		{
			ISystemSignal* pSystemSignal = m_quSystemSignals.front();
			SAFE_RELEASE(pSystemSignal);
			m_quSystemSignals.pop();
		}

		OnTick();

		SAFE_RELEASE(m_pEventReactor);
		GH_DELETE_T( this, NetWork, MEMCATEGORY_GENERAL);
	}
    
	
	void CNetWork::BegineGc(uint32 uGcTime)
	{
		if(uGcTime==m_uGcTime)
			return;
        
		if(m_uGcTime>0)
		{
            GameHub::GetTickMgr()->UnRegisterTick(this);
		}
		m_uGcTime =  uGcTime;
		GameHub::GetTickMgr()->RegisterTick(uGcTime, this);
	}
    
	
	void CNetWork::StopGc()
	{
		if(0==m_uGcTime)
			return;
		
		GameHub::GetTickMgr()->UnRegisterTick(this);
		m_uGcTime =  0;
	}
    
    
	
	void CNetWork::SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop)
	{
		ISystemSignal* pSystemSignal = GetSystemSignal(m_pEventReactor);
		pSystemSignal->SetCallBackSignal(uSignal,pFunc, pContext, bLoop);
		m_quSystemSignals.push(pSystemSignal);
	}
    

	
	uint32 CNetWork::GetConnectorId()
	{
		return ++m_uConnectorExCount;
	}
    
    uint32 CNetWork::GetAcceptorId()
	{
		return ++m_uAcceptorExCount;
	}
    
    
    void CNetWork::OnAccept(IEventReactor* pReactor, SOCKET Socket)
	{
        CNetWork::GetSingleton().NewAcceptor(pReactor,Socket);
	}
    
    
    void CNetWork::NewAcceptor(IEventReactor* pReactor, SOCKET Socket)
	{
		uint32 uId = GetAcceptorId();
		IAcceptor* pAcceptor = GetAcceptor(Socket, pReactor);
		CAcceptorEx* pAcceptorEx  = GH_NEW_T(CAcceptorEx, MEMCATEGORY_GENERAL, "")( uId, pAcceptor);
        
        pAcceptorEx->SetCallbackFunc(m_pOnDisconnected,
                                     m_pOnSomeDataSend,
                                     m_pOnSomeDataRecv,
                                     m_uCheckPingTickTime);
        
		m_mapAcceptorExs.insert(std::make_pair(uId, pAcceptorEx));
		m_pOnNew(uId, pAcceptorEx);
	}
                            
                            
    bool CNetWork::BeginListen(const char* szNetAddr, uint16 uPort,
                               AcceptorExFuncOnNew pOnNew,
                               AcceptorExFuncOnDisconnected pOnDisconnected,
                               AcceptorExFuncOnSomeDataSend pOnSomeDataSend,
                               AcceptorExFuncOnSomeDataRecv pOnSomeDataRecv,
                               uint32 uCheckPingTickTime)
    {
        GH_ASSERT(NULL==m_pListener);
        m_pListener = GetListener(m_pEventReactor);
        CNetAddr addr(szNetAddr, uPort);
        
        bool bRes=m_pListener->Listen(addr, &CNetWork::OnAccept);
        GH_ASSERT(true==bRes);
        m_uCheckPingTickTime = uCheckPingTickTime;
        
        m_pOnNew = pOnNew;
        m_pOnDisconnected = pOnDisconnected;
        m_pOnSomeDataSend = pOnSomeDataSend;
        m_pOnSomeDataRecv = pOnSomeDataRecv;

        
        return true;
    }
                            
    void CNetWork::EndListen()
    {
        GH_ASSERT(NULL!=m_pListener);
        SAFE_RELEASE(m_pListener);
    }
    

	uint32 CNetWork::Connect(const char* szNetAddr, uint16 uPort,
                                                 ConnectorExFuncOnDisconnected pOnDisconnected,
                                                 ConnectorExFuncOnConnectFailed pOnConnectFailed,
                                                 ConnectorExFuncOnConnectted pOnConnectted,
                                                 ConnectorExFuncOnSomeDataSend pOnSomeDataSend,
                                                 ConnectorExFuncOnSomeDataRecv pOnSomeDataRecv,
                                                 ConnectorExFuncOnPingServer  pOnPingServer,
                                                 uint32 uPingTick /* = 45000 */, uint32 uTimeOut)
	{
		uint32 uId = GetConnectorId();
		
		IConnector* pConnector = GetConnector(m_pEventReactor);
		CConnectorEx* pConnectorEx = GH_NEW_T(CConnectorEx, MEMCATEGORY_GENERAL, "")( uId, pConnector);
		
		pConnectorEx->SetCallbackFunc( pOnDisconnected,  pOnConnectFailed,
                                      pOnConnectted,  pOnSomeDataSend,
                                      pOnSomeDataRecv,pOnPingServer,
                                      uPingTick);
        
		CNetAddr addr(szNetAddr, uPort);
		timeval time;
		time.tv_sec = uTimeOut;
		time.tv_usec = 0;
		pConnector->Connect(addr, &time);
		m_mapConnectorExs.insert(std::make_pair(uId, pConnectorEx));
		return uId;
	}
    
	
	uint32 CNetWork::GetConnectorExPingValue(uint32 uId)
	{
		CConnectorEx* pEx = FindConnectorEx(uId);
		if(pEx)
		{
			return pEx->GetPingValue();
		}else
		{
			return 9999999; 
		}
	}
    
	
	void CNetWork::DispatchEvents()
	{
		m_pEventReactor->DispatchEvents();
	}
    
	
	bool CNetWork::ShutDownConnectorEx(uint32 uId)
	{
		Map_ConnectorExs::iterator iter = m_mapConnectorExs.find(uId);
        
        if (m_mapConnectorExs.end()!=iter)
        {
            CConnectorEx* pConnectorEx = iter->second;
            
            pConnectorEx->ShutDown();
            
            m_quIdleConnectorExs.push(pConnectorEx);
            
            m_mapConnectorExs.erase(iter);
            return true;
        }else
        {
            return false;
        }
	}
    
	
    
	
	void CNetWork::OnTick()
	{
		if(!m_quIdleConnectorExs.empty())
		{
			while(!m_quIdleConnectorExs.empty())
			{
				CConnectorEx* pConnectorEx = m_quIdleConnectorExs.front();
				pConnectorEx->Release();
				m_quIdleConnectorExs.pop();
			}
		}
	}
    
	
	CConnectorEx*	CNetWork::FindConnectorEx(uint32 uId)
	{
		Map_ConnectorExs::iterator iter = m_mapConnectorExs.find(uId);
		if(m_mapConnectorExs.end()==iter)
		{
			return NULL;
		}else
		{
			return iter->second;
		}
	}
    
	
	PipeResult CNetWork::ConnectorExSendData(uint32 uId, const void* pData, uint32 uSize)
	{
		CConnectorEx* pConnectorEx = FindConnectorEx(uId);
		if(pConnectorEx)
		{
			PipeResult eRes = pConnectorEx->Send(pData, uSize);
			GH_ASSERT( ePR_OK == eRes );
			return  ePR_OK;
		}
		return ePR_Disconnected;
	}	
}