//
//  CListener.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LISTENER_H
#define _LISTENER_H

#include "event_reactor.h"
#include "net_addr.h"
#include "socket.h"
#include "network_interface.h"
#include <event.h>
#include <event2/listener.h>

enum eListenerState
{
	eLS_UnListen = 0,
	eLS_Listened,
};

class CListener: public IReactorHandler
{
public:
	//构造函数
	CListener(IEventReactor *pReactor);
	//析构函数
	virtual ~CListener(void);
	//监听
	bool Listen(CNetAddr &addr, FuncListenerOnAccept pFunc);
	//获取event_base
	IEventReactor *GetReactor();
	//关闭
	void ShutDown();
	//是否监听中
	bool IsListened();

private:
	//设置状态
	void SetState(eListenerState eState);
	//注册
	bool RegisterToReactor();
	//卸载
	bool UnRegisterFromReactor();
	//监听回调
	static void lcb_Accept(struct evconnlistener *listener,
						   evutil_socket_t fd,
						   struct sockaddr *sa,
						   int socklen, void *arg);
	//处理监听
	void HandleInput(int Socket, struct sockaddr *sa);
private:
	FuncListenerOnAccept m_pFuncOnAccept;
	eListenerState m_eState;
	CNetAddr m_ListenAddress;
	IEventReactor *m_pEventReactor;
	struct evconnlistener *m_pListener;
	CSocket m_Socket;
	event m_event;
};
#endif
