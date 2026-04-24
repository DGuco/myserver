/*****************************************************************
* FileName:task_helper.h
* Summary :
* Date	  :2024-7-15
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __TASK_HELPER_H__
#define __TASK_HELPER_H__
#include <type_traits>
#include "my_assert.h"
#include "safe_pointer.h"
#include "task.h"

class CTaskScheduler;

// 首先定义一个辅助traits模板来检查所有类型是否相同
template<typename... Args> struct are_all_same;

// 基本情况：单个类型总是返回true
template<typename T> struct are_all_same<T> : std::true_type {};

// 递归情况：检查第一个和第二个类型是否相同，然后递归检查剩余类型
template<typename T, typename U, typename... Rest>
struct are_all_same<T, U, Rest...> : 
    std::bool_constant<std::is_same_v<T, U> && are_all_same<T, Rest...>::value> {};

/*
MakeIndexSequence<5> : public MakeIndexSequence<4,4>
MakeIndexSequence<4,4> : public MakeIndexSequence<3,3,4>
MakeIndexSequence<3,3,4> : public MakeIndexSequence<2,2,3,4>
MakeIndexSequence<2,2,3,4> : public MakeIndexSequence<1,1,2,3,4>
MakeIndexSequence<1,1,2,3,4> : public MakeIndexSequence<0,0,1,2,3,4>
MakeIndexSequence<0,0,1,2,3,4> : public IndexSequence<0,1,2,3,4>
IndexSequence<0,1,2,3,4> 
*/
template <size_t... Ints>
struct IndexSequence { using type = IndexSequence; };

template <size_t N, size_t... Ints>
struct MakeIndexSequence : MakeIndexSequence<N-1, N-1, Ints...> {};

template <size_t... Ints>
struct MakeIndexSequence<0, Ints...> : IndexSequence<Ints...> {};

template<size_t NUM_PARAMS, typename return_type, typename... Args>
struct TaskCaller
{
	using function_type = typename std::function<return_type(Args...)>;
public:
	/**
	 不是通用引用：function_type&& 是具体类型的右值引用，不是通用引用（T&&），
	 所以 std::forward<function_type> 在这里没有意义,不需要完美转发
	 */
	static return_type invoke(function_type& func, std::tuple<Args...>& args) 
	{
		using Indices = typename MakeIndexSequence<sizeof...(Args)>::type;
		return invokeImpl(func, args, Indices());
	}

	template <size_t... Indices>
	static return_type invokeImpl(function_type& func, std::tuple<Args...>& args, IndexSequence<Indices...>) 
	{
		return func(std::get<Indices>(args)...);
	}
};

template<typename return_type>
struct TaskCaller<0, return_type>
{
	using function_type = typename std::function<return_type()>;
public:
	static return_type invoke(function_type& func)
	{
		return func();
	}
};

template<typename return_type, typename Arg>
struct TaskCaller<1, return_type, Arg>
{
	using function_type = typename std::function<return_type(Arg)>;
public:
	static return_type invoke(function_type& func, Arg& arg)
	{
		return func(arg);
	}
};

// template<typename return_type, typename... Args>
// struct TaskCaller<2, return_type, Args...>
// {
//     using function_type = typename std::function<return_type(Args...)>;
// public:
//     static return_type invoke(function_type func, std::tuple<Args...>& args)
//     {
//         return func(std::get<0>(args),
//             std::get<1>(args));
//     }
// };

template<int combine_count,typename return_type, class Func,typename ...Args>
struct CombineTaskCreater
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f,
								enCombineType combineType = enCombineType::eCombineAll)
	{
		return std::make_shared<CWithReturnTask<combine_count,Func, Args...>>(scheduler, signature, std::forward<Func>(f), combineType);
	}
};

template<int combine_count,class Func, typename ...Args>
struct CombineTaskCreater<combine_count,void,Func,Args...>
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f,
								enCombineType combineType = enCombineType::eCombineAll)
	{
		return std::make_shared<CNoReturnTask<combine_count,Func, Args...>>(scheduler, signature, std::forward<Func>(f), combineType);
	}
};

