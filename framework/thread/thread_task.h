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
#include <atomic>
#include "my_thread.h"
#include "lock_free_limit_queue.h"
#include "log.h"
#include "task_helper.h"

#define EMPTY_VOID_FUNC = []{}
#define MAX_CHILD_TASK_COUNT (5)
using namespace my_std;

class CThreadScheduler;
class CThreadTask;
enum class enTaskState : unsigned char
{
	eTaskInit = 0,
	eTaskDoing = 1,
	eTaskDone = 2,
	eTaskFailed = 3,
};

enum class enCombineType : unsigned char
{
	eCombineNone = 0,
	eCombineCombined = 1,
	eCombineDone = 2,
};

class CArgsHolder
{
public:
	CArgsHolder() {};
	virtual ~CArgsHolder() {};
public:
	virtual void  FillWaitTaskParm(TaskPtr pTask,TaskPtr pWaitTask) = 0;
	virtual bool  Empty() = 0;
};

class CThreadTask : public enable_shared_from_this<CThreadTask>
{
public:
	CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature);
	virtual ~CThreadTask();
	void SetStartTime(time_t time)				{ m_nExecuteStart = time; }
	time_t GetStartTime()						{ return m_nExecuteStart; }
	const std::string& GetSignature()			{ return m_TaskSignature; }
	TaskPtr GetShared()							{ return shared_from_this(); }
	CSafePtr<CThreadScheduler>   GetScheduler() { return m_pScheduler; }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	enTaskState GetState()						{ return m_nState.load(std::memory_order_acquire); }
	void SetState(enTaskState state)			{ m_nState.store(state, std::memory_order_release);}
	enCombineType GetCombineType()				{ return m_combineType; }
	void SetCombineType(enCombineType state)	{ m_combineType = state; }
	template<BYTE combine_index, class... Args>
	void SetAcceptCombineInfo()
	{
		CSafeLock guard(m_combineLock);
		if (m_pCombinedArgs != NULL)
		{
			ASSERT_EX(false, "This Task has combined once");
		}
		m_pCombinedArgs = new CTaskArgsList<combine_index, Args...>();
	}
	
	void SetCombineTask(TaskPtr ptr);
	void SetCombineCount(BYTE value);
	void CombineTaskDone();
	void OnFinish();
	void OnFailed();
	void AddChildTask(TaskPtr pTask);
	void Run();
	void RunChildTask();
public:
	virtual void  Execute() = 0;
	virtual void  ExecuteChildTask(TaskPtr pChildTask) = 0;
	virtual void  ExecuteFromParent(void* pRes,bool sucess = true) = 0;
	virtual void* GetRes() = 0;
	virtual void* GetArgs() = 0;
private:
	std::atomic<enTaskState>			m_nState;
	std::string							m_TaskSignature;	//任务签名
	time_t								m_nExecuteStart;	//任务开始执行时间
	LockFreeLimitQueue<TaskPtr>			m_childTaskVec;
	//combine info
	CMyLock								m_combineLock;
	TaskPtr								m_pCombineTask;
	std::atomic_uchar					m_combineDone;
	std::atomic_uchar					m_combineCount;
	enCombineType						m_combineType;
	CSafePtr<CArgsHolder>				m_pCombinedArgs;
protected:
	CSafePtr<CThreadScheduler>		m_pScheduler;
};

class CVoidArgsList : public CArgsHolder
{
public:
	CVoidArgsList() {};
	virtual ~CVoidArgsList() {};
	virtual void  FillWaitTaskParm(TaskPtr pTask, TaskPtr pWaitTask)
	{
		return;
	}

	virtual bool  Empty()
	{
		return true;
	}
};
template<BYTE combineIndex, class... Args>
class CTaskArgsList : public CArgsHolder
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
	virtual void  FillWaitTaskParm(TaskPtr pTask,TaskPtr pWaitTask)
	{
		void* pRes = pTask->GetRes();
		void* pArgs = pWaitTask->GetArgs();
		if (pRes == NULL || pArgs == NULL)
		{
			return;
		}
		ParamTypeElement& tmArgs = *(ParamTypeElement*)(pArgs);
		using ArgType = args<combineIndex>::type;
		ArgType& tmRes = *(ArgType*)(pRes);
		std::get<combineIndex>(tmArgs) = tmRes;
	}

	virtual bool Empty()
	{
		return false;
	}
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
	//每个参数的类型
	template<size_t I>
	struct args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, ArgsTubleType>::type;
	};
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		: CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}
	virtual ~CWithReturnTask()
	{}
	virtual void Execute()
	{
		m_Res = TaskCaller<arity, return_type, Args...>::invoke(m_Func, m_ArgTuple);
	}

	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent((void*)(&m_Res));
		}
		else
		{
			pChildTask->ExecuteFromParent(NULL, false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes, bool sucess = true)
	{}

	virtual void* GetRes()
	{
		return (void*)(&m_Res);
	}
	virtual void* GetArgs()
	{
		return (void*)(&m_ArgTuple);
	}
private:
	function_type				m_Func;
	ArgsTubleType				m_ArgTuple;
	return_type					m_Res;
};

