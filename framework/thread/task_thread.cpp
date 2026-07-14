#include "task_thread.h"

CTaskThread::CTaskThread(CSafePtr<CTaskScheduler> scheduler)
	: m_pScheduler(scheduler)
{

}

CTaskThread::~CTaskThread()
{}

bool CTaskThread::PrepareToRun()
{
	g_thread_data.own_scheduler = m_pScheduler;
	m_funcInit();
	return true;
}

bool CTaskThread::PrepareEnd()
{
	return true;
}

void CTaskThread::Run()
{
	while (!IsStoped())
	{
		//更新线程的缓存时间
		CTimeHelper::GetSingletonPtr()->SetTime();
		m_funcTick();
		m_pScheduler->ConsumeTask();
		//
		SLEEP(1);
	}
}