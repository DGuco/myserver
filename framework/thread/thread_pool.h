//
// mythread.h
// 基于c++14简单的线程池
// Created by DGuco on 17-10-12.
// Copyright © 2016年 DGuco. All rights reserved.
//


#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H

#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <bits/unordered_map.h>

using namespace std;

/**
 * 成员函数生命为ininle，避免multiple definition error
 */

void runWithNothing();

inline void runWithNothing()
{

}

typedef std::function<void()> FuncBeforeRun;

class CThreadPool
{
public:
	CThreadPool();
	CThreadPool(size_t);
	CThreadPool(size_t, FuncBeforeRun func);
	~CThreadPool();
	CThreadPool(const CThreadPool &) = delete;
	const CThreadPool &operator=(CThreadPool &) = delete;
	//向后添加任务
	template<class F, class... Args>
	auto PushTaskBack(F &&f, Args &&... args);
	//向前添加任务
	template<class F, class... Args>
	auto PushTaskFront(F &&f, Args &&... args);
	bool IsInThisThread();
	bool IsThisThreadIn(thread *thrd);

private:
	void ThreadFunc();
private:
	std::unordered_map<std::thread::id, std::shared_ptr<thread>> m_mWorkers;
	std::deque<std::function<void()> > m_qTasks;

	std::mutex m_mutex;
	std::condition_variable m_condition;
	FuncBeforeRun m_fBeforeRun;
	bool m_stop;
};

#include "thread_pool.cpp"

#endif //SERVER_THREADPOOL_H
