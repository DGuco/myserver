/*****************************************************************
* FileName:thread_scheduler.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __THREAD_SCHEDULER_H__
#define __THREAD_SCHEDULER_H__

#include <queue>
#include <thread>
#include <functional>
#include <list>
#include <safe_pointer.h>
#include "thread_task.h"
#include "task_helper.h"
#include "time_helper.h"
#include "my_thread.h"
#include "my_lock.h"

#define THREAD_TASK_DEBUG_TIME (20 *1000)   //任务队列调试时间间隔

class CThreadScheduler
{
public:
	CThreadScheduler(std::string signature);
	virtual ~CThreadScheduler();
	virtual void ConsumeTask();
	virtual bool Init(size_t threads,
					ThreadFuncParam initFunc = NULL,
					ThreadFuncParam tickFunc= NULL,
					void**		    initFuncArgs = NULL,
					void**		    tickFuncArgs = NULL);
					
	void PushTask(TaskPtr pTask);
	void DebugTask();
	int  ThreadCount() { return m_Workers.size(); }
public:
	void Schedule(TaskPtr pTask)
	{
		if(pTask->GetState() != enTaskState::eTaskInit)
		{
			ASSERT_EX(false,"CThreadScheduler Schedule task failed,the task has been scheduled");
			return;
		}
		pTask->SetState(enTaskState::eTaskWaitingFoDoing);
		std::lock_guard<std::mutex> guard(m_queue_mutex);
		m_Tasks.push(pTask);
	}

	template<class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> Schedule(std::string signature, Func&& f)
	{
		TaskPtr pTask = TaskCreater<return_type, void, Func>::CreateTask(this, signature, f);
		Schedule(pTask);
		return CTaskHelper<return_type>(pTask);
	}

	template<class Func, typename return_type = std::result_of<Func()>::type>
	static CTaskHelper<return_type> Schedule(CSafePtr<CThreadScheduler> pScheduler, std::string signature, Func&& f)
	{
		TaskPtr pTask = TaskCreater<return_type, void, Func>::CreateTask(pScheduler, signature, f);
		pScheduler->Schedule(pTask);
		return CTaskHelper<return_type>(pTask);
	}

	template<typename ...Args,int combine_count = sizeof...(Args)>
	static CApplyCombineTaskHelper<combine_count> ApplyCombine(Args... args)
	{
		std::vector<TaskPtr> taskList = {args...};
		return CApplyCombineTaskHelper<combine_count>(taskList);
	}

    // template<typename RT1, typename RT2, typename RT3, typename RT4, typename RT5>
    // static CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4,RT5> AcceptCombine(
    //                                      CTaskHelper<RT1>& task1,
    //                                      CTaskHelper<RT2>& task2,
    //                                      CTaskHelper<RT3>& task3,
    //                                      CTaskHelper<RT4>& task4,
    //                                      CTaskHelper<RT5>& task5)
    // {
    //     task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2, RT3, RT4, RT5>();
    //     task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2, RT3, RT4, RT5>();
    //     task3.GetTask()->SetAcceptCombineInfo<2, RT1, RT2, RT3, RT4, RT5>();
    //     task4.GetTask()->SetAcceptCombineInfo<3, RT1, RT2, RT3, RT4, RT5>();
    //     task5.GetTask()->SetAcceptCombineInfo<4, RT1, RT2, RT3, RT4, RT5>();
    //     std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask(),task5.GetTask() };
    //     return CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4, RT5>(this, taskList);
    // }

	// 1. 可变参数主模板
	template<typename... TaskHelpers>
	static CAcceptCombineTaskHelper<typename TaskHelpers::ReturnType...> AcceptCombine(TaskHelpers&... tasks) 
	{
		constexpr size_t TaskCount = sizeof...(TaskHelpers);
		std::vector<TaskPtr> taskList = {tasks.GetTask()...};
		CombineArgs<0,typename TaskHelpers::ReturnType...>(tasks...);
		return CAcceptCombineTaskHelper<typename TaskHelpers::ReturnType...>(taskList);
	}

	// // 2. 实现模板（利用索引序列展开）
	// template<typename... TaskHelpers, size_t... Indices>
	// static CAcceptCombineTaskHelper<typename TaskHelpers::ReturnType...> AcceptCombineImpl(TaskHelpers&... tasks, std::index_sequence<Indices...>) 
	// {
	// 	// 为每个任务设置组合信息（自动生成索引和类型列表）
	// 	(tasks.GetTask()->SetAcceptCombineInfo<Indices, typename TaskHelpers::ReturnType...>(), ...);
	// 	// 收集任务指针到向量（参数包展开）
	// 	std::vector<TaskPtr> taskList = {tasks.GetTask()...};
	// 	// 返回对应数量的组合任务助手
	// 	return CAcceptCombineTaskHelper<typename TaskHelpers::ReturnType...>(this, taskList);
	// }	
	
private:
	template<int N,typename ...Args>
    static void CombineArgs()
    {
        return;
    }
    template<int N,typename ...Args,typename T>
    static void CombineArgs(const T& t)
    {
  		t.GetTask()->SetAcceptCombineInfo<N,Args...>();
    }

    template<int N,typename ...Args,typename First, typename... Rest>
    static void CombineArgs(First& first, Rest &...rest)
    {
		first.GetTask()->SetAcceptCombineInfo<N,Args...>();
        CombineArgs<N+1,Args...>(rest...);
    }
	
private:
	std::vector<CSafePtr<CMyThread>> m_Workers;
	std::queue<TaskPtr> m_Tasks;
	CMyLock		m_queue_mutex;
	std::list<TaskPtr> m_WaitingTasks;
	std::string m_Signature;	//任务签名
	CMyTimer	debug_timer;	//线程详情debug timer
	//std::condition_variable condition;
	bool stop;
};

#endif //__THREAD_SCHEDULER_H__
