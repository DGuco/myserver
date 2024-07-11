/*****************************************************************
* FileName:thread_scheduler.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __THREAD_SCHEDULER_H__
#define __THREAD_SCHEDULER_H__

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include "thread_task.h"
#include "safe_pointer.h"
#include "time_helper.h"

struct thread_data
{
	CMyTimer	debug_timer;	//发送心跳包定时器
};

thread_local thread_data g_thread_data;

class ThreadPool 
{
public:
	ThreadPool(size_t);
	template<class Func, class... Args>
	void Schedule(std::string signature, Func&& f, Args&&... args);
	~ThreadPool();
private:
	void ConsumeTask();
	void DebugTask();
private:
	std::vector<std::thread> workers;
	std::queue<CSafePtr<CThreadTask>> tasks;
	std::mutex queue_mutex;
	//std::condition_variable condition;
	bool stop;
};

#endif //__THREAD_SCHEDULER_H__

