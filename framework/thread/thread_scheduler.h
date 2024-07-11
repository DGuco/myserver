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
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <safe_pointer.h>
#include "my_thread.h"
#include "thread_task.h"
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
	bool Init(size_t threads);
	template<class Func, class... Args>
	void Schedule(std::string signature, Func&& f, Args&&... args);
	~CThreadScheduler();
	void ConsumeTask();
	void DebugTask();
private:
	std::vector<CSafePtr<CTaskThread>> m_Workers;
	std::queue<std::shared_ptr<CThreadTask>> m_Tasks;
	std::mutex	m_queue_mutex;
	std::string m_Signature;	//����ǩ��
	CMyTimer	debug_timer;	//�߳�����debug timer
	//std::condition_variable condition;
	bool stop;
};

template<class Func, class... Args>
void CThreadScheduler::Schedule(std::string signature, Func&& f, Args&&... args)
{
	using return_type = typename std::result_of<Func(Args...)>::type;
	std::shared_ptr<CThreadTask> pTask = TaskCreater<return_type, Func, Args...>::CreateTask(signature,f,args...);
	std::lock_guard<std::mutex> guard(m_queue_mutex);
	m_Tasks.emplace(pTask);
}

#endif //__THREAD_SCHEDULER_H__

