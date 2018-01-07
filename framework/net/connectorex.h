//
//  ConnectorEx.h
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#ifndef dpsg_ConnectorEx_h
#define dpsg_ConnectorEx_h

#include "network_interface.h"
#include "net_work.h"

#include "TickMgr.h"

namespace Net
{
	INTER_FACE IConnector;
    
	class NETWORK_API CConnectorEx : public GameHub::CTick
	{
		friend class CNetWork;
	public:
		CConnectorEx(uint32 uId, IConnector* pConnector);
		virtual ~CConnectorEx(void);
        
		uint32	GetId() const;
        
		void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize);
		void		SetCallbackFunc( ConnectorExFuncOnDisconnected pOnDisconnected,
                                    ConnectorExFuncOnConnectFailed pOnConnectFailed,
                                    ConnectorExFuncOnConnectted pOnConnectted,
                                    ConnectorExFuncOnSomeDataSend pOnSomeDataSend,
                                    ConnectorExFuncOnSomeDataRecv pOnSomeDataRecv,
                                    ConnectorExFuncOnPingServer  pOnPingServer,
                                    uint32 uPingTick);
        
		PipeResult Send(const void* pData, uint32 uSize);
		void*	GetRecvData()const;
		uint32	GetRecvDataSize();
		uint32   GetSendDataSize();
		void		PopRecvData(uint32 uSize);
		void		SetMaxSendBufSize(uint32 uSize);
		uint32	GetMaxSendBufSize();
		void		SetMaxRecvBufSize(uint32 uSize);
		uint32	GetMaxRecvBufSize();
		bool		IsConnected();
        
        
		uint32	GetPingValue() const;
		void		OnPingBack();
		void		OnTick();
		const char* GetTickName() { return "CConnectorEx Tick"; };
        
	protected:
		void		ShutDown();
		void		Release();
        
	private:
		static void ConnectorFuncOnDisconnected(IConnector* pConnector);
		static void ConnectorFuncOnConnectFailed(IConnector* pConnector);
		static void ConnectorFuncOnConnectted(IConnector* pConnector);
		static void ConnectorFuncOnSomeDataSend(IConnector* pConnector);
		static void ConnectorFuncOnSomeDataRecv(IConnector* pConnector);
        
	private:
        
		uint32				m_uId;
		IConnector*		m_pConnector;
		uint32				m_uPingTick;
		uint32				m_uLastPingTime;
		uint32				m_uPingValue;
        
		ConnectorExFuncOnDisconnected		m_pFuncOnDisconnected;
		ConnectorExFuncOnConnectFailed		m_pOnConnectFailed;
		ConnectorExFuncOnConnectted			m_pOnConnectted;
		ConnectorExFuncOnSomeDataSend		m_pOnSomeDataSend;
		ConnectorExFuncOnSomeDataRecv		m_pOnSomeDataRecv;
		ConnectorExFuncOnPingServer			m_pOnPingServer;
	};
    
}

#endif
