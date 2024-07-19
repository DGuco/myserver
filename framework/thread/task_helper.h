/*****************************************************************
* FileName:task_helper.h
* Summary :
* Date	  :2024-7-15
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TASK_HELPER_H__
#define __TASK_HELPER_H__

#include "safe_pointer.h"
#include "thread_task.h"

template<typename return_type, class Func, typename... Args>
struct TaskCreater
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		TaskPtr parent,
		TaskPtr child,
		const Func f,
		Args...args)
	{
		return std::make_shared<CWithReturnTask<Func, Args...>>(scheduler, signature, parent, f, args...);
	}
};

template<class Func, typename... Args>
struct TaskCreater<void, Func, Args...>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		TaskPtr parent,
		TaskPtr child,
		const Func f,
		Args...args)
	{
		return std::make_shared<CNoReturnTask<Func, Args...>>(scheduler, signature, parent, f, args...);
	}
};

class CTaskHelper
{
public:
	CTaskHelper(TaskPtr ptr) : m_pTaskPtr (ptr)
	{
	}

	~CTaskHelper() 
	{
	}
	template<class Scheduler, class Func, class... Args>
	CTaskHelper ThenApply(CSafePtr<Scheduler> scheduler,
							std::string signature,
							Func&& f,
							Args&&... args);
private:
	TaskPtr m_pTaskPtr;
};

template<class Scheduler,class Func, class... Args>
CTaskHelper CTaskHelper::ThenApply(CSafePtr<Scheduler> scheduler,
									std::string signature,
									Func&& f,
									Args&&... args)
{
	using return_type = typename std::result_of<Func(Args...)>::type;
	std::shared_ptr<CThreadTask> pTask = TaskCreater<return_type, Func, Args...>::CreateTask(scheduler, signature, m_pTaskPtr, f, args...);
	scheduler->PushTask(pTask);
	return CTaskHelper(pTask);
}
#endif //__TASK_HELPER_H__
