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
					  ThreadFuncParamWrapper initFunc = ThreadFuncParamWrapper(),
					  ThreadFuncParamWrapper tickFunc = ThreadFuncParamWrapper());
	void PushTask(TaskPtr pTask);
	void DebugTask();
public:
	template<class Func, typename return_type = std::result_of<Func()>::type>
	CTaskHelper<return_type> Schedule(std::string signature, Func&& f)
	{
		TaskPtr pTask = TaskCreater<return_type, void, Func>::CreateTask(this, signature, f);
		std::lock_guard<std::mutex> guard(m_queue_mutex);
		m_Tasks.push(pTask);
		return CTaskHelper<return_type>(pTask);
	}

	template<typename RT1,typename RT2>
	CAcceptCombineTaskHelper<RT1, RT2> AcceptCombine(CTaskHelper<RT1>& task1, 
													 CTaskHelper<RT2>& task2)
	{
		task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2>();
		task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2>();
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask() };
		return CAcceptCombineTaskHelper<RT1, RT2>(this, taskList);
	}


	template<typename RT1, typename RT2, typename RT3>
	CAcceptCombineTaskHelper<RT1, RT2,RT3> AcceptCombine(
										 CTaskHelper<RT1>& task1,
										 CTaskHelper<RT2>& task2,
										 CTaskHelper<RT3>& task3)
	{
		task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2, RT3>();
		task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2, RT3>();
		task3.GetTask()->SetAcceptCombineInfo<2, RT1, RT2, RT3>();
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask()};
		return CAcceptCombineTaskHelper<RT1, RT2, RT3>(this, taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4>
	CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4> AcceptCombine(
										 CTaskHelper<RT1>& task1,
										 CTaskHelper<RT2>& task2,
										 CTaskHelper<RT3>& task3,
										 CTaskHelper<RT4>& task4)
	{
		task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2, RT3, RT4>();
		task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2, RT3, RT4>();
		task3.GetTask()->SetAcceptCombineInfo<2, RT1, RT2, RT3, RT4>();
		task4.GetTask()->SetAcceptCombineInfo<3, RT1, RT2, RT3, RT4>();
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask() };
		return CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4>(this, taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4, typename RT5>
	CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4,RT5> AcceptCombine(
										 CTaskHelper<RT1>& task1,
										 CTaskHelper<RT2>& task2,
										 CTaskHelper<RT3>& task3,
										 CTaskHelper<RT4>& task4,
										 CTaskHelper<RT5>& task5)
	{
		task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2, RT3, RT4, RT5>();
		task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2, RT3, RT4, RT5>();
		task3.GetTask()->SetAcceptCombineInfo<2, RT1, RT2, RT3, RT4, RT5>();
		task4.GetTask()->SetAcceptCombineInfo<3, RT1, RT2, RT3, RT4, RT5>();
		task5.GetTask()->SetAcceptCombineInfo<4, RT1, RT2, RT3, RT4, RT5>();
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask(),task5.GetTask() };
		return CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4, RT5>(this, taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4, typename RT5, typename RT6>
	CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4, RT5, RT6> AcceptCombine(
										 CTaskHelper<RT1>& task1,
										 CTaskHelper<RT2>& task2,
										 CTaskHelper<RT3>& task3,
										 CTaskHelper<RT4>& task4,
										 CTaskHelper<RT5>& task5,
										 CTaskHelper<RT6>& task6)
	{
		task1.GetTask()->SetAcceptCombineInfo<0, RT1, RT2, RT3, RT4, RT5, RT6>();
		task2.GetTask()->SetAcceptCombineInfo<1, RT1, RT2, RT3, RT4, RT5, RT6>();
		task3.GetTask()->SetAcceptCombineInfo<2, RT1, RT2, RT3, RT4, RT5, RT6>();
		task4.GetTask()->SetAcceptCombineInfo<3, RT1, RT2, RT3, RT4, RT5, RT6>();
		task5.GetTask()->SetAcceptCombineInfo<4, RT1, RT2, RT3, RT4, RT5, RT6>();
		task6.GetTask()->SetAcceptCombineInfo<5, RT1, RT2, RT3, RT4, RT5, RT6>();
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask(),task5.GetTask(),task6.GetTask() };
		return CAcceptCombineTaskHelper<RT1, RT2, RT3, RT4, RT5, RT6>(this, taskList);
	}

	template<typename ...Args>
	CApplyCombineTaskHelper ApplyCombine(Args... args)
	{
		std::vector<TaskPtr> taskList;
		PushArgs(taskList, args...);
		return CApplyCombineTaskHelper(this,taskList);
	}

private:
	//把参数压栈
	void PushArgs(std::vector<TaskPtr>& taskList)
	{
		return;
	}
	template<typename T>
	void PushArgs(std::vector<TaskPtr>& taskList, const T& t)
	{
		taskList.push_back(t.GetTask());
		t.GetTask().
	}

	template<typename First, typename... Rest>
	void PushArgs(std::vector<TaskPtr>& taskList, First& first, Rest &...rest)
	{
		taskList.push_back(first.GetTask());
		PushArgs(taskList, rest...);
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
