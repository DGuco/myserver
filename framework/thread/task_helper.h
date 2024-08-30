/*****************************************************************
* FileName:task_helper.h
* Summary :
* Date	  :2024-7-15
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TASK_HELPER_H__
#define __TASK_HELPER_H__
#include "my_assert.h"
#include "safe_pointer.h"
#include "thread_task.h"

template<size_t NUM_PARAMS, typename return_type, typename... Args>
struct TaskCaller
{
};

template<typename return_type>
struct TaskCaller<0, return_type>
{
	using function_type = typename std::function<return_type()>;
public:
	static return_type invoke(function_type func)
	{
		return func();
	}
};

template<typename return_type, typename Arg>
struct TaskCaller<1, return_type, Arg>
{
	using function_type = typename std::function<return_type(Arg)>;
public:
	static return_type invoke(function_type func, Arg arg)
	{
		return func(arg);
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<2, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<3, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<4, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<5, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<6, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<7, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<8, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args));
		return func(arg);
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<9, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<10, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	static return_type invoke(function_type func, std::tuple<Args...>& args)
	{
		return func(std::get<0>(args),
			std::get<1>(args),
			std::get<2>(args),
			std::get<3>(args),
			std::get<4>(args),
			std::get<5>(args),
			std::get<6>(args),
			std::get<7>(args),
			std::get<8>(args),
			std::get<9>(args));
	}
};

template<typename return_type, class Func,typename ...Args>
struct CombineTaskCreater
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CWithReturnTask<Func, Args...>>(scheduler, signature, f);
	}
};

template<class Func, typename ...Args>
struct CombineTaskCreater<void,Func,Args...>
{
	static TaskPtr CreateTask(CSafePtr<CThreadScheduler> scheduler,
								std::string signature,
								const Func f)
	{
		return std::make_shared<CNoReturnTask<Func, Args...>>(scheduler, signature, f);
	}
};

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
		std::string signature = m_pTaskPtr->GetSignature() + "_ThenAccept";
		std::shared_ptr<CThreadTask> pChildTask = TaskCreater<return_type, Res,Func>::CreateTask(scheduler, signature, func);
		//如果前置任务已完成
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone 
			|| m_pTaskPtr->GetState() == enTaskState::eTaskFailed)
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
		std::string signature = m_pTaskPtr->GetSignature() + "_ThenApply";
		std::shared_ptr<CThreadTask> pChildTask = TaskCreater<return_type, void, Func>::CreateTask(scheduler, signature, func);
		//如果前置任务已完成
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone
			|| m_pTaskPtr->GetState() == enTaskState::eTaskFailed)
		{
			m_pTaskPtr->ExecuteChildTask(pChildTask);
		}
		else
		{
			m_pTaskPtr->AddChildTask(pChildTask);
		}
		return CTaskHelper<return_type>(pChildTask);
	}

	TaskPtr GetTask()
	{
		return m_pTaskPtr;
	}
private:
	TaskPtr m_pTaskPtr;
};

template<class Func, class ...Args>
struct ReturnHolder
{
};

// Pseudo-void type: it takes up no space but can be moved and copied
// 伪空类型：它不占用空间，但可以移动和复制
struct fake_void
{};

template<typename Par1>
struct ParmHolder
{
	typedef Par1 type;
};

template<>
struct ParmHolder<void>
{
	typedef fake_void type;
};

template<class... ParamList>
class CAcceptCombineTaskHelper
{
	enum
	{
		//参数个数
		arity = sizeof...(ParamList)
	};
	using ParamTypeElement = typename std::tuple<ParamList...>;
	//每个参数的类型
	template<size_t I>
	struct args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, ParamTypeElement>::type;
	};
public:
	CAcceptCombineTaskHelper(CSafePtr<CThreadScheduler>	scheduler,std::vector<TaskPtr> taskList)
	{
		m_pScheduler = scheduler;
		m_TaskList = taskList;
	}

	~CAcceptCombineTaskHelper()
	{
	}

	template<class Func, typename return_type = std::result_of<Func(ParamList...)>::type>
	CTaskHelper<return_type> AcceptAll(Func&& func)
	{
		ASSERT(m_TaskList.size() > 0 && m_TaskList.size() < UCHAR_MAX);
		std::string signature = m_TaskList[0]->GetSignature() + "_AcceptAll";
		std::shared_ptr<CThreadTask> pTask = CombineTaskCreater<return_type,Func,ParamList...>::CreateTask(scheduler, signature, func);
		BYTE index = 0;
		pTask->SetCombineCount(m_TaskList.size());
		for (auto pChild : m_TaskList)
		{
			pChild->SetCombineTask(pTask);
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	CSafePtr<CThreadScheduler>	m_pScheduler;
	std::vector<TaskPtr>		m_TaskList;
};

class CApplyCombineTaskHelper
{
public:
	CApplyCombineTaskHelper(CSafePtr<CThreadScheduler>	scheduler, std::vector<TaskPtr> taskList)
	{
		m_pScheduler = scheduler;
		m_TaskList = taskList;
	}

	~CApplyCombineTaskHelper()
	{}

	template<class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> ApplyAll(Func&& func)
	{
		ASSERT(m_TaskList.size() > 0 && m_TaskList.size() < UCHAR_MAX);
		std::string signature = m_TaskList[0]->GetSignature() + "_ApplyAll";
		std::shared_ptr<CThreadTask> pTask = CombineTaskCreater<return_type, Func, void>::CreateTask(m_pScheduler, signature, func);
		pTask->SetCombineCount(m_TaskList.size());
		for (auto pChild : m_TaskList)
		{
			pChild->SetCombineTask(pTask);
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	CSafePtr<CThreadScheduler>	m_pScheduler;
	std::vector<TaskPtr>		m_TaskList;
};

#endif //__TASK_HELPER_H__
