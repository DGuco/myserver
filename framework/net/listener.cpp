#include <sstream>
#include "listener.h"
#include <sys/socket.h>



namespace Net
{
	IListener* GetListener(IEventReactor* pReactor)
	{
		return GH_NEW_T(CListener, MEMCATEGORY_GENERAL, "")(pReactor);
	}

	listener::listener(IEventReactor* pReactor)
		:m_pEventReactor(pReactor)
		,m_eState(eLS_UnListen)
		,m_pFuncOnAccept(NULL)
	{

	}

	listener::~listener(void)
	{
	}


	bool listener::RegisterToReactor()
	{
        event_set(&m_event, m_Socket.GetSystemSocket(), EV_READ|EV_PERSIST, &listener::lcb_Accept,this);
        event_base_set (static_cast<event_base *>(GetReactor()->GetEventBase()), &m_event);
		event_add(&m_event, NULL);

		SetState(eLS_Listened);
		return true;
	}

	void listener::SetState(eListenerState eState)
	{
		m_eState = eState;
	}

	bool listener::IsListened()
	{
		return eLS_Listened == m_eState;
	}

	bool listener::UnRegisterFromReactor()
	{
		event_del(&m_event);
		SetState(eLS_UnListen);
		return true;
	}
	
	void listener::ShutDown()
	{
		if(IsListened())
			GetReactor()->UnRegister(this);
	}

	void listener::Release()
	{
		ShutDown();
		GH_DELETE_T( this, listener, MEMCATEGORY_GENERAL);
	}

	IEventReactor*		listener::GetReactor()
	{
		return m_pEventReactor;
	}

	void listener::lcb_Accept(int Socket, short nEventMask, void* arg)
	{
		listener* pListener = static_cast<listener *>(arg);
		pListener->HandleInput(Socket,nEventMask,pListener);
	}


	void listener::HandleInput(int Socket, short nEventMask, void* arg)
	{


		SOCKET NewSocket;
		for(;;)
		{
			// �����µ��������� 
			NewSocket = accept(m_Socket.GetSystemSocket(), NULL, NULL);
			if(INVALID_SOCKET == NewSocket)
			{
				int32 nErrorCode = PpeGetLastError();
				switch(nErrorCode)
				{
#ifdef __APPLE_CC__ 
                    case ECONNABORTED:
                    case EINTR:
                    case EWOULDBLOCK:
                        return;
                    case EMFILE:
                    {
                        GH_INFO("accept socket failed : EMFILE");
                        GH_ASSERT(false);
                        return;
                    }break;
#endif
                        
#if defined(__linux__)
						case ECONNABORTED:
						case EINTR:
						case EWOULDBLOCK:
#elif defined(WIN32)
						case EWOULDBLOCK:
#endif
							return;
#if defined(__linux__)
						case EMFILE:
							{
								GH_INFO("accept socket failed : EMFILE");
                                GH_ASSERT(false);
								return;
							}break;
#endif
				}
				std::ostringstream strm;
				strm<<"accept failed with error code:"<<nErrorCode<<"."<<std::ends;
				GH_INFO(strm.str().c_str());
                continue;
			}
			m_pFuncOnAccept(m_pEventReactor, NewSocket);
		}
	}

	bool listener::Listen(CNetAddr& addr, ListenerFuncOnAccept pFunc)
	{
		m_ListenAddress.SetAddress(addr.GetAddress());
		m_ListenAddress.SetPort(addr.GetPort());
		m_Socket.Open();//��ʼ��socket
		m_Socket.Bind(m_ListenAddress);
		m_Socket.SetNonblocking();
		if(listen(m_Socket.GetSystemSocket(),SOMAXCONN))
		{
			std::ostringstream strm;
			strm<<"listen failed with error code:"<<PpeGetLastError()<<"."<<std::ends;
			GH_INFO(strm.str().c_str());
		}
		GetReactor()->Register(this);//ע���reactor
		m_pFuncOnAccept = pFunc;
		return true;
	}
}