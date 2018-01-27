#include <sstream>
#include "listener.h"
#include <sys/socket.h>
#include <my_assert.h>

CListener::CListener(IEventReactor *pReactor)
	: m_pEventReactor(pReactor),
	  m_eState(eLS_UnListen),
	  m_pFuncOnAccept(NULL)
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
										  CListener::lcb_Accept,
										  this,
										  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
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
	m_Socket.Shutdown();
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
	if (listen(m_Socket.GetSystemSocket(), SOMAXCONN)) {
		MY_ASSERT_STR(false, exit(0), "Listen failed with error code: %s", strerror(errno))
	}
	GetReactor()->Register(this);//ע���reactor
	m_pFuncOnAccept = pFunc;
	return true;
}