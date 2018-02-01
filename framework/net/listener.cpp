#include <sstream>
#include "listener.h"
#include <my_assert.h>

CListener::CListener(IEventReactor *pReactor, int listenQueue)
	: m_pEventReactor(pReactor),
	  m_eState(eLS_UnListen),
	  m_pFuncOnAccept(NULL),
	  m_iListenQueueMax(listenQueue)
{

}

CListener::~CListener(void)
{
	ShutDown();
}

bool CListener::RegisterToReactor()
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_ListenAddress.GetPort());
	sin.sin_addr.s_addr = inet_addr(m_ListenAddress.GetAddress());
	m_pListener = evconnlistener_new_bind(GetReactor()->GetEventBase(),
										  &CListener::lcb_Accept,
										  (void *) this,
										  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
										  -1,
										  (struct sockaddr *) &sin,
										  sizeof(sin));

	MY_ASSERT_STR(m_pListener != NULL, exit(0), "Create evconnlistener failed.")
	SetState(eLS_Listened);
	return true;
}

void CListener::SetState(eListenerState eState)
{
	m_eState = eState;
}

bool CListener::IsListened()
{
	return eLS_Listened == m_eState;
}

bool CListener::UnRegisterFromReactor()
{
	event_del(&m_event);
	SetState(eLS_UnListen);
	return true;
}

void CListener::ShutDown()
{
	if (IsListened())
		GetReactor()->UnRegister(this);
	if (m_pListener != NULL) {
		evconnlistener_free(m_pListener);
	}
}

IEventReactor *CListener::GetReactor()
{
	return m_pEventReactor;
}

void CListener::lcb_Accept(struct evconnlistener *listener,
						   evutil_socket_t fd,
						   struct sockaddr *sa,
						   int socklen,
						   void *arg)
{
	CListener *pListener = static_cast<CListener *>(arg);
	pListener->HandleInput(fd, sa);
}

void CListener::HandleInput(int socket, struct sockaddr *sa)
{
	m_pFuncOnAccept(m_pEventReactor, socket, sa);
}

bool CListener::Listen(CNetAddr &addr, FuncListenerOnAccept pFunc)
{
	m_ListenAddress.SetAddress(addr.GetAddress());
	m_ListenAddress.SetPort(addr.GetPort());
	m_pFuncOnAccept = pFunc;
	GetReactor()->Register(this);
	return true;
}