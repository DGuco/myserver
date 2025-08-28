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

#define THREAD_TASK_DEBUG_TIME (20 *1000)   //任务队列调试时间间隔
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
	void DebugTask();
	//
	int  ThreadCount() { return m_Workers.size(); }
	//
	virtual void ScheduleTask(TaskPtr pTask);
public:
	void StopScheduler();
	void Join(); 	
private:
	std::vector<CSafePtr<CMyThread>> m_Workers;
	CMyTimer						 debug_timer;	//线程详情debug timer
	bool 							 stop;
	//std::condition_variable condition;
};

#endif //__THREAD_SCHEDULER_H__
