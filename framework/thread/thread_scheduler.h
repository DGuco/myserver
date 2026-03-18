/*****************************************************************
* FileName:thread_scheduler.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __THREAD_SCHEDULER_H__
#define __THREAD_SCHEDULER_H__

#include <queue>
#include <thread>
#include <functional>
#include <list>
#include "safe_pointer.h"
#include "task.h"
#include "task_helper.h"
#include "time_helper.h"
#include "my_thread.h"
#include "my_lock.h"
#include "task_scheduler.h"

class CThreadScheduler : public CTaskScheduler
{
public:
	CThreadScheduler(std::string signature);
	virtual ~CThreadScheduler();
	bool Init(size_t threads,
					ThreadFuncParam initFunc = NULL,
					ThreadFuncParam tickFunc= NULL,
					void**		    initFuncArgs = NULL,
					void**		    tickFuncArgs = NULL);
	//
	int  ThreadCount() { return m_Workers.size(); }
	//
	virtual void ScheduleTask(TaskPtr pTask);
public:
	void StopScheduler();
	void Join(); 	
private:
	std::vector<CSafePtr<CMyThread>> m_Workers;
	bool 							 stop;
	//std::condition_variable condition;
};

#endif //__THREAD_SCHEDULER_H__
