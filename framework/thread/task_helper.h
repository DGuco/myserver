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

template<typename ResType>
struct ReturnHolder
{
	ResType value;
};

template<>
struct ReturnHolder<void>
{};

template<typename return_type, typename Par, class Func>
struct TaskCreater
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CWithReturnTask<Func,Par>>(scheduler, signature, f);
	}
};

template<typename Par, class Func>
struct TaskCreater<void, Par, Func>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CNoReturnTask<Func,Par>>(scheduler, signature, f);
	}
};

template<typename return_type, class Func>
struct TaskCreater<return_type, void, Func>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CWithReturnTask<Func, void>>(scheduler, signature, f);
	}
};

template<class Func>
struct TaskCreater<void, void, Func>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CNoReturnTask<Func,void>>(scheduler, signature, f);
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

	//template<class Scheduler,class Func, class... Args, typename return_type = std::result_of<Func(Args...)>::type>
	/*lambda [a,b]() {},a,b作为捕获的参数列表并不是函数的参数在std::result_of<Func(Args...)>编译时，推导的Args...类型
	实际上是空 在C++11中，std::result_of<F(Args...)>::type用于推导出调用F类型的函数对象实例并传递Args类型的参数后
	的返回类型。std::result_of<F(Args...)>::type无法推导出[](int param) {}这个有参数的lambda表达式，是因为没有提
	供参数类型。在使用std::result_of时，需要提供函数的参数类型，如std::result_of<decltype(lambda)(int)>::type，
	这样std::result_of就能正确推导出有参数的lambda表达式的返回类型了*/
	template<class Scheduler,class Func,typename return_type = std::result_of<Func(Res)>::type>
	CTaskHelper<return_type> ThenAccept(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		std::shared_ptr<CThreadTask> pChildTask = TaskCreater<return_type, Res,Func>::CreateTask(scheduler, "ChildTask", func);
		//如果前置任务已完成
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone)
		{
			m_pTaskPtr->ExecuteChildTask(pChildTask);
		}else
		{
			m_pTaskPtr->AddChildTask(pChildTask);
		}
		return CTaskHelper<return_type>(pChildTask);
	}

	template<class Scheduler, class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> ThenApply(CSafePtr<Scheduler> scheduler, Func&& func)
	{
		std::shared_ptr<CThreadTask> pTask = TaskCreater<return_type, void, Func>::CreateTask(scheduler, "ChildTask", func);
		//如果前置任务已完成
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone)
		{
			m_pTaskPtr->ExecuteChildTask(pChildTask);
		}
		else
		{
			m_pTaskPtr->AddChildTask(pChildTask);
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	TaskPtr m_pTaskPtr;
};

#endif //__TASK_HELPER_H__