template<typename return_type, typename Par, class Func>
struct TaskCreater
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f)
	{
		return std::make_shared<CWithReturnTask<0,Func,Par>>(scheduler, signature, std::forward<Func>(f));
	}
};

template<typename Par, class Func>
struct TaskCreater<void, Par, Func>
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f)
	{
		return std::make_shared<CNoReturnTask<0,Func,Par>>(scheduler, signature, std::forward<Func>(f));
	}
};

template<typename return_type, class Func>
struct TaskCreater<return_type, void, Func>
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f)
	{
		return std::make_shared<CWithReturnTask<0,Func, void>>(scheduler, signature, std::forward<Func>(f));
	}
};

template<class Func>
struct TaskCreater<void, void, Func>
{
	static TaskPtr CreateTask(CSafePtr<CTaskScheduler> scheduler,
								std::string signature,
								Func&& f)
	{
		return std::make_shared<CNoReturnTask<0,Func,void>>(scheduler, signature, std::forward<Func>(f));
	}
};

template<typename Res>
class CTaskHelper
{
public:
	using ReturnType = Res;  // 添加此行用于类型推导
public:
	CTaskHelper(TaskPtr ptr) : m_pTaskPtr (ptr)
	{}
	~CTaskHelper() 
	{}
// 	CTaskHelper(const CTaskHelper&) = delete;//禁止复制构造函数	
// 	CTaskHelper& operator=(const CTaskHelper&) = delete;//禁止复制赋值运算符		
// 	CTaskHelper(CTaskHelper&&) = delete;//禁止移动构造函数	
// 	CTaskHelper& operator=(CTaskHelper&&) = delete;//禁止移动赋值运算符		

