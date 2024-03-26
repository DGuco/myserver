//
//  timer_signal.h
//  Created by DGuco on 18-1-7.
//  �źź�timer����
//

#ifndef _PPE_SIGNAL_H_
#define _PPE_SIGNAL_H_

#include <signal.h>
#include <event2/event.h>
#include "network_interface.h"

class CSystemSignal: public IReactorHandler
{
public:
	//���캯��
	explicit CSystemSignal(IEventReactor *pReactor,uint32 uSignal, FuncOnSignal pFunc, void *pContext);
	//��������
	~CSystemSignal() override;
	//�����źŻص�
	void RegisterSignal();
private:
	//ע��
	bool RegisterToReactor() override;
	//��ȡevent_base
	IEventReactor *GetReactor() override;
	//�յ��ź�
	void OnSignalReceive();
	//�źŻص�
	static void lcb_OnSignal(int fd, short event, void *arg);
private:
	FuncOnSignal m_pFuncOnSignal;
	void *m_pContext;
	IEventReactor *m_pReactor;
	event m_event;
	int m_iSignal;
};

#endif
