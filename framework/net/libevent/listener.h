//
//  CListener.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef _LISTENER_H
#define _LISTENER_H

#include "event_reactor.h"
#include "net_addr.h"
#include "socket.h"
#include "network_interface.h"
#include <timer_event.h>
#include <event2/listener.h>

enum eListenerState
{
	eLS_UnListen = 0,
	eLS_Listened,
};

class CListener: public IReactorHandler
{
public:
	//���캯��
	CListener(IEventReactor *pReactor, int listenQueue);
	//��������
	virtual ~CListener(void);
	//����
	bool Listen(CNetAddr &addr, FuncListenerOnAccept pFunc);
	//��ȡevent_base
	IEventReactor *GetReactor();
	//�ر�
	void ShutDown();
	//�Ƿ������
	bool IsListened();

private:
	//����״̬
	void SetState(eListenerState eState);
	//ע��
	bool RegisterToReactor();
	//ж��
	bool UnRegisterFromReactor();
	//�����ص�
	static void lcb_Listen(struct evconnlistener *listener,
						   evutil_socket_t fd,
						   struct sockaddr *sa,
						   int socklen, void *arg);
	//��������
	static void lcb_AcceptError(struct evconnlistener *listener, void *ctx);
	//�������
	void HandleInput(int Socket, struct sockaddr *sa);
private:
	CNetAddr m_ListenAddress;
	event m_event;

	IEventReactor *m_pEventReactor;
	eListenerState m_eState;
	FuncListenerOnAccept m_pFuncOnAccept;
	int m_iListenQueueMax;
	struct evconnlistener *m_pListener;
};
#endif
