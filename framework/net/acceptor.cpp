#include "log.h"
#include <my_assert.h>
#include <event2/bufferevent.h>
#include "acceptor.h"

CAcceptor::CAcceptor(SOCKET socket,
					 IEventReactor *pReactor,
					 CNetAddr *netAddr,
					 FuncAcceptorOnDisconnected pOnDisconnected,
					 FuncAcceptorOnSomeDataRecv pOnSomeDataRecv)
	: IBufferEvent(pReactor, socket),
	  m_pNetAddr(netAddr),
	  m_eState(eAS_Disconnected),
	  m_tCreateTime(GetMSTime()),
	  m_pFuncOnDisconnected(pOnDisconnected),
	  m_pFuncOnSomeDataRecv(pOnSomeDataRecv)
{
}

CAcceptor::~CAcceptor()
{
	ShutDown();
}

void CAcceptor::GetRemoteIpAddress(char *szBuf, unsigned int uBufSize)
{
	MY_ASSERT_STR(uBufSize >= 16, return, "uBufSize is too small");
	CNetAddr addr;
	m_oSocket.GetRemoteAddress(addr);
	strncpy(szBuf, addr.GetAddress(), 16);
}

bool CAcceptor::IsConnected()
{
	return m_eState == eAS_Connected;
}

time_t CAcceptor::GetCreateTime()
{
	return m_tCreateTime;
}

void CAcceptor::lcb_OnPipeRead(struct bufferevent *bev, void *arg)
{
	CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
	pAcceptor->m_pFuncOnSomeDataRecv(pAcceptor);
}

void CAcceptor::lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg)
{
	CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
	MY_ASSERT_STR(pAcceptor != NULL, return, "Event param is illegal");
	if (nWhat & EVBUFFER_EOF) {
		pAcceptor->m_pFuncOnDisconnected(pAcceptor);
		return;
	}
	if (nWhat & EVBUFFER_ERROR) {
		pAcceptor->m_pFuncOnDisconnected(pAcceptor);
	}
	return;
}

void CAcceptor::ShutDown()
{
	if (!IsConnected())
		return;
	GetReactor()->UnRegister(this);
	m_oSocket.Close();
	SetState(eAS_Disconnected);
}

CAcceptor::eAcceptorState CAcceptor::GetState()
{
	return m_eState;
}

void CAcceptor::SetState(eAcceptorState eState)
{
	m_eState = eState;
}

void CAcceptor::BuffEventUnavailableCall()
{
	m_pFuncOnDisconnected(this);
}

void CAcceptor::AfterBuffEventCreated()
{
	bufferevent_setcb(m_pStBufEv,
					  &CAcceptor::lcb_OnPipeRead,
					  NULL,
					  &CAcceptor::lcb_OnEvent,
					  (void *) this);
	bufferevent_enable(m_pStBufEv, EV_READ);
	bufferevent_disable(m_pStBufEv, EV_WRITE);
	SetState(eAS_Connected);
}