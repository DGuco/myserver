#include <my_assert.h>
#include "connector.h"
#include "../../gateserver/inc/gatedef.h"

CConnector::CConnector(IEventReactor *pReactor)
	: IBufferEvent(pReactor, NULL),
	  m_pReactor(pReactor),
	  m_pStBufEv(NULL),
	  m_uMaxInBufferSize(RECVBUFLENGTH),
	  m_uMaxOutBufferSize(POSTBUFLENGTH),
	  m_eState(eCS_Disconnected),
	  m_pFuncOnDisconnected(NULL),
	  m_pFuncOnConnectFailed(NULL),
	  m_pFuncOnConnectted(NULL),
	  m_pFuncOnSomeDataSend(NULL),
	  m_pFuncOnSomeDataRecv(NULL)
{
}

CConnector::~CConnector(void)
{
	ShutDown();
}

void CConnector::GetRemoteIpAddress(char *szBuf, uint32 uBufSize)
{
	strncpy(szBuf, m_Addr.GetAddress(), 16);
}

bool CConnector::Connect(const CNetAddr &addr, const timeval *time /* = NULL */)
{
	m_Socket.Open();
	sockaddr_in saiAddress;
	m_Addr.Copy(addr);
	CSocket::Address2sockaddr_in(saiAddress, addr);
	m_Socket.SetNonblocking();
	int nResult = connect(m_Socket.GetSystemSocket(), reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr));
	MY_ASSERT(SOCKET_ERROR != nResult, return false);

	switch (errno) {
#if defined(GH_OS_WIN32)
		case EWOULDBLOCK:
#else
	case EINPROGRESS:
#endif
		nResult = 1;
		break;
#ifdef GH_OS_WIN32
		case ENOBUFS:
			nResult=2;
			eReason=ePCFR_NOBUFFER;
			GH_INFO("connect return error ENOBUFS : %d", ENOBUFS);
			return false;
#endif
	case EADDRINUSE:
	case ECONNREFUSED:
	case ETIMEDOUT:
	case ENETUNREACH:
	case ECONNRESET: return false;
	default: return false;
	}

	event_set(&m_ConnectEvent, (int) m_Socket.GetSystemSocket(), EV_WRITE, CConnector::lcb_OnConnectResult, this);
	event_base_set(GetReactor()->GetEventBase(), &m_ConnectEvent);
	event_add(&m_ConnectEvent, time);
	SetState(eCS_Connecting);

	return true;
}

void CConnector::SetCallbackFunc(FuncConnectorOnDisconnected pOnDisconnected,
								 FuncConnectorOnConnectFailed pOnConnectFailed,
								 FuncConnectorOnConnectted pOnConnectted,
								 FuncConnectorOnSomeDataSend pOnSomeDataSend,
								 FuncConnectorOnSomeDataRecv pOnSomeDataRecv)
{
	m_pFuncOnDisconnected = pOnDisconnected;
	m_pFuncOnConnectFailed = pOnConnectFailed;
	m_pFuncOnConnectted = pOnConnectted;
	m_pFuncOnSomeDataSend = pOnSomeDataSend;
	m_pFuncOnSomeDataRecv = pOnSomeDataRecv;
}

void CConnector::lcb_OnConnectResult(int Socket, short nEventMask, void *arg)
{
	CConnector *pConnector = static_cast<CConnector *>(arg);
	pConnector->HandleInput(Socket, nEventMask, NULL);
}

void CConnector::HandleInput(int32 Socket, int16 nEventMask, void *arg)
{
	switch (nEventMask) {
	case EV_WRITE: {
		OnConnectted();
		MY_ASSERT_STR(false, return, "OnConnectted %s : %d ", m_Addr.GetAddress(), m_Addr.GetPort());
	}
	case EV_TIMEOUT: {
		ShutDown();
		m_pFuncOnConnectFailed(this);
		MY_ASSERT_STR(false, return, "connect %s : %d EV_TIMEOUT", m_Addr.GetAddress(), m_Addr.GetPort());
	}
	default: MY_ASSERT_STR(false, return, "connect failed, unkown error!!!");
	}
}

void CConnector::OnConnectted()
{
	GetReactor()->Register(this);
	m_pFuncOnConnectted(this);
}

void CConnector::ShutDown()
{
	if (IsConnecting()) {
		event_del(&m_ConnectEvent);
		MY_ASSERT_STR(false, DO_NOTHING, "ShutDown In Connecting: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort());
	}
	else if (IsConnected()) {
		MY_ASSERT_STR(false, DO_NOTHING, "ShutDown In Connected: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort());
		GetReactor()->UnRegister(this);
	}
	m_Socket.Shutdown();
	SetState(eCS_Disconnected);
}

void CConnector::lcb_OnPipeRead(struct bufferevent *bev, void *arg)
{
	CConnector *pConnector = static_cast<CConnector *>(arg);
	pConnector->m_pFuncOnSomeDataRecv(pConnector);
}

void CConnector::lcb_OnPipeWrite(bufferevent *bev, void *arg)
{
	CConnector *pConnector = static_cast<CConnector *>(arg);
	pConnector->m_pFuncOnSomeDataSend(pConnector);
}

void CConnector::lcb_OnPipeError(bufferevent *bev, int16 nWhat, void *arg)
{

	CConnector *pConnector = static_cast<CConnector *>(arg);

	MY_ASSERT_STR(false, DO_NOTHING, "%s, %d lcb_OnPipeError With PpeGetLastError %d", pConnector->m_Addr.GetAddress(),
				  pConnector->m_Addr.GetPort(), PpeGetLastError());

	pConnector->ShutDown();

	if (nWhat & EVBUFFER_EOF) {

		MY_ASSERT_STR(false, DO_NOTHING, "%s, %d lcb_OnPipeError EVBUFFER_EOF %d",
					  pConnector->m_Addr.GetAddress(),
					  pConnector->m_Addr.GetPort(),
					  nWhat);
		pConnector->m_pFuncOnDisconnected(pConnector);
		return;
	}

	if (nWhat & EVBUFFER_ERROR) {
		MY_ASSERT_STR(false, DO_NOTHING, "%s, %d lcb_OnPipeError EVBUFFER_ERROR %d", pConnector->m_Addr.GetAddress(),
					  pConnector->m_Addr.GetPort(), nWhat);
		pConnector->m_pFuncOnDisconnected(pConnector);
		return;
	}
}

bool CConnector::IsConnected()
{
	return m_eState == eCS_Connected;
}

bool CConnector::IsConnecting()
{
	return m_eState == eCS_Connecting;
}

bool CConnector::IsDisconnected()
{
	return m_eState == eCS_Disconnected;
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
	switch (m_Socket.GetSocketError()) {
	case ePCFR_UNREACH:
	case ePCFR_REFUSED:
	case ePCFR_RESET:
	case ePCFR_LOCALADDRINUSE:
	case ePCFR_NOBUFFER:
	case ePCFR_TIMEDOUT : m_pFuncOnDisconnected(this);
		break;
	default:MY_ASSERT_STR(false, break, "unknown socket error!!!");
	}
}

void CConnector::BuffEventAvailableCall()
{

}

void CConnector::AfterBuffEventCreated()
{
	bufferevent_setcb(m_pStBufEv,
					  CConnector::lcb_OnPipeRead,
					  CConnector::lcb_OnPipeWrite,
					  CConnector::lcb_OnPipeError,
					  (void *) this);
	SetState(eCS_Connected);

}
