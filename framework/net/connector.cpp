#include <my_assert.h>
#include "connector.h"

CConnector::CConnector(IEventReactor *pReactor,
					   FuncBufferEventOnDataSend funcOnDataSend,
					   FuncBufferEventOnDataRecv funcOnDataRecv,
					   FuncBufferEventOnDisconnected funcDisconnected,
					   int iTargetId,
					   int iPingTick)
	: IBufferEvent(pReactor,
				   -1,
				   funcOnDataSend,
				   funcOnDataRecv,
				   funcDisconnected),
	  m_eState(eCS_Disconnected),
	  m_iTargetId(iTargetId),
	  m_iPingTick(iPingTick),
	  m_pKeepLiveEvent(NULL)
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
	sockaddr_in saiAddress;
	m_oAddr.Copy(addr);
	CSocket::Address2SockAddrIn(saiAddress, addr);
	GetReactor()->Register(this);
	int iRet = bufferevent_socket_connect(m_pStBufEv, reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr));
	if (iRet != 0) {
		return false;
	}
	OnConnectted();
	return true;
}

bool CConnector::ReConnect()
{
	int iRet = bufferevent_socket_connect(m_pStBufEv, 0, 0);
	if (iRet != 0) {
		return false;
	}
	SetState(eCS_Connected);
	return true;
}

void CConnector::SetCallbackFunc(FuncConnectorOnConnectFailed pOnConnectFailed,
								 FuncConnectorOnConnectted pOnConnected,
								 FuncConnectorOnPingServer pOnPingServer)
{
	m_pFuncOnConnectFailed = pOnConnectFailed;
	m_pFuncOnConnectted = pOnConnected;
	m_pFuncOnPingServer = pOnPingServer;
}

void CConnector::OnConnectted()
{
	SetState(eCS_Connected);
	m_oSocket.SetSocket(bufferevent_getfd(m_pStBufEv));
	m_pFuncOnConnectted(this);
	m_pKeepLiveEvent = new CTimerEvent(GetReactor(),
									   &CConnector::lcb_OnPingServer,
									   (void *) this,
									   m_iPingTick,
									   0,
									   -1);
}

void CConnector::ShutDown()
{
	event_del(&m_oConnectEvent);
	SetState(eCS_Disconnected);
}

void CConnector::OnEvent(int16 nWhat)
{
	if (nWhat & BEV_EVENT_EOF ||
		nWhat & BEV_EVENT_READING ||
		nWhat & BEV_EVENT_ERROR ||
		nWhat & BEV_EVENT_WRITING) {
		SetState(eCS_Disconnected);
		m_pFuncDisconnected(this);
		return;
	}

//	if (nWhat & BEV_EVENT_CONNECTED) {
//		m_pFuncOnConnectted(this);
//	}
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
	bufferevent_enable(m_pStBufEv, EV_READ | EV_WRITE);
}

int CConnector::GetTargetId() const
{
	return m_iTargetId;
}

void CConnector::lcb_OnPingServer(int fd, short event, void *param)
{
	CConnector *pConnector = (CConnector *) param;
	MY_ASSERT(pConnector != NULL, return;)
	if (pConnector->GetState() == eConnectorState::eCS_Connected) {
		pConnector->m_pFuncOnPingServer(fd, event, pConnector);
	}
}
