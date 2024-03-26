//
// Created by dguco on 18-2-8.
// ����libevet��timer�¼�
//

#ifndef SERVER_SYS_EVENT_H
#define SERVER_SYS_EVENT_H

#include <event2/event.h>
#include <event2/event_struct.h>
#include <memory>
#include "network_interface.h"

class CTimerEvent: public IReactorHandler
{
public:
	//���캯��
	CTimerEvent(IEventReactor *pReactor,
				FuncOnTimeOut m_pFuncOnTimerOut,
				void *param,
				int sec,
				int usec,
				int loopTimes/*-1 ���� > 0 ����*/);
	//��������
	~CTimerEvent();
	//��ʱ����
	void OnTimerOut(int fd, short event);
	void LaterCall(int sec, int usec);
	void ReCall(int sec, int usec);
	//ֹͣ
	void Cancel();
private:
	//ע��
	bool RegisterToReactor();
	//��ȡevent_base
	IEventReactor *GetReactor();
private:
	//��ʱ�ص�
	static void lcb_TimeOut(int fd, short event, void *arg);
private:
	IEventReactor *m_pReactor;
	FuncOnTimeOut m_pFuncOnTimerOut;
	void *m_pParam; //��ʱ�ص�����l
	int m_iSec;  //��
	int m_iUsec; //΢��
	int m_iLoopTimes; //ѭ������
	event m_event;
};

#endif //SERVER_TIMER_EVENT_H
