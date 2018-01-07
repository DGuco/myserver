//
//  Connector.cpp
//  dpsg
//
//  Created by chopdown on 13-1-14.
//
//

#include "connector.h"


namespace Net
{
	IConnector* GetConnector(IEventReactor* pReactor)
	{
		return GH_NEW_T(CConnector, MEMCATEGORY_GENERAL, "")( pReactor);
	}
    
	CConnector::CConnector(IEventReactor* pReactor)
    :m_pReactor(pReactor)
    ,m_pStBufEv(NULL)
    ,m_uMaxInBufferSize(eBS_MaxInBufferSize)
    ,m_uMaxOutBufferSize(eBS_MaxOutBufferSize)
    ,m_eState(eCS_Disconnected)
    ,m_pFuncOnDisconnected(NULL)
    ,m_pFuncOnConnectFailed(NULL)
    ,m_pFuncOnConnectted(NULL)
    ,m_pFuncOnSomeDataSend(NULL)
    ,m_pFuncOnSomeDataRecv(NULL)
	{
	}
	
	CConnector::~CConnector(void)
	{
		GH_ASSERT(true==IsDisconnected());
	}
    
	void	CConnector::GetRemoteIpAddress(char* szBuf, uint32 uBufSize)
	{
		GH_ASSERT(uBufSize >= 16);
		strncpy( szBuf, m_Addr.GetAddress(), 16 );
	}
    
	bool CConnector::Connect(const CNetAddr& addr, const timeval * time /* = NULL */)
	{

		GH_INFO("connect begin : %s , %d ", addr.GetAddress(), addr.GetPort());
        
		GH_ASSERT(NULL==m_pStBufEv);
		EPipeConnFailedReason eReason;
		m_Socket.Open();
		sockaddr_in saiAddress;
		m_Addr.Copy(addr);
		CSocket::Address2sockaddr_in(saiAddress,addr);
		m_Socket.SetNonblocking();
		int nResult=connect(m_Socket.GetSystemSocket(),reinterpret_cast<sockaddr*>(&saiAddress),sizeof(sockaddr));
		GH_ASSERT(SOCKET_ERROR == nResult);
        
		int nError = PpeGetLastError();
		switch(nError)
		{
#if defined(GH_OS_WIN32)
			case EWOULDBLOCK:
#else
			case EINPROGRESS:
#endif
				nResult=1;
				break;
#ifdef GH_OS_WIN32
			case ENOBUFS:
				nResult=2;
				eReason=ePCFR_NOBUFFER;
				GH_INFO("connect return error ENOBUFS : %d", ENOBUFS);
				return false;
#endif
			case EADDRINUSE:
				nResult=2;
				eReason=ePCFR_LOCALADDRINUSE;

				GH_INFO("connect return error EADDRINUSE : %d", EADDRINUSE);
				return false;
			case ECONNREFUSED:
				nResult=2;
				eReason=ePCFR_REFUSED;

				GH_INFO("connect return error ECONNREFUSED : %d", ECONNREFUSED);
				return false;
			case ETIMEDOUT:
				nResult=2;
				eReason=ePCFR_TIMEDOUT;
				GH_INFO("connect return error ETIMEDOUT : %d", ETIMEDOUT);
				return false;
			case ENETUNREACH:
				nResult=2;
				eReason=ePCFR_UNREACH;
				GH_INFO("connect return error ENETUNREACH : %d", ENETUNREACH);
				return false;
			case ECONNRESET:
				nResult=2;
				eReason=ePCFR_RESET;
				GH_INFO("connect return error ECONNRESET : %d", ECONNRESET);
				return false;
			default:
            {

                GH_INFO("connect return error  unkown : %d", nError);
                GH_ASSERT(false);
            }
		}
        
        event_set(&m_ConnectEvent, (int)m_Socket.GetSystemSocket(), EV_WRITE, CConnector::lcb_OnConnectResult, this);
		event_base_set (static_cast<event_base *>(GetReactor()->GetEventBase()), &m_ConnectEvent);
		event_add(&m_ConnectEvent, time);
		SetState(eCS_Connecting);
        
		return true;
	}
    
