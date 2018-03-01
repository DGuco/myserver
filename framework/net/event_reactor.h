//
//  event_reactor.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _EVENT_REACTOR_H_
#define _EVENT_REACTOR_H_

#include "net_inc.h"
#include "network_interface.h"
#include "sys_event.h"
#include <string>

using namespace std;

enum eNetModule
{
	NET_SYSTEM = -1, //由系统决定
	NET_SELECT = 0,
	NET_POLL = 1,
	NET_EPOLL = 2,
	NET_KQUEUE = 3, //mac os
	NET_INVALID = 4,
};

class CEventReactor: public IEventReactor
{
public:
	//构造函数
	CEventReactor(eNetModule netModule);
	//析构函数
	virtual ~CEventReactor();
	//初始化event_base
	void Init(eNetModule netModule);
	//注册处理
	bool Register(IReactorHandler *pHandler);
	//删除处理
	bool UnRegister(IReactorHandler *pHandler);
	//获取libevent event_base
	event_base *GetEventBase();
	//event监听分发事件
	void DispatchEvents();
	//释放资源
	void Release();

private:
	uint32 m_uReactorHandlerCounter;
	event_base *m_pEventBase;
	event_config *m_pEventConfig;

private:
	string NET_MODULE[4] = {"select", "poll", "epoll", "kqueue" /*mac os*/};
};

#endif
