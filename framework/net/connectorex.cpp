//
//  ConnectorEx.cpp
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#include "connectorex.h"
#include "BaseTickMgr.h"
#include "Timer.h"

namespace Net
{
    
	CConnectorEx::CConnectorEx(uint32 uId, IConnector* pConnector)
    :m_uId(uId)
    ,m_pConnector(pConnector)
    ,m_uPingTick(0)
    ,m_uLastPingTime(0)
    ,m_uPingValue(0)
    ,m_pOnPingServer(NULL)
	{
		pConnector->SetCallbackFunc(&ConnectorFuncOnDisconnected, &ConnectorFuncOnConnectFailed, &ConnectorFuncOnConnectted, &ConnectorFuncOnSomeDataSend, &ConnectorFuncOnSomeDataRecv);
		pConnector->SetContext(this);
	}
    
	CConnectorEx::~CConnectorEx(void)
	{
	}
    
	void CConnectorEx::GetRemoteIpAddress(char* szBuf, uint32 uBufSize)
	{
		return m_pConnector->GetRemoteIpAddress(szBuf, uBufSize);
	}
    
	uint32	CConnectorEx::GetId() const
	{
		return m_uId;
	}
    
	void CConnectorEx::ShutDown()
	{
		m_pConnector->ShutDown();
		if(m_pOnPingServer)
		{
            GameHub::GetTickMgr()->UnRegisterTick(this);
		}
	}
    
	void CConnectorEx::Release()
	{
		m_pConnector->ShutDown();
		m_pConnector->Release();
		m_pConnector=NULL;
		GH_DELETE_T( this, CConnectorEx, MEMCATEGORY_GENERAL);
	}
    
	void CConnectorEx::SetCallbackFunc(ConnectorExFuncOnDisconnected pOnDisconnected,
                                       ConnectorExFuncOnConnectFailed pOnConnectFailed,
                                       ConnectorExFuncOnConnectted pOnConnectted,
                                       ConnectorExFuncOnSomeDataSend pOnSomeDataSend,
                                       ConnectorExFuncOnSomeDataRecv pOnSomeDataRecv,
                                       ConnectorExFuncOnPingServer pOnPingServer,
                                       uint32 uPingTick)
	{
		m_pFuncOnDisconnected =pOnDisconnected;
		m_pOnConnectFailed =pOnConnectFailed;
		m_pOnConnectted =pOnConnectted;
		m_pOnSomeDataSend =pOnSomeDataSend;
		m_pOnSomeDataRecv =pOnSomeDataRecv;
        
        
		m_uPingTick = uPingTick;
        
		if(NULL != pOnPingServer)
		{
			m_pOnPingServer = pOnPingServer;
            GH_INFO("CConnectorEx RegisterTick %d\n", uPingTick);
			GameHub::GetTickMgr()->RegisterTick(uPingTick, this);
		}
	}
    
	uint32	CConnectorEx::GetPingValue() const
	{
		return m_uPingValue;
	}
    
	void		CConnectorEx::OnPingBack()
	{
		uint32 uCurTime = GameHub::Timer::GetTicksMs();
		m_uPingValue = (uCurTime - m_uLastPingTime)/2;
	}
    
	void		CConnectorEx::OnTick()
	{
        if(this->IsConnected()){
            m_uLastPingTime = GameHub::Timer::GetTicksMs();
            m_pOnPingServer(this);
        }

	}
	
	PipeResult	CConnectorEx::Send(const void* pData, uint32 uSize)
	{
		return m_pConnector->Send(pData,uSize);
	}
    
	void* CConnectorEx::GetRecvData()const
	{
		return m_pConnector->GetRecvData();
	}
    
	uint32 CConnectorEx::GetRecvDataSize()
	{
		return m_pConnector->GetRecvDataSize();
	}
    
	uint32 CConnectorEx::GetSendDataSize()
	{
		return m_pConnector->GetSendDataSize();
	}
    
	void CConnectorEx::PopRecvData(uint32 uSize)
	{
		m_pConnector->PopRecvData(uSize);
	}
    
	void CConnectorEx::SetMaxSendBufSize(uint32 uSize)
	{
		m_pConnector->SetMaxSendBufSize(uSize);
	}
    
	uint32 CConnectorEx::GetMaxSendBufSize()
	{
		return m_pConnector->GetMaxSendBufSize();
	}
    
	void CConnectorEx::SetMaxRecvBufSize(uint32 uSize)
	{
		return m_pConnector->SetMaxRecvBufSize(uSize);
	}
    
	uint32 CConnectorEx::GetMaxRecvBufSize()
	{
		return m_pConnector->GetMaxRecvBufSize();
	}
    
	bool CConnectorEx::IsConnected()
	{
		return m_pConnector->IsConnected();
	}
    
	void CConnectorEx::ConnectorFuncOnDisconnected(IConnector* pConnector)
	{
		CConnectorEx* pConnectorEx = static_cast<CConnectorEx*>(pConnector->GetContext());
		pConnectorEx->m_pFuncOnDisconnected(pConnectorEx);
	}
	
	void CConnectorEx::ConnectorFuncOnConnectFailed(IConnector* pConnector)
	{
		CConnectorEx* pConnectorEx = static_cast<CConnectorEx*>(pConnector->GetContext());
		pConnectorEx->m_pOnConnectFailed(pConnectorEx);
	}
	
	void CConnectorEx::ConnectorFuncOnConnectted(IConnector* pConnector)
	{
		CConnectorEx* pConnectorEx = static_cast<CConnectorEx*>(pConnector->GetContext());
		pConnectorEx->m_pOnConnectted(pConnectorEx);
	}
    
	void CConnectorEx::ConnectorFuncOnSomeDataSend(IConnector* pConnector)
	{
		CConnectorEx* pConnectorEx = static_cast<CConnectorEx*>(pConnector->GetContext());
		pConnectorEx->m_pOnSomeDataSend(pConnectorEx);
	}
    
	void CConnectorEx::ConnectorFuncOnSomeDataRecv(IConnector* pConnector)
	{
		CConnectorEx* pConnectorEx = static_cast<CConnectorEx*>(pConnector->GetContext());
		pConnectorEx->m_pOnSomeDataRecv(pConnectorEx);
	}
    
    
}