	void CConnector::SetCallbackFunc(ConnectorFuncOnDisconnected pOnDisconnected, ConnectorFuncOnConnectFailed pOnConnectFailed, ConnectorFuncOnConnectted pOnConnectted, ConnectorFuncOnSomeDataSend pOnSomeDataSend, ConnectorFuncOnSomeDataRecv pOnSomeDataRecv)
	{
		m_pFuncOnDisconnected = pOnDisconnected;
		m_pFuncOnConnectFailed = pOnConnectFailed;
		m_pFuncOnConnectted = pOnConnectted;
		m_pFuncOnSomeDataSend = pOnSomeDataSend;
		m_pFuncOnSomeDataRecv = pOnSomeDataRecv;
	}
    
	void	CConnector::lcb_OnConnectResult(int Socket, short nEventMask, void *arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->HandleInput(Socket,nEventMask,NULL);
	}
    
	void CConnector::HandleInput(int32 Socket, int16 nEventMask, void* arg)
	{
		switch(nEventMask)
		{
            case EV_WRITE:
			{

				GH_INFO("OnConnectted %s : %d ", m_Addr.GetAddress(), m_Addr.GetPort());
				OnConnectted();
				return;
			}break;
            case EV_TIMEOUT:
			{
				GH_INFO("connect %s : %d EV_TIMEOUT", m_Addr.GetAddress(), m_Addr.GetPort());
				GH_ASSERT(IsConnecting());
				ShutDown();
				m_pFuncOnConnectFailed(this);
			}break;
            default:
			{
				GH_DEBUG_ASSERT(false, "connect failed, unkown error!!!");
			}break;
		}
	}
    
	void		CConnector::OnConnectted()
	{
		GetReactor()->Register(this);
		m_pFuncOnConnectted(this);
	}
    
	bool		CConnector::RegisterToReactor()
	{
		m_pStBufEv = bufferevent_new(static_cast<int>(m_Socket.GetSystemSocket()),
                                     CConnector::lcb_OnPipeRead,
                                     CConnector::lcb_OnPipeWrite,
                                     CConnector::lcb_OnPipeError,this);
        
		if(NULL==m_pStBufEv)
			GH_DEBUG_ASSERT(false, "bufferevent_new failed!!");
        
		bufferevent_setwatermark(m_pStBufEv,EV_READ,0,m_uMaxInBufferSize);
		bufferevent_setwatermark(m_pStBufEv,EV_WRITE,0,m_uMaxOutBufferSize);
		bufferevent_base_set(static_cast<event_base *>(GetReactor()->GetEventBase()), m_pStBufEv);
        
		int32 nRes = bufferevent_enable(m_pStBufEv, EV_READ | EV_WRITE);
		GH_ASSERT(0==nRes);
		SetState(eCS_Connected);
		return true;
	}
    
	bool		CConnector::UnRegisterFromReactor()
	{
		GH_ASSERT(NULL!=m_pStBufEv);
		bufferevent_disable(m_pStBufEv, EV_READ | EV_WRITE);
		return true;
	}
    
