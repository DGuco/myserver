//
// Created by dguco on 18-2-8.
//

#include "timer_event.h"

int CTimerEvent::m_lcNnxtTimerId = 0;

CTimerEvent::CTimerEvent(IEventReactor *pReactor,
						 FuncOnTimeOut pFuncOnTimerOut,
						 void *param,
						 int sec,
						 int usec,
						 int loopTimes)
	: m_pReactor(pReactor),
	  m_pFuncOnTimerOut(pFuncOnTimerOut),
	  m_pParam(param),
	  m_iSec(sec),
	  m_iUsec(usec),
	  m_iLoopTimes(loopTimes),
	  m_iTimerId(GetNextTimerId())
{
	GetReactor()->Register(this);
}

CTimerEvent::~CTimerEvent()
{
	Cancel();
}

void CTimerEvent::LaterCall(int sec, int usec)
{
	struct timeval tv;
	tv.tv_sec = sec;    // 秒
	tv.tv_usec = usec;  // 微秒
	evtimer_assign(&m_event, GetReactor()->GetEventBase(), &lcb_TimeOut, (void *) this);
	evtimer_add(&m_event, &tv);
}

void CTimerEvent::ReCall(int sec, int usec)
{
	evtimer_del(&m_event);
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	evtimer_assign(&m_event, GetReactor()->GetEventBase(), &lcb_TimeOut, (void *) this);
	evtimer_add(&m_event, &tv);
}

void CTimerEvent::Cancel()
{
	evtimer_del(&m_event);
}

void CTimerEvent::OnTimerOut(int fd, short event)
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

void CTimerEvent::lcb_TimeOut(int fd, short event, void *arg)
{
	CTimerEvent *pTimer = static_cast<CTimerEvent *>(arg);
	pTimer->OnTimerOut(fd, event);
}

int CTimerEvent::GetNextTimerId()
{
	return m_lcNnxtTimerId++;
}

void CTimerEvent::setM_lcNnxtTimerId(int m_lcNnxtTimerId)
{
	CTimerEvent::m_lcNnxtTimerId = m_lcNnxtTimerId;
}

bool CTimerEvent::RegisterToReactor()
{
	LaterCall(m_iSec, m_iUsec);
	return true;
}

bool CTimerEvent::UnRegisterFromReactor()
{
	Cancel();
}
IEventReactor *CTimerEvent::GetReactor()
{
	return m_pReactor;
}


