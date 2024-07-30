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
		const Func f,
		Args...args)
	{
		return std::make_shared<CWithReturnTask<Func, Args...>>(scheduler, signature, f, args...);
	}
};

template<class Func, typename... Args>
struct TaskCreater<void, Func, Args...>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func f,
		Args...args)
	{
		return std::make_shared<CNoReturnTask<Func, Args...>>(scheduler, signature, f, args...);
	}
};

template<typename Res>
class CTaskHelper
{
public:
	CTaskHelper(TaskPtr ptr) : m_pTaskPtr (ptr)
	{
	}

	~CTaskHelper() 
	{
	}
	template<class Scheduler,class Func, class... Args, typename return_type = std::result_of<Func(Args...)>::type>
	CTaskHelper<return_type> ThenApply(CSafePtr<Scheduler> scheduler,Func&& func, Args&&... args)
	{
		std::shared_ptr<CThreadTask> pTask = TaskCreater<return_type, Func, Args...>::CreateTask(scheduler, "ChildTask", func, args...);
		//如果前置任务已完成
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone)
		{
			using function_type = typename std::function<return_type(Res)>;
			//执行scheduler为同一个直接执行
			if (scheduler == m_pTaskPtr->GetScheduler())
			{
				function_type callFunc = func;
				void* pRes = m_pTaskPtr->GetResult();
				if (pRes != NULL)
				{
					return_type resValue = *(return_type*)pRes;
					func(resValue);
				}
			}
			else
			{

			}
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	TaskPtr m_pTaskPtr;
};
#endif //__TASK_HELPER_H__