	void	CConnector::ShutDown()
	{
		if(IsConnecting())
		{
			GH_INFO("ShutDown In Connecting: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort());

			event_del(&m_ConnectEvent);
		}
		else if(IsConnected())
		{
			GH_INFO("ShutDown In Connected: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort());
			GetReactor()->UnRegister(this);
		}
		m_Socket.Shutdown();
		SetState(eCS_Disconnected);
	}
    
	void		CConnector::Release()
	{
		ShutDown();
		if(m_pStBufEv)
		{
			bufferevent_free(m_pStBufEv);
			m_pStBufEv = NULL;
		}
		GH_DELETE_T( this, CConnector, MEMCATEGORY_GENERAL);
	}
    
	void	 CConnector::lcb_OnPipeRead(struct bufferevent* bev, void* arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->m_pFuncOnSomeDataRecv(pConnector);
	}
    
	void	CConnector::lcb_OnPipeWrite(bufferevent* bev, void* arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->m_pFuncOnSomeDataSend(pConnector);
	}
    
	void	CConnector::lcb_OnPipeError(bufferevent* bev, int16 nWhat, void* arg)
	{

		CConnector* pConnector = static_cast<CConnector *>(arg);

		GH_INFO("%s, %d lcb_OnPipeError With PpeGetLastError %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort(), PpeGetLastError());

		pConnector->ShutDown();
		
		if(nWhat&EVBUFFER_EOF)
		{

			GH_INFO("%s, %d lcb_OnPipeError EVBUFFER_EOF %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort() , nWhat);
			pConnector->m_pFuncOnDisconnected(pConnector);
			return;
		}
        
		if(nWhat&EVBUFFER_ERROR)
		{
			GH_INFO("%s, %d lcb_OnPipeError EVBUFFER_ERROR %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort() , nWhat);
			pConnector->m_pFuncOnDisconnected(pConnector);
			return;
		}
		GH_ASSERT(false);
	}
    
	void*	CConnector::GetContext(void) const
	{
		return m_pContext;
	}
    
	void		CConnector::SetContext(void* pContext)
	{
		m_pContext = pContext;
	}
    
	PipeResult CConnector::Send(const void* pData, uint32 uSize)
	{
		if(!IsConnected())
			return ePR_Disconnected;
        
		if(m_pStBufEv->output->off+uSize > m_uMaxOutBufferSize)
		{

			GH_INFO("%s %d CConnector send buffer out of size", m_Addr.GetAddress(), m_Addr.GetPort() );
			ShutDown();
			m_pFuncOnDisconnected(this);
			return ePR_OutPipeBuf;
		}
        
		int32 nRes = bufferevent_write(m_pStBufEv,  pData, uSize);
		GH_ASSERT(0==nRes);
		return ePR_OK;
	}
    
	void*	CConnector::GetRecvData()const
	{
		GH_ASSERT(NULL!=m_pStBufEv);
		return m_pStBufEv->input->buffer;
	}
    
	uint32	CConnector::GetRecvDataSize()
	{
		GH_ASSERT(NULL!=m_pStBufEv);
		return static_cast<uint32>(m_pStBufEv->input->off);
	}
    
	uint32	CConnector::GetSendDataSize()
	{
		GH_ASSERT(NULL!=m_pStBufEv);
		return static_cast<uint32>(m_pStBufEv->output->off);
	}
    
	void		CConnector::PopRecvData(uint32 uSize)
	{
		GH_ASSERT(NULL!=m_pStBufEv);
		GH_ASSERT(m_pStBufEv->input->off >= uSize);
		GH_ASSERT(uSize>=0);
		if(uSize>0)
			evbuffer_drain(m_pStBufEv->input,uSize);
	}
    
	void		CConnector::SetMaxSendBufSize(uint32 uSize)
	{
		GH_ASSERT(uSize>0);
		m_uMaxOutBufferSize=uSize;
		if(IsConnected())
		{
			GH_ASSERT(NULL!=m_pStBufEv);
			bufferevent_setwatermark(m_pStBufEv,EV_WRITE,0,m_uMaxOutBufferSize);
		}
	}
    
	uint32	CConnector::GetMaxSendBufSize()
	{
		return m_uMaxOutBufferSize;
	}
    
	void	CConnector::SetMaxRecvBufSize(uint32 uSize)
	{
		GH_ASSERT(uSize>0);
		m_uMaxInBufferSize=uSize;
		if(IsConnected())
		{
			GH_ASSERT(NULL!=m_pStBufEv);
			bufferevent_setwatermark(m_pStBufEv,EV_READ,0,m_uMaxInBufferSize);
		}
	}
    
	uint32	CConnector::GetMaxRecvBufSize()
	{
		return m_uMaxInBufferSize;
	}
    
	bool		CConnector::IsConnected()
	{
		return m_eState == eCS_Connected;
	}
    
	bool	CConnector::IsConnecting()
	{
		return m_eState == eCS_Connecting;
	}
    
	bool CConnector::IsDisconnected()
	{
		return m_eState == eCS_Disconnected;
	}
    
	IEventReactor* CConnector::GetReactor()
	{
		return m_pReactor;
	}
    
    
	CConnector::eConnectorState CConnector::GetState()
	{
		return m_eState;
	}
	
	void CConnector::SetState(eConnectorState eState)
	{
		m_eState = eState;
	}
    
    
	void CConnector::ProcessSocketError()
	{
		switch(m_Socket.GetSocketError())
		{
            case ePCFR_UNREACH:
            case ePCFR_REFUSED:
            case ePCFR_RESET:
            case ePCFR_LOCALADDRINUSE:
            case ePCFR_NOBUFFER:
            case ePCFR_TIMEDOUT:
                m_pFuncOnDisconnected(this);
                break;
            default:
                GH_DEBUG_ASSERT(false, "unknown socket error!!!");
		}
	}
    
}