/*****************************************************************
* FileName:thread_task.h
* Summary :
* Date	  :2024-7-10
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__
#include <functional>
#include <string>
#include <tuple>
#include <memory>
#include <atomic>
#include <queue>
#include "my_thread.h"
#include "log.h"
#include "t_array.h"

using namespace my_std;

class CThreadScheduler;
enum class enTaskState : unsigned char
{
	eTaskInit = 0,
	eTaskWaitingFoDoing = 1,
	eTaskDoing = 2,
	eTaskDone = 3,
	eTaskFailed = 4,
};

enum class enCombineType : unsigned char
{
	eCombineInit = 0,
	eCombineApply = 1,
	eCombineAccept = 2,
	eCombineDone = 3,
};

class IArgsHolder
{
public:
	IArgsHolder() {};
	virtual ~IArgsHolder() {};
	virtual void  FillWaitTaskParm(TaskPtr pTask,TaskPtr pWaitTask) = 0;
	virtual bool  Empty() = 0;
};

template<BYTE combineIndex,class... Args>
class CArgsHolder 
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

template<int combineIndex>
class CArgsHolder<combineIndex,void>
{
public:
	void FillWaitTaskParm(TaskPtr pTask, TaskPtr pWaitTask)
	{
		return;
	}

	virtual bool  Empty()
	{
		return true;
	}
};

class CThreadTask : public enable_shared_from_this<CThreadTask>
{
	friend class CThreadScheduler;
public:
	CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature);
	virtual ~CThreadTask();
	time_t GetStartTime()						{ return m_nExecuteStart; }
	const std::string& GetSignature()			{ return m_TaskSignature; }
	TaskPtr GetShared()							{ return shared_from_this(); }
	CSafePtr<CThreadScheduler>   GetScheduler() { return m_pScheduler; }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	enTaskState GetState()						{ return m_nState.load(std::memory_order_acquire); }
	//添加到执行队列
	void AddToSchedulerQueue();
	//添加子任务
	void AddChildTask(TaskPtr pTask);
protected:
	void SetStartTime(time_t time)				{ m_nExecuteStart = time; }
	void SetState(enTaskState state)			{ m_nState.store(state, std::memory_order_release);}
	virtual void OnFinish();
	virtual void OnFailed();
	void Run();
	void RunChildTask();
public:
	template<BYTE combine_index, class... Args>
	void SetAcceptCombineInfo()
	{
		CSafeLock guard(m_combineLock);
		if (m_pCombinedArgs != NULL)
		{
			ASSERT_EX(false, "This Task has combined once");
		}
		m_pCombinedArgs = new CArgsHolder<combine_index, Args...>();
	}
	
public:
	virtual void  Execute() = 0;
	virtual void  ExecuteChildTask(TaskPtr pChildTask) = 0;
	virtual void  ExecuteFromParent(void* pRes,bool sucess = true) = 0;
	virtual void* GetRes() = 0;
	virtual void* GetArgs() = 0;
public:
	virtual bool  IsCombinedTask() {return false;}
	virtual void  CombineTaskDone()  {ASSERT_EX(false,"NOT Combinetask call CombineTaskDone illegal");}
	virtual void  SetCombineTask(int index,TaskPtr pTask) {ASSERT_EX(false,"NOT Combinetask call SetCombineTask illegal");}
protected:
	CMyLock								m_childTaskLock;
	std::string							m_TaskSignature;	//任务签名
	time_t								m_nExecuteStart;	//任务开始执行时间
	//任务执行完成后，需要执行的后续子任务
	std::queue<TaskPtr>					m_childTaskQueue;
	//当前任务的执行状态
	std::atomic<enTaskState>			m_nState;
	//如果当前任务是一组合任务的前置任务，保存作为前置任务的参数的位置信息
	CSafePtr<IArgsHolder>				m_pCombinedArgs;
	CSafePtr<CThreadScheduler>			m_pScheduler;
};

template<int combine_count>
class CCombineTask : public CThreadTask
{
public:
	CCombineTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
		: CThreadTask(scheduler, signature)
	{
		SetCombineType(enCombineType::eCombineInit);
		m_combineDone = 0;
	}
	
	virtual ~CCombineTask()
	{
		try 
		{
			enCombineType bType = GetCombineType();
			enTaskState bState = GetState();
			//等待执行CombineTaskDone
			// {
			// 	CSafeLock guard(m_combineLock);
			// 	if (m_pCombineTask != NULL &&
			// 		bType == enCombineType::eCombineAccept &&
			// 		bState == enTaskState::eTaskDone)
			// 	{
			// 		m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
			// 	}
			// }
			if (bState == enTaskState::eTaskDone)
			{
				//m_pCombineTask->CombineTaskDone();
			}
			else
			{
				//m_pCombineTask->OnFailed();
			}
			SetCombineType(enCombineType::eCombineDone);
			RunChildTask();
		}
		catch(std::exception e)
		{}
	}

	virtual void OnFinish()
	{
		enCombineType bType = GetCombineType();
		// //等待执行CombineTaskDone
		// {
		// 	CSafeLock guard(m_combineLock);
		// 	if (m_pCombineTask != NULL && bType == enCombineType::eCombineAccept)
		// 	{
		// 		m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
		// 	}
		// }
		// m_pCombineTask->CombineTaskDone();
		SetCombineType(enCombineType::eCombineDone);
		SetState(enTaskState::eTaskDone);
		RunChildTask();
	}

	virtual void OnFailed() 
	{
		enCombineType bType = GetCombineType();
		//等待执行CombineTaskDone
		//m_pCombineTask->OnFailed();
		SetCombineType(enCombineType::eCombineDone);
		SetState(enTaskState::eTaskFailed);
		RunChildTask();
		CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
	}

	enCombineType GetCombineType()				{ return m_combineType; }
	void SetCombineType(enCombineType state)	{ m_combineType = state; }

	virtual void  SetCombineTask(int index,TaskPtr pTask)
	{
		if(index >= 0 && index < combine_count)
		{
			if(m_pCombineTask[index].lock() == NULL)
			{
				m_pCombineTask[index] = pTask;
			}else
			{
				ASSERT_EX(false,"SetCombineTask index has set");
			}
		}else
		{
			ASSERT_EX(false,"SetCombineTask index overflow");
		}
	}

	// void SetCombineTask(TaskPtr ptr,enCombineType combineType)
	// {
	// 	enCombineType bType = GetCombineType();
	// 	if (bType != enCombineType::eCombineInit)
	// 	{
	// 		ASSERT_EX(false, "This task {%s} has been combined", m_TaskSignature.c_str());
	// 	}
	// 	enTaskState bState = GetState();
	// 	{
	// 		CSafeLock guard(m_combineLock);
	// 		m_pCombineTask = ptr;
	// 		if (bState == enTaskState::eTaskDone)
	// 		{
	// 			if (m_pCombinedArgs != NULL && combineType == enCombineType::eCombineAccept)
	// 			{
	// 				m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
	// 			}
	// 		}
	// 	}
	// 	//此时任务已经完成，则直接CombineTaskDone
	// 	if (bState == enTaskState::eTaskDone)
	// 	{
	// 		m_pCombineTask->CombineTaskDone();
	// 		SetCombineType(enCombineType::eCombineDone);
	// 	}
	// 	//此时任务失败，则直接OnFailed
	// 	else if (bState == enTaskState::eTaskFailed)
	// 	{
	// 		m_pCombineTask->OnFailed();
	// 		SetCombineType(enCombineType::eCombineDone);
	// 	}
	// 	else  //任务还没有完成，正在执行中，标记任务等待执行CombineTaskDone
	// 	{
	// 		SetCombineType(combineType);
	// 	}
	// }
	
	virtual void CombineTaskDone()
	{
		enTaskState bState = GetState();
		//其中有一个前置任务失败了，后续的不用执行了
		if (bState == enTaskState::eTaskFailed)
		{
			return;
		}
		
		// fetch_add(1) 保证原子性递增
		const int oldValue = m_combineDone.fetch_add(1, std::memory_order_acq_rel);
		const int newValue = oldValue + 1;
		// 修改判断条件为严格相等
		if (newValue == combine_count) 
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
	}

	virtual bool  IsCombinedTask() {return true;}
private:
	//combine info
	CMyLock								m_combineLock;
	//前置任务列表
	TArray<WeakTaskPtr,combine_count>	m_pCombineTask;
	//如果当前任务是合并任务之一(前置任务或者后续子任务)，合并任务的类型
	enCombineType						m_combineType;
	//如果当前任务是一组合并任务的后续子任务，前置任务已完成的数量
	std::atomic_int						m_combineDone;
};

template<>
class CCombineTask<0> : public CThreadTask
{
public:
	CCombineTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
		: CThreadTask(scheduler, signature)
	{}
};

template<int combine_count,class Func, class...Args>
class CWithReturnTask : public CCombineTask<combine_count>
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
		: CCombineTask<combine_count>(scheduler, signature)
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
	{
		ASSERT_EX(false, "<class Func, class...Args>CWithReturnTask can not ExecuteFromParent");
	}

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

template<int combine_count,class Func, typename Par>
class CWithReturnTask<combine_count,Func,Par> : public CCombineTask<combine_count>
{
	using return_type = typename std::result_of<Func(Par)>::type;
	using function_type = typename std::function<return_type(Par)>;
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		: CCombineTask<combine_count>(scheduler, signature)
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

template<int combine_count,class Func>
class CWithReturnTask<combine_count,Func,void> : public CCombineTask<combine_count>
{
	using return_type = typename std::result_of<Func()>::type;
	using function_type = typename std::function<return_type()>;
public:
	CWithReturnTask(CSafePtr<CThreadScheduler> scheduler,
					std::string signature,
					const Func& func)
		: CCombineTask<combine_count>(scheduler, signature)
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

template<int combine_count,class Func, class...Args>
class CNoReturnTask : public CCombineTask<combine_count>
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
		: CCombineTask<combine_count>(scheduler, signature)
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
	{
		ASSERT_EX(false, "<class Func, class...Args>CNoReturnTask can not ExecuteFromParent");
	}

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

template<int combine_count,class Func, typename Par>
class CNoReturnTask<combine_count,Func,Par> : public CCombineTask<combine_count>
{
	using function_type = typename std::function<void(Par)>;
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		:CCombineTask<combine_count>(scheduler, signature)
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

template<int combine_count,class Func>
class CNoReturnTask<combine_count,Func,void> : public CCombineTask<combine_count>
{
	using function_type = typename std::function<void()>;
public:
	CNoReturnTask(CSafePtr<CThreadScheduler> scheduler,
		std::string signature,
		const Func& func)
		:CCombineTask<combine_count>(scheduler, signature)
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
