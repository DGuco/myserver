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
	CThreadScheduler(std::string signature,size_t);
	template<class Func, class... Args>
	void Schedule(std::string signature, Func&& f, Args&&... args);
	~CThreadScheduler();
	void ConsumeTask();
	void DebugTask();
private:
	std::vector<CSafePtr<CTaskThread>> m_Workers;
	std::queue<std::shared_ptr<CThreadTask>> m_Tasks;
	std::mutex	m_queue_mutex;
	std::string m_Signature;	//任务签名
	CMyTimer	debug_timer;	//线程详情debug timer
	//std::condition_variable condition;
	bool stop;
};

#endif //__THREAD_SCHEDULER_H__

