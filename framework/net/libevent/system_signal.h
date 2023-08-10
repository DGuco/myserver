//
//  timer_signal.h
//  Created by DGuco on 18-1-7.
//  信号和timer管理
//

#ifndef _PPE_SIGNAL_H_
#define _PPE_SIGNAL_H_

#include <signal.h>
#include <event2/event.h>
#include "network_interface.h"

class CSystemSignal: public IReactorHandler
{
public:
	//构造函数
	explicit CSystemSignal(IEventReactor *pReactor,uint32 uSignal, FuncOnSignal pFunc, void *pContext);
	//析构函数
	~CSystemSignal() override;
	//设置信号回调
	void RegisterSignal();
private:
	//注册
	bool RegisterToReactor() override;
	//获取event_base
	IEventReactor *GetReactor() override;
	//收到信号
	void OnSignalReceive();
	//信号回调
	static void lcb_OnSignal(int fd, short event, void *arg);
private:
	FuncOnSignal m_pFuncOnSignal;
	void *m_pContext;
	IEventReactor *m_pReactor;
	event m_event;
	int m_iSignal;
};

#endif
