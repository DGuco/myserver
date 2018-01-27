#include <my_assert.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "acceptor.h"

CAcceptor::CAcceptor(SOCKET socket,
					 IEventReactor *pReactor,
					 CNetAddr *netAddr)
	: IBufferEvent(pReactor, NULL),
	  m_eState(eAS_Disconnected),
	  m_uMaxOutBufferSize(POSTBUFLENGTH),
	  m_uMaxInBufferSize(RECVBUFLENGTH),
	  m_pFuncOnDisconnected(NULL),
	  m_pFuncOnSomeDataSend(NULL),
	  m_pFuncOnSomeDataRecv(NULL),
	  m_pNetAddr(netAddr)
{
	m_Socket.SetSystemSocket(socket);
	m_Socket.SetNonblocking();
	m_pReactor->Register(this);
}

CAcceptor::~CAcceptor()
{
	ShutDown();
}

void CAcceptor::GetRemoteIpAddress(char *szBuf, unsigned int uBufSize)
{
	MY_ASSERT_STR(uBufSize >= 16, return, "uBufSize is too small");
	CNetAddr addr;
	m_Socket.GetRemoteAddress(addr);
	strncpy(szBuf, addr.GetAddress(), 16);
}

void CAcceptor::SetCallbackFunc(FuncAcceptorOnDisconnected pOnDisconnected,
								FuncAcceptorOnSomeDataSend pOnSomeDataSend,
								FuncAcceptorOnSomeDataRecv pOnSomeDataRecv)
{
	m_pFuncOnDisconnected = pOnDisconnected;
	m_pFuncOnSomeDataSend = pOnSomeDataSend;
	m_pFuncOnSomeDataRecv = pOnSomeDataRecv;
}

bool CAcceptor::IsConnected()
{
	return m_eState == eAS_Connected;
}

void CAcceptor::lcb_OnPipeRead(struct bufferevent *bev, void *arg)
{
	CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
	pAcceptor->m_pFuncOnSomeDataRecv(pAcceptor);
}

void CAcceptor::lcb_OnPipeWrite(bufferevent *bev, void *arg)
{
	CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
	pAcceptor->m_pFuncOnSomeDataSend(pAcceptor);
}

void CAcceptor::lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg)
{
	CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
	MY_ASSERT_STR(pAcceptor != NULL, return, "CAcceptor Pipe Error with code %d", PpeGetLastError());

	pAcceptor->ShutDown();
	if (nWhat & EVBUFFER_EOF) {
		pAcceptor->m_pFuncOnDisconnected(pAcceptor);
		return;
	}
	else if (nWhat & EVBUFFER_ERROR) {
		pAcceptor->m_pFuncOnDisconnected(pAcceptor);
		return;
	}

	pAcceptor->ProcessSocketError();
}

void CAcceptor::ShutDown()
{
	if (!IsConnected())
		return;
	GetReactor()->UnRegister(this);
	m_Socket.Close();
	SetState(eAS_Disconnected);
}

PipeResult CAcceptor::Send(const void *pData, unsigned int uSize)
{
	if (!IsConnected())
		return ePR_Disconnected;

	if (uSize > m_uMaxOutBufferSize) {
		return ePR_OutPipeBuf;
	}

	MY_ASSERT(IsEventBuffAvailable(), return ePR_BufNull);
	bufferevent_write(m_pStBufEv, pData, uSize);
	return ePR_OK;
}

CAcceptor::eAcceptorState CAcceptor::GetState()
{
	return m_eState;
}

void CAcceptor::SetState(eAcceptorState eState)
{
	m_eState = eState;
}

CSocket CAcceptor::GetSocket() const
{
	return m_Socket;
}

void CAcceptor::ProcessSocketError()
{
	switch (m_Socket.GetSocketError()) {
	case ePCFR_UNREACH:
	case ePCFR_REFUSED:
	case ePCFR_RESET:
	case ePCFR_LOCALADDRINUSE:
	case ePCFR_NOBUFFER:
	case ePCFR_TIMEDOUT: {
		m_pFuncOnDisconnected(this);
		break;
	}
	default: MY_ASSERT_STR(false, return, "Unknown socket error");
	}
}

void CAcceptor::BuffEventAvailableCall()
{
	m_pFuncOnDisconnected(this);
}

void CAcceptor::AfterBuffEventCreated()
{
	bufferevent_setcb(m_pStBufEv,
					  CAcceptor::lcb_OnPipeRead,
					  CAcceptor::lcb_OnPipeWrite,
					  CAcceptor::lcb_OnEvent,
					  (void *) this);
	SetState(eAS_Connected);
}