template<class Func, typename Par>
class CWithReturnTask<Func,Par> : public CThreadTask
{
	using return_type = typename std::result_of<Func(Par)>::type;
	using function_type = typename std::function<return_type(Par)>;
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		: CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}
	virtual ~CWithReturnTask()
	{}
	virtual void Execute()
	{
		m_Res = TaskCaller<1,return_type,Par>::invoke(m_Func, m_Param);
	}

	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent((void*)(&m_Res));
		}else
		{
			pChildTask->ExecuteFromParent(NULL,false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes,bool sucess = true)
	{
		if (pRes != NULL && sucess)
		{
			m_Param = *(Par*)(pRes);
			//如果就在当前的执行shcheler中，直接执行
			if (g_thread_data.own_scheduler == m_pScheduler)
			{
				Run();
			}
			else
			{
				//push 到对应scheduler的队列中
				m_pScheduler->PushTask(GetShared());
			}
		}
		else
		{
			OnFailed();
		}
	}

	virtual void* GetRes()
	{
		return (void*)(&m_Res);
	}

	virtual void* GetArgs()
	{
		return NULL;
	}
private:
	function_type				m_Func;
	return_type					m_Res;
	Par							m_Param;
};

template<class Func>
class CWithReturnTask<Func,void> : public CThreadTask
{
	using return_type = typename std::result_of<Func()>::type;
	using function_type = typename std::function<return_type()>;
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler,
					std::string signature,
					const Func& func)
		: CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}

	virtual ~CWithReturnTask()
	{}
	
	virtual void Execute()
	{
		m_Res = TaskCaller<0,return_type>::invoke(m_Func);
	}

	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent((void*)(&m_Res));
		}
		else
		{
			pChildTask->ExecuteFromParent(NULL,false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes, bool sucess = true)
	{
		if (sucess)
		{
			//如果就在当前的执行shcheler中，直接执行
			if (g_thread_data.own_scheduler == m_pScheduler)
			{
				Run();
			}
			else
			{
				//push 到对应scheduler的队列中
				m_pScheduler->PushTask(GetShared());
			}
		}
		else
		{
			OnFailed();
		}
	}

	virtual void* GetRes()
	{
		return (void*)(&m_Res);
	}

	virtual void* GetArgs()
	{
		return NULL;
	}
private:
	function_type				m_Func;
	return_type					m_Res;
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
	//每个参数的类型
	template<size_t I>
	struct args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, ArgsTubleType>::type;
	};
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		: CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}
	virtual ~CNoReturnTask()
	{}
	
	virtual void Execute()
	{
		TaskCaller<arity, void, Args...>::invoke(m_Func, m_ArgTuple);
	}

	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent(NULL);
		}
		else
		{
			pChildTask->ExecuteFromParent(NULL, false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes, bool sucess = true)
	{}

	virtual void* GetRes()
	{
		return NULL;
	}

	virtual void* GetArgs()
	{
		return (void*)(&m_ArgTuple);
	}
private:
	function_type				m_Func;
	ArgsTubleType				m_ArgTuple;
};

template<class Func, typename Par>
class CNoReturnTask<Func,Par> : public CThreadTask
{
	using function_type = typename std::function<void(Par)>;
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		:CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}
	virtual ~CNoReturnTask()
	{}

	virtual void Execute()
	{
		TaskCaller<1,void,Par>::invoke(m_Func,m_Param);
	}
	
	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent(NULL);
		}
		else
		{
			pChildTask->ExecuteFromParent(NULL, false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes,bool sucess)
	{
		if (pRes != NULL && sucess)
		{
			m_Param = *(Par*)(pRes);
			//如果就在当前的执行shcheler中，直接执行
			if (g_thread_data.own_scheduler == m_pScheduler)
			{
				Run();
			}
			else
			{
				//push 到对应scheduler的队列中
				m_pScheduler->PushTask(GetShared());
			}
		}
		else
		{
			OnFailed();
		}
	}

	virtual void* GetRes()
	{
		return NULL;
	}

	virtual void* GetArgs()
	{
		return NULL;
	}
private:
	function_type			m_Func;
	Par						m_Param;
};

template<class Func>
class CNoReturnTask<Func,void> : public CThreadTask
{
	using function_type = typename std::function<void()>;
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		:CThreadTask(scheduler, signature)
	{
		m_Func = func;
	}
	virtual ~CNoReturnTask()
	{}
	virtual void Execute()
	{
		TaskCaller<0,void>::invoke(m_Func);
	}

	virtual void ExecuteChildTask(TaskPtr pChildTask)
	{
		if (GetState() == enTaskState::eTaskDone)
		{
			pChildTask->ExecuteFromParent(NULL);
		}
		else
		{
			pChildTask->ExecuteFromParent(NULL, false);
		}
	}

	virtual void  ExecuteFromParent(void* pRes, bool sucess)
	{
		if (sucess)
		{
			//如果就在当前的执行shcheler中，直接执行
			if (g_thread_data.own_scheduler == m_pScheduler)
			{
				Run();
			}
			else
			{
				//push 到对应scheduler的队列中
				m_pScheduler->PushTask(GetShared());
			}
		}
		else
		{
			OnFailed();
		}
	}
	virtual void* GetRes()
	{
		return NULL;
	}

	virtual void* GetArgs()
	{
		return NULL;
	}
private:
	function_type			m_Func;
};

#endif //__THREAD_TASK_H__
