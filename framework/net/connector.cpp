#include <my_assert.h>
#include "connector.h"

CConnector::CConnector(IEventReactor *pReactor,
					   FuncBufferEventOnDataSend funcOnDataSend,
					   FuncBufferEventOnDataRecv funcOnDataRecv,
					   FuncBufferEventOnDisconnected m_pFuncDisconnected)
	: IBufferEvent(pReactor, -1, funcOnDataSend, funcOnDataRecv, m_pFuncDisconnected),
	  m_eState(eCS_Disconnected)
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

bool CConnector::Connect(const CNetAddr &addr)
{
	if (!m_oSocket.Open()) {
		return false;
	}
	sockaddr_in saiAddress;
	m_oAddr.Copy(addr);
	CSocket::Address2SockAddrIn(saiAddress, addr);
	m_oSocket.SetNonblocking();
	GetReactor()->Register(this);
	int iRet = bufferevent_socket_connect(m_pStBufEv, reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr));
	if (iRet != 0) {
		return false;
	}
	SetState(eCS_Connecting);
	return true;
}

void CConnector::SetCallbackFunc(FuncConnectorOnConnectFailed pOnConnectFailed,
								 FuncConnectorOnConnectted pOnConnectted)
{
	m_pFuncOnConnectFailed = pOnConnectFailed;
	m_pFuncOnConnectted = pOnConnectted;
}

void CConnector::OnConnectted()
{
	SetState(eCS_Connected);
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

void CConnector::lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg)
{

}

void CConnector::OnEvent(int16 nWhat)
{
	if (nWhat & BEV_EVENT_CONNECTED) {
		OnConnectted();
		return;
	}
	if (nWhat & EVBUFFER_EOF) {
		MY_ASSERT_STR(false, DO_NOTHING, "%s : %d lcb_OnEvent EVBUFFER_EOF %d",
					  m_oAddr.GetAddress(),
					  m_oAddr.GetPort(),
					  nWhat);
		m_pFuncDisconnected(this);
		return;
	}

	if (nWhat & EVBUFFER_ERROR) {
		MY_ASSERT_STR(false, DO_NOTHING, "%s : %d lcb_OnEvent EVBUFFER_ERROR %d",
					  m_oAddr.GetAddress(),
					  m_oAddr.GetPort(), nWhat);
		m_pFuncDisconnected(this);
		return;
	}
	ShutDown();
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

void CConnector::BuffEventUnavailableCall()
{

}

void CConnector::AfterBuffEventCreated()
{
	bufferevent_enable(m_pStBufEv, EV_WRITE);
}