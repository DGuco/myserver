//
//  event_reactor.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
//


#ifndef _EVENT_REACTOR_H_
#define _EVENT_REACTOR_H_

#include "net_inc.h"
#include "network_interface.h"
#include "timer_event.h"
#include <string>

using namespace std;

class CEventReactor: public IEventReactor
{
public:
	//���캯��
	CEventReactor();
	//��������
	virtual ~CEventReactor();
	//��ʼ��event_base
	void Init();
	//ע�ᴦ��
	bool Register(IReactorHandler *pHandler);
	//��ȡlibevent event_base
	event_base *GetEventBase();
	//event�����ַ��¼�
	void DispatchEvents();
	//�ͷ���Դ
	void Release();
private:
	event_base *m_pEventBase;
	event_config *m_pConfig;
};

#endif
