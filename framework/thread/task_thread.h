#ifndef TASK_THREAD_H
#define TASK_THREAD_H
#include "my_thread.h"
#include "task_scheduler.h"

class CTaskThread : public CMyThread
{
public:
	CTaskThread(CSafePtr<CTaskScheduler> scheduler);
	virtual ~CTaskThread();
	virtual bool PrepareToRun();
	virtual bool PrepareEnd();
	virtual void Run();
private:
	CSafePtr<CTaskScheduler>	m_pScheduler;
};

#endif