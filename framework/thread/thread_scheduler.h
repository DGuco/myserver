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
	template<class Func, class... Args>
	CTaskHelper Schedule(std::string signature, Func&& f, Args&&... args);
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

template<class Func, class... Args>
CTaskHelper CThreadScheduler::Schedule(std::string signature, Func&& f, Args&&... args)
{
	using return_type = typename std::result_of<Func(Args...)>::type;
	TaskPtr pTask = TaskCreater<return_type, Func, Args...>::CreateTask(this, signature, NULL,NULL,f, args...);
	std::lock_guard<std::mutex> guard(m_queue_mutex);
	m_Tasks.push(pTask);
	return CTaskHelper(pTask);
}

#endif //__THREAD_SCHEDULER_H__

