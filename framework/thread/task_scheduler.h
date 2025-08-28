/**
 * @file task_scheduler.h
 * @author DGuco(1139140929@qq.com)
 * @brief 任务调度器
 * @version 0.1
 * @date 2025-08-28
 */

#ifndef _TASK_SCHEDULER_H_
#define _TASK_SCHEDULER_H_

#include <queue>
#include <thread>
#include <functional>
#include <list>
#include "safe_pointer.h"
#include "task.h"
#include "task_helper.h"
#include "time_helper.h"
#include "my_thread.h"
#include "my_lock.h"

class CTaskScheduler
{
public:
	CTaskScheduler(std::string signature);
    //
	virtual ~CTaskScheduler();
	//调度任务
    virtual void ScheduleTask(TaskPtr pTask);
	//执行任务
    void ConsumeTask();
	//添加任务
    void PushTask(TaskPtr pTask);
public:
	template<class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> Schedule(std::string signature, Func&& f)
	{
		TaskPtr pTask = TaskCreater<return_type, void, Func>::CreateTask(this, signature, f);
		ScheduleTask(pTask);
		return CTaskHelper<return_type>(pTask);
	}

	template<class Func, typename return_type = std::result_of<Func()>::type>
	static CTaskHelper<return_type> Schedule(CSafePtr<CTaskScheduler> pScheduler, std::string signature, Func&& f)
	{
		TaskPtr pTask = TaskCreater<return_type, void, Func>::CreateTask(pScheduler, signature, f);
		pScheduler->ScheduleTask(pTask);
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

	void StopScheduler();

	void Join(); 
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
	
protected:
	std::queue<TaskPtr> m_Tasks;
	CMyLock		        m_queue_mutex;
	std::string         m_Signature;	//任务签名
	bool stop;
};

#endif