	//template<class Func, class... Args, typename return_type = std::result_of<Func(Args...)>::type>
	/*lambda [a,b]() {},a,b作为捕获的参数列表并不是函数的参数在std::result_of<Func(Args...)>编译时，推导的Args...类型
	实际上是空 在C++11中，std::result_of<F(Args...)>::type用于推导出调用F类型的函数对象实例并传递Args类型的参数后
	的返回类型。std::result_of<F(Args...)>::type无法推导出[](int param) {}这个有参数的lambda表达式，是因为没有提
	供参数类型。在使用std::result_of时，需要提供函数的参数类型，如std::result_of<decltype(lambda)(int)>::type，
	这样std::result_of就能正确推导出有参数的lambda表达式的返回类型了*/
	template<class Scheduler,class Func,typename return_type = std::result_of<Func(Res)>::type>
	CTaskHelper<return_type> ThenAccept(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		std::string signature = m_pTaskPtr->GetSignature() + "_ThenAccept";
		std::shared_ptr<CTask> pChildTask = TaskCreater<return_type, Res,Func>::CreateTask(scheduler.Get(), signature, std::forward<Func>(func));
		m_pTaskPtr->AddChildTask(pChildTask);
		//有可能子任务添加到队列之前，前置任务就已经完成了，后续任务没有执行，再次尝试执行
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone 
			|| m_pTaskPtr->GetState() == enTaskState::eTaskFailed)
		{ 
			m_pTaskPtr->RunChildTask();
		}
		return CTaskHelper<return_type>(pChildTask);
	}

	template<class Scheduler,class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> ThenApply(CSafePtr<Scheduler> scheduler, Func&& func)
	{
		std::string signature = m_pTaskPtr->GetSignature() + "_ThenApply";
		std::shared_ptr<CTask> pChildTask = TaskCreater<return_type, void, Func>::CreateTask(scheduler.Get(), signature, std::forward<Func>(func));
		m_pTaskPtr->AddChildTask(pChildTask);
		//有可能子任务添加到队列之前，前置任务就已经完成了，后续任务没有执行，再次尝试执行
		if (m_pTaskPtr->GetState() == enTaskState::eTaskDone
			|| m_pTaskPtr->GetState() == enTaskState::eTaskFailed)
		{
			m_pTaskPtr->RunChildTask();
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

template<class... Args>
class CAcceptCombineTaskHelper
{
	enum
	{
		//参数个数
		arity = sizeof...(Args)
	};
	using ParamTypeElement = typename std::tuple<Args...>;
	//每个参数的类型
	template<size_t I>
	struct args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, ParamTypeElement>::type;
	};
public:
	CAcceptCombineTaskHelper(std::vector<TaskPtr> taskList)
	{
		m_TaskList = taskList;
	}

	~CAcceptCombineTaskHelper()
	{}

// 	CAcceptCombineTaskHelper(const CAcceptCombineTaskHelper&) = delete;//禁止复制构造函数	
// 	CAcceptCombineTaskHelper& operator=(const CAcceptCombineTaskHelper&) = delete;//禁止复制赋值运算符		
// 	CAcceptCombineTaskHelper(CAcceptCombineTaskHelper&&) = delete;//禁止移动构造函数	
// 	CAcceptCombineTaskHelper& operator=(CAcceptCombineTaskHelper&&) = delete;//禁止移动赋值运算符		

	template<class Scheduler,class Func, typename return_type = std::result_of<Func(Args...)>::type>
	CTaskHelper<return_type> AcceptAll(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		std::string signature = m_TaskList[0]->GetSignature() + "_AcceptAll";
		std::shared_ptr<CTask> pTask = CombineTaskCreater<arity,return_type,Func,Args...>::CreateTask(scheduler.Get(),signature, std::forward<Func>(func));
		for(int index = 0; index < m_TaskList.size(); ++index)
		{
			pTask->SetCombineTask(index, m_TaskList[index]);
			m_TaskList[index]->AddChildTask(pTask);
			//有可能子任务添加到队列之前，前置任务就已经完成了，后续任务没有执行，再次尝试执行
			if (m_TaskList[index]->GetState() == enTaskState::eTaskDone
				|| m_TaskList[index]->GetState() == enTaskState::eTaskFailed)
			{
				m_TaskList[index]->RunChildTask();
			}
		}

		return CTaskHelper<return_type>(pTask);
	}

	template<class Scheduler, class Func, typename FirstArg = args<0>::type,typename return_type = std::result_of<Func(FirstArg)>::type>  
	CTaskHelper<return_type> AcceptAny(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		// 检查所有参数类型是否相同
    	static_assert(are_all_same<Args...>::value, "AcceptAny All arguments must be the same type");
		std::string signature = m_TaskList[0]->GetSignature() + "_AcceptAny";
		std::shared_ptr<CTask> pTask = CombineTaskCreater<arity,return_type,Func,FirstArg>::CreateTask(scheduler.Get(),signature, std::forward<Func>(func), enCombineType::eCombineAny);
		for(int index = 0; index < m_TaskList.size(); ++index)
		{
			pTask->SetCombineTask(index, m_TaskList[index]);
			m_TaskList[index]->AddChildTask(pTask);
			//有可能子任务添加到队列之前，前置任务就已经完成了，后续任务没有执行，再次尝试执行
			if (m_TaskList[index]->GetState() == enTaskState::eTaskDone
				|| m_TaskList[index]->GetState() == enTaskState::eTaskFailed)
			{
				m_TaskList[index]->RunChildTask();
			}
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	std::vector<TaskPtr>		m_TaskList;
};

template<int combine_count>
class CApplyCombineTaskHelper
{
public:
	CApplyCombineTaskHelper(std::vector<TaskPtr> taskList)
	{
		m_TaskList = taskList;
	}

	~CApplyCombineTaskHelper()
	{}

// 	CApplyCombineTaskHelper(const CApplyCombineTaskHelper&) = delete;//禁止复制构造函数	
// 	CApplyCombineTaskHelper& operator=(const CApplyCombineTaskHelper&) = delete;//禁止复制赋值运算符		
// 	CApplyCombineTaskHelper(CApplyCombineTaskHelper&&) = delete;//禁止移动构造函数	
// 	CApplyCombineTaskHelper& operator=(CApplyCombineTaskHelper&&) = delete;//禁止移动赋值运算符		

	template<class Scheduler,class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> ApplyAll(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		std::string signature = m_TaskList[0]->GetSignature() + "_ApplyAll";
		std::shared_ptr<CTask> pTask = CombineTaskCreater<combine_count,return_type, Func, void>::CreateTask(scheduler.Get(), signature, std::forward<Func>(func));
		for(int index = 0; index < m_TaskList.size(); ++index)
		{
			m_TaskList[index]->AddChildTask(pTask);
			pTask->SetCombineTask(index, m_TaskList[index]);
		}
		return CTaskHelper<return_type>(pTask);
	}

	template<class Scheduler,class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> ApplyAny(CSafePtr<Scheduler> scheduler,Func&& func)
	{
		std::string signature = m_TaskList[0]->GetSignature() + "_ApplyAny";
		std::shared_ptr<CTask> pTask = CombineTaskCreater<combine_count,return_type, Func, void>::CreateTask(scheduler.Get(), signature, std::forward<Func>(func), enCombineType::eCombineAny);
		for(int index = 0; index < m_TaskList.size(); ++index)
		{
			m_TaskList[index]->AddChildTask(pTask);
			pTask->SetCombineTask(index, m_TaskList[index]);
		}
		return CTaskHelper<return_type>(pTask);
	}
private:
	std::vector<TaskPtr>		m_TaskList;
};

class IArgsHolder
{
public:
	IArgsHolder() {};
	virtual ~IArgsHolder() {};
	virtual void  FillWaitTaskParm(TaskPtr pTask,TaskPtr pWaitTask) = 0;
	virtual bool  Empty() = 0;
};

template<int combineIndex,typename ...Args>
class CArgsHolder : public IArgsHolder
{
	enum
    {
        arity = sizeof...(Args)
    };

	/*
		Args... 已经支持右值引用：Args... 作为模板参数包，可以接受包括右值引用在内的任意类型。
		Args&&... 的作用：Args&&... 通常用于函数参数中，作为通用引用的参数包，用于接收任意数量
		的参数并保持它们的值类别。但在 CArgsHolder 类中，Args... 是用于定义 std::tuple 的类型，
		而不是作为函数参数，因此不需要使用 Args&&...。
		在以下情况下，您需要使用 Args&&...：
		template <typename... Args>
		void forwarder(Args&&... args) {
			function_that_needs_args(std::forward<Args>(args)...);
		}
	*/
    using ParamTypeElement = typename std::tuple<Args...>;
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, ParamTypeElement>::type;
    };
public:
	virtual void  FillWaitTaskParm(TaskPtr pParentTask,TaskPtr pChildTask)
	{
		void* pRes = pParentTask->GetRes();
		void* pArgs = pChildTask->GetArgs();
		if (pRes == NULL || pArgs == NULL)
		{
			return;
		}
		ParamTypeElement& tmArgs = *(ParamTypeElement*)(pArgs);
		using ArgType = args<combineIndex>::type;
		ArgType& tmRes = *(ArgType*)(pRes);
        // 使用 std::forward 保持值类别
        std::get<combineIndex>(tmArgs) = std::forward<ArgType>(tmRes);
	}

	virtual bool Empty()
	{
		return false;
	}
};

template<int combineIndex>
class CArgsHolder<combineIndex,void>
{
public:
	virtual void FillWaitTaskParm(TaskPtr pTask, TaskPtr pWaitTask)
	{
		return;
	}

	virtual bool  Empty()
	{
		return true;
	}
};


#endif //__TASK_HELPER_H__
