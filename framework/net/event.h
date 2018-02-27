//
// Created by dguco on 18-2-8.
// 基于libevet的timer事件
//

#ifndef SERVER_TIMER_EVENT_H
#define SERVER_TIMER_EVENT_H

#include "network_interface.h"
#include <event.h>

class CEvent: public IReactorHandler
{
public:
	//构造函数
	CEvent(IEventReactor *pReactor,
		   FuncOnTimeOut m_pFuncOnTimerOut,
		   void *param,
		   int sec,
		   int usec,
		   int loopTimes,
		   int fd = -1,
		   int eventType = -1);
	//析构函数
	~CEvent() override;
	//超时处理
	void OnTimerOut(int fd, short event);
	void LaterCall(int sec, int usec);
	void ReCall(int sec, int usec);
	//停止
	void Cancel();
private:
	//注册
	bool RegisterToReactor() override;
	//卸载
	bool UnRegisterFromReactor() override;
	//获取event_base
	IEventReactor *GetReactor() override;
private:
	//超时回调
	static void lcb_TimeOut(int fd, short event, void *arg);
	static int GetNextTimerId();
	static void setM_lcNnxtTimerId(int m_lcNnxtTimerId);
private:
	IEventReactor *m_pReactor;
	FuncOnTimeOut m_pFuncOnTimerOut;
	void *m_pParam; //超时回调参数
	int m_iSec;  //秒
	int m_iUsec; //微妙
	int m_iLoopTimes; //循环次数 0:forever > 0 次数
	int m_iFd;
	int m_iEventType;
	event m_event;
};

#endif //SERVER_TIMER_EVENT_H
