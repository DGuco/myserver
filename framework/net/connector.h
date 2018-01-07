//
//  Connector.h
//  dpsg
//
//  Created by chopdown on 13-1-12.
//
//

#ifndef dpsg_Connector_h
#define dpsg_Connector_h

#include "network_interface.h"
#include "net_Inc.h"
#include "Socket.h"
#include "event_reactor.h"
#include "net_addr.h"

namespace Net
{
	class NETWORK_API CConnector :public IConnector
	{
		enum eConnectorState
		{
			eCS_Disconnected=0,
			eCS_Connecting,
			eCS_Connected,
		};
        
		enum eBufSize
		{
			eBS_MaxOutBufferSize = 1024 * 16,
			eBS_MaxInBufferSize = 1024 * 128,
		};
        
	public:
		CConnector(IEventReactor* pReactor);
		virtual ~CConnector(void);
        
        
		void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize);
		bool		Connect(const CNetAddr& addr, const timeval * time = NULL);
		void		SetCallbackFunc( ConnectorFuncOnDisconnected pOnDisconnected,
                                    ConnectorFuncOnConnectFailed pOnConnectFailed,
                                    ConnectorFuncOnConnectted pOnConnectted,
                                    ConnectorFuncOnSomeDataSend pOnSomeDataSend,
                                    ConnectorFuncOnSomeDataRecv pOnSomeDataRecv);
        
		void*	GetContext(void) const;
		void		SetContext(void* pContext);
        
		PipeResult Send(const void* pData, uint32 uSize);
		void		ShutDown();
		void*	GetRecvData()const;
		uint32	GetRecvDataSize();
		uint32   GetSendDataSize();
		void		PopRecvData(uint32 uSize);
		void		SetMaxSendBufSize(uint32 uSize);
		uint32	GetMaxSendBufSize();
		void		SetMaxRecvBufSize(uint32 uSize);
		uint32	GetMaxRecvBufSize();
		bool		IsConnected();
		bool		IsConnecting();
		bool		IsDisconnected();
		void		Release();
		
        
	private:
        
		IEventReactor*	GetReactor();
		bool		RegisterToReactor();
		bool		UnRegisterFromReactor();
        
		static void	lcb_OnConnectResult(int Socket, short nEventMask, void *arg);
		static void	lcb_OnPipeRead(struct bufferevent* bev, void* arg);
		static void	lcb_OnPipeWrite(bufferevent* bev, void* arg);
		static void	lcb_OnPipeError(bufferevent* bev, int16 nWhat, void* arg);
        
		eConnectorState GetState();
		void SetState(eConnectorState eState);
        
		void OnConnectted();
        
		void HandleInput(int32 Socket, int16 nEventMask, void* arg);
        
		void ProcessSocketError();
	private:
		IEventReactor*			m_pReactor;
        
		ConnectorFuncOnDisconnected		m_pFuncOnDisconnected;
		ConnectorFuncOnConnectFailed		m_pFuncOnConnectFailed;
		ConnectorFuncOnConnectted			m_pFuncOnConnectted;
		ConnectorFuncOnSomeDataSend		m_pFuncOnSomeDataSend;
		ConnectorFuncOnSomeDataRecv		m_pFuncOnSomeDataRecv;
        
		void*						m_pContext;
		CNetAddr				m_Addr;
		CSocket					m_Socket;
		eConnectorState		m_eState;
		bufferevent*				m_pStBufEv;
		event						m_ConnectEvent;
		uint32						m_uMaxOutBufferSize;		
		uint32						m_uMaxInBufferSize;			
	};
}


#endif
