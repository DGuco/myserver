/*****************************************************************
* FileName:thread_task.h
* Summary :
* Date	  :2024-7-10
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__
#include <functional>
#include <string>
#include <tuple>
#include <memory>
#include <my_thread.h>
#include "log.h"

#define EMPTY_VOID_FUNC = []{}

template<size_t NUM_PARAMS,typename return_type, typename... Args>
struct TaskCaller
{
};

template<typename return_type, typename... Args>
struct TaskCaller<0, return_type,Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
	//using callback_function_type = typename std::function<void(bool,return_type)>;
public:
	static return_type invoke(function_type func,std::tuple<Args...>& args)
	{
		return func();
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<1, return_type, Args...>
{
	using function_type = typename std::function<return_type(Args...)>;
	//using callback_function_type = typename std::function<void(bool, return_type)>;
public:
	static return_type invoke(function_type func,std::tuple<Args...>& args)
	{
		return func(std::get<0>(args));
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

template<typename return_type, typename... Args>
struct TaskCaller<11, return_type, Args...>
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
			std::get<9>(args),
			std::get<10>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<12, return_type, Args...>
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
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<13, return_type, Args...>
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
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<14, return_type, Args...>
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
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args),
			std::get<13>(args));
	}
};

template<typename return_type, typename... Args>
struct TaskCaller<15, return_type, Args...>
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
			std::get<9>(args),
			std::get<10>(args),
			std::get<11>(args),
			std::get<12>(args),
			std::get<13>(args),
			std::get<14>(args));
	}
};


class CThreadScheduler;
class CThreadTask;

enum class enTaskState : unsigned char
{
	eTaskInit = 0,
	eTaskDoing = 1,
	eTaskDone = 2,
	eTaskFailed = 3,
};

class CThreadTask : public enable_shared_from_this<CThreadTask>
{
public:
	CThreadTask(CSafePtr<CThreadScheduler> scheduler,
				std::string signature,
				TaskPtr parentTask,
				TaskPtr childTask)
		:m_pScheduler(scheduler),
		 m_TaskSignature(signature),
		 m_pParentTask(parentTask),
		 m_pChildTask(childTask){};
	virtual ~CThreadTask() {};
	void SetStartTime(time_t time)				{ m_nExecuteStart = time; }
	void SetEndTime(time_t time)				{ m_nExecuteEnd = time; }
	time_t GetStartTime()						{ return m_nExecuteStart; }
	time_t GetEndTime()							{ return m_nExecuteEnd; }
	const std::string& GetSignature()			{ return m_TaskSignature; }
	TaskPtr GetShared()							{ return shared_from_this(); }
	CSafePtr<CThreadScheduler>   GetScheduler() { return m_pScheduler; }
	TaskPtr GetParentTask()						{ return m_pParentTask; }
	TaskPtr GetChildask()					    { return m_pChildTask.expired() ? m_pChildTask.lock() : NULL;}
	enTaskState GetState()						{ return m_nState.load(std::memory_order_acquire); }
	void SetState(enTaskState state)
	{ 
		m_nState.store(state, std::memory_order_release);
	}
	virtual void Execute() = 0;

	virtual void OnFinish()
	{
		SetState(enTaskState::eTaskDone);
	}
	virtual void OnFailed()
	{
		SetState(enTaskState::eTaskFailed);
	}
private:
	std::string						m_TaskSignature;	//任务签名
	time_t							m_nExecuteStart;	//任务开始执行时间
	time_t							m_nExecuteEnd;		//任务执行完成时间
	CSafePtr<CThreadScheduler>		m_pScheduler;
	TaskPtr							m_pParentTask;
	WeakTaskPtr						m_pChildTask;
	std::atomic<enTaskState>		m_nState;
	std::list<TaskPtr>				m_ChildTaskList;
};

template<class Func, class...Args>
class CWithReturnTask : public CThreadTask
{
	enum
	{
		//参数个数
		arity = sizeof...(Args)
	};
	using ArgsTubleType = typename std::tuple<Args...>;
	using return_type = typename std::result_of<Func(Args...)>::type;
	using function_type = typename std::function<return_type(Args...)>;
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler, 
					std::string signature, 
					TaskPtr parentTask,
					TaskPtr childTask,
					const Func& func, 
					Args...args)
		: CThreadTask(scheduler,signature, parentTask,childTask)
	{
		m_Func = func;
		m_ArgTuple = std::make_tuple(args...);
	}
	virtual ~CWithReturnTask()
	{}
	virtual void Execute()
	{
		m_Res = TaskCaller<arity, return_type, Args...>::invoke(m_Func, m_ArgTuple);
	}
private:
	function_type			m_Func;
	ArgsTubleType			m_ArgTuple;
	return_type				m_Res;
	std::function<void()>	m_CallBack;
};

template<class Func, class...Args>
class CNoReturnTask : public CThreadTask
{
	enum
	{
		//参数个数
		arity = sizeof...(Args)
	};
	using ArgsTubleType = typename std::tuple<Args...>;
	using function_type = typename std::function<void(Args...)>;
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
					std::string signature,
					TaskPtr parentTask,
					TaskPtr childTask,
					const Func& func,
					Args...args)
		: CThreadTask(scheduler, signature, parentTask, childTask)
	{
		m_Func = func;
		m_ArgTuple = std::make_tuple(args...);
	}
	virtual ~CNoReturnTask()
	{}
	virtual void Execute()
	{
		TaskCaller<arity, void, Args...>::invoke(m_Func, m_ArgTuple);
	}
	virtual void OnFinish()
	{}
	virtual void OnFailed()
	{}
private:
	function_type			m_Func;
	ArgsTubleType			m_ArgTuple;
	std::function<void()>	m_CallBack;
};

#endif //__THREAD_TASK_H__
