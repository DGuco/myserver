//
// Created by dguco on 18-2-8.
//

#include "sys_event.h"

CSysEvent::CSysEvent(IEventReactor *pReactor,
			   FuncOnTimeOut pFuncOnTimerOut,
			   void *param,
			   int sec,
			   int usec,
			   int loopTimes,
			   int fd,
			   int eventType)
	: m_pReactor(pReactor),
	  m_pFuncOnTimerOut(pFuncOnTimerOut),
	  m_pParam(param),
	  m_iSec(sec),
	  m_iUsec(usec),
	  m_iLoopTimes(loopTimes),
	  m_iFd(fd),
	  m_iEventType(eventType)
{
	GetReactor()->Register(this);
}

CSysEvent::~CSysEvent()
{
	Cancel();
}

void CSysEvent::LaterCall(int sec, int usec)
{
	struct timeval tv;
	tv.tv_sec = sec;    // 秒
	tv.tv_usec = usec;  // 微秒
	//如果时socket事件
	if (m_iFd > 0) {
		event_set(&m_event, m_iFd, m_iEventType, &lcb_TimeOut, m_pParam);
		event_base_set(GetReactor()->GetEventBase(), &m_event);
		event_add(&m_event, &tv);
	}
	else { //timer 事件
		evtimer_assign(&m_event, GetReactor()->GetEventBase(), &lcb_TimeOut, m_pParam);
		evtimer_add(&m_event, &tv);
	}
}

void CSysEvent::ReCall(int sec, int usec)
{
	evtimer_del(&m_event);
	LaterCall(sec, usec);
}

void CSysEvent::Cancel()
{
	evtimer_del(&m_event);
}

void CSysEvent::OnTimerOut(int fd, short event)
{
	m_pFuncOnTimerOut(fd, event, m_pParam);
	if (m_iLoopTimes == 0) {
		LaterCall(m_iSec, m_iUsec);
	}
	else {
		if (m_iLoopTimes > 1) {
			LaterCall(m_iSec, m_iUsec);
			m_iLoopTimes--;
		}
	}
}

void CSysEvent::lcb_TimeOut(int fd, short event, void *arg)
{
	CSysEvent *pTimer = static_cast<CSysEvent *>(arg);
	pTimer->OnTimerOut(fd, event);
}

bool CSysEvent::RegisterToReactor()
{
	LaterCall(m_iSec, m_iUsec);
	return true;
}

bool CSysEvent::UnRegisterFromReactor()
{
	Cancel();
}

IEventReactor *CSysEvent::GetReactor()
{
	return m_pReactor;
}




