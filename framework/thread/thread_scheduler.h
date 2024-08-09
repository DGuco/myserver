/*****************************************************************
* FileName:thread_scheduler.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#pragma once
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

class CThreadScheduler
{
public:
	CThreadScheduler(std::string signature);
	~CThreadScheduler();
	bool Init(size_t threads);
	void PushTask(TaskPtr pTask);
	void ConsumeTask();
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
	static CCombineTaskHelper<RT1, RT2> Combine(CTaskHelper<RT1> task1, 
											    CTaskHelper<RT2> task2)
	{
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask() };
		return CCombineTaskHelper<RT1, RT2>(taskList);
	}

	template<typename RT1, typename RT2, typename RT3>
	static CCombineTaskHelper<RT1, RT2,RT3> Combine(
										 CTaskHelper<RT1> task1,
										 CTaskHelper<RT2> task2,
										 CTaskHelper<RT3> task3)
	{
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask()};
		return CCombineTaskHelper<RT1, RT2, RT3>(taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4>
	static CCombineTaskHelper<RT1, RT2, RT3, RT4> Combine(
										 CTaskHelper<RT1> task1,
										 CTaskHelper<RT2> task2,
										 CTaskHelper<RT3> task3,
										 CTaskHelper<RT4> task4)
	{
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask() };
		return CCombineTaskHelper<RT1, RT2, RT3, RT4>(taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4, typename RT5>
	static CCombineTaskHelper<RT1, RT2, RT3, RT4,RT5> Combine(
										 CTaskHelper<RT1> task1,
										 CTaskHelper<RT2> task2,
										 CTaskHelper<RT3> task3,
										 CTaskHelper<RT4> task4,
										 CTaskHelper<RT5> task5)
	{
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask(),task5.GetTask() };
		return CCombineTaskHelper<RT1, RT2, RT3, RT4, RT5>(taskList);
	}

	template<typename RT1, typename RT2, typename RT3, typename RT4, typename RT5, typename RT6>
	static CCombineTaskHelper<RT1, RT2, RT3, RT4, RT5, RT6> Combine(
										 CTaskHelper<RT1> task1,
										 CTaskHelper<RT2> task2,
										 CTaskHelper<RT3> task3,
										 CTaskHelper<RT4> task4,
										 CTaskHelper<RT5> task5,
										 CTaskHelper<RT6> task6)
	{
		std::vector<TaskPtr> taskList{ task1.GetTask(),task2.GetTask(),task3.GetTask(),task4.GetTask(),task5.GetTask(),task6.GetTask() };
		return CCombineTaskHelper<RT1, RT2, RT3, RT4, RT5, RT6>(taskList);
	}
private:
	std::vector<CSafePtr<CTaskThread>> m_Workers;
	std::queue<TaskPtr> m_Tasks;
	CMyLock		m_queue_mutex;
	std::list<TaskPtr> m_WaitingTasks;
	std::string m_Signature;	//任务签名
	CMyTimer	debug_timer;	//线程详情debug timer
	//std::condition_variable condition;
	bool stop;
};

#endif //__THREAD_SCHEDULER_H__
