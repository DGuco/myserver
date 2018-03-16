#include <my_assert.h>
#include "connector.h"

CConnector::CConnector(IEventReactor *pReactor)
	: IBufferEvent(pReactor),
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
	strncpy(szBuf, m_oAddr.GetAddress(), 16);
}

bool CConnector::Connect(const CNetAddr &addr, const timeval time /* = NULL */)
{
	if (!m_oSocket.Open()) {
		return false;
	}
	sockaddr_in saiAddress;
	m_oAddr.Copy(addr);
	CSocket::Address2SockAddrIn(saiAddress, addr);
	m_oSocket.SetNonblocking();
//	int iRet = bufferevent_socket_connect(m_pStBufEv, reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr));
//	if (iRet != 0) {
//		return false;
//	}
	int iRet = connect(m_oSocket.GetSocket(), reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr));
	if (iRet != 0) {
		//连接操作进行中
		if (errno == EINPROGRESS) {
			event_set(&m_oConnectEvent, m_oSocket.GetSocket(), EV_WRITE, &CConnector::lcb_OnConnectResult, this);
			event_base_set(GetReactor()->GetEventBase(), &m_oConnectEvent);
			event_add(&m_oConnectEvent, &time);
			SetState(eCS_Connecting);
			return true;
		}
		else {
			return false;
		}
	}
	else {
		//连接成功
		OnConnectted();
	}
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

void CConnector::lcb_OnConnectResult(int socket, short nEventMask, void *arg)
{
	CConnector *pConnector = static_cast<CConnector *>(arg);
	pConnector->HandleInput(socket, nEventMask, NULL);
}

void CConnector::HandleInput(int32 socket, int16 nEventMask, void *arg)
{
	switch (nEventMask) {
	case EV_WRITE: {
		OnConnectted();
		MY_ASSERT_STR(false, return, "OnConnectted %s : %s ", m_oAddr.GetAddress(), m_oAddr.GetPort());
	}
	case EV_TIMEOUT: {
		ShutDown();
		m_pFuncOnConnectFailed(this);
		MY_ASSERT_STR(false, return, "connect %s : %d EV_TIMEOUT", m_oAddr.GetAddress(), m_oAddr.GetPort());
	}
	default: MY_ASSERT_STR(false, return, "connect failed, unkown error!!!");
	}
}

void CConnector::OnConnectted()
{
	SetState(eCS_Connected);
	GetReactor()->Register(this);
	m_pFuncOnConnectted(this);
}

void CConnector::ShutDown()
{
	if (IsConnecting()) {
		event_del(&m_oConnectEvent);
		MY_ASSERT_STR(false, DO_NOTHING, "ShutDown In Connecting: %s : %d", m_oAddr.GetAddress(), m_oAddr.GetPort());
	}
	else if (IsConnected()) {
		MY_ASSERT_STR(false, DO_NOTHING, "ShutDown In Connected: %s : %d", m_oAddr.GetAddress(), m_oAddr.GetPort());
		GetReactor()->UnRegister(this);
	}
	m_oSocket.Close();
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
	MY_ASSERT_STR(false,
				  DO_NOTHING,
				  "%s : %d lcb_OnPipeError With PpeGetLastError {}",
				  pConnector->m_oAddr.GetAddress(),
				  pConnector->m_oAddr.GetPort(),
				  PpeGetLastError());
	pConnector->ShutDown();
	if (nWhat & EVBUFFER_EOF) {
		MY_ASSERT_STR(false, DO_NOTHING, "%s : %d lcb_OnPipeError EVBUFFER_EOF %d",
					  pConnector->m_oAddr.GetAddress(),
					  pConnector->m_oAddr.GetPort(),
					  nWhat);
		pConnector->m_pFuncOnDisconnected(pConnector);
		return;
	}

	if (nWhat & EVBUFFER_ERROR) {
		MY_ASSERT_STR(false, DO_NOTHING, "%s : %d lcb_OnPipeError EVBUFFER_ERROR %d", pConnector->m_oAddr.GetAddress(),
					  pConnector->m_oAddr.GetPort(), nWhat);
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
	switch (m_oSocket.GetSocketError()) {
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

void CConnector::BuffEventUnavailableCall()
{

}

void CConnector::AfterBuffEventCreated()
{
	bufferevent_setcb(m_pStBufEv,
					  CConnector::lcb_OnPipeRead,
					  CConnector::lcb_OnPipeWrite,
					  CConnector::lcb_OnPipeError,
					  (void *) this);
//	bufferevent_enable(m_pStBufEv, EV_READ | EV_WRITE);
}