/*****************************************************************
* FileName:thread_scheduler.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __THREAD_SCHEDULER_H__
#define __THREAD_SCHEDULER_H__

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <list>
#include <safe_pointer.h>
#include "my_lock.h"
#include "my_thread.h"
#include "thread_task.h"
#include "task_helper.h"
#include "time_helper.h"

class CThreadScheduler;
class CTaskThread : public CMyThread
{
public:
	CTaskThread(CSafePtr<CThreadScheduler> scheduler);
	virtual ~CTaskThread();
	virtual bool			PrepareToRun();
	virtual void			Run();
private:
	CSafePtr<CThreadScheduler>	m_pScheduler;
};

class CThreadScheduler 
{
public:
	CThreadScheduler(std::string signature);
	~CThreadScheduler();
	bool Init(size_t threads);
	template<class Func,typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> Schedule(std::string signature,Func&& f);
	void PushTask(TaskPtr pTask);
	void ConsumeTask();
	void DebugTask();
private:
	std::vector<CSafePtr<CTaskThread>> m_Workers;
	std::queue<TaskPtr> m_Tasks;
	CMyLock		m_queue_mutex;
	std::list<TaskPtr> m_WaitingTasks;
	std::string m_Signature;	//任务签名
	CMyTimer	debug_timer;	//线程详情debug timer
	//std::condition_variable condition;
	bool stop;
};

template<class Func,typename return_type>
CTaskHelper<return_type> CThreadScheduler::Schedule(std::string signature,Func&& f)
{
	TaskPtr pTask = TaskCreater<return_type,void,Func>::CreateTask(this, signature,f);
	std::lock_guard<std::mutex> guard(m_queue_mutex);
	m_Tasks.push(pTask);
	return CTaskHelper<return_type>(pTask);
}

class CSingleThreadScheduler : public CThreadScheduler
{
public:
	CSingleThreadScheduler(std::string signature) 
		: CThreadScheduler(signature)
	{
		Init(1);
	}
	CSingleThreadScheduler() : 
		CThreadScheduler("SingleThreadScheduler")
	{
		Init(1);
	}
};

#endif //__THREAD_SCHEDULER_H__

