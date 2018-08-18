//
// Created by dguco on 18-8-18.
//

#include "thread_pool.h"


inline CThreadPool::CThreadPool()
	: CThreadPool(thread::hardware_concurrency())
{

}


inline CThreadPool::CThreadPool(size_t threads)
	: m_fBeforeRun(&runWithNothing),
	  m_stop(false)
{
	for (size_t i = 0; i < threads; ++i) {
		std::shared_ptr<thread> th = std::make_shared<thread>(std::mem_fn(&CThreadPool::ThreadFunc), this);
		if (th) {
			m_mWorkers[th->get_id()] = std::move(th);
		}
	}
}

inline CThreadPool::CThreadPool(size_t threads, FuncBeforeRun func)
	: m_fBeforeRun(func),
	  m_stop(false)
{
	for (size_t i = 0; i < threads; ++i) {
		std::shared_ptr<thread> th = std::make_shared<thread>(std::mem_fn(&CThreadPool::ThreadFunc), this);
		if (th) {
			m_mWorkers[th->get_id()] = std::move(th);
		}
	}
}


inline CThreadPool::~CThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_stop = true;
		m_condition.notify_all();
	}
	for (auto it = m_mWorkers.begin(); it != m_mWorkers.end();) {
		it->second->join();
		m_mWorkers.erase(it);
	}
}


inline bool CThreadPool::IsInThisThread()
{
	auto it = m_mWorkers.find(this_thread::get_id());
	return it != m_mWorkers.end();
}


inline bool CThreadPool::IsThisThreadIn(thread *thrd)
{
	if (thrd) {
		auto it = m_mWorkers.find(thrd->get_id());
		return it != m_mWorkers.end();
	}
	else {
		return false;
	}
}

inline void CThreadPool::ThreadFunc()
{
	m_fBeforeRun();
	while (true) {
		std::function<void()> task;
		std::unique_lock<std::mutex> lock(this->m_mutex);
		this->m_condition.wait(lock,
							   [this]
							   { return (this->m_stop || !this->m_qTasks.empty()); });
		//如果停止线程
		if (this->m_stop) {
			return;
		}
		//如果唤醒时队列任务为空，则认为是意外唤醒继续阻塞
		if (this->m_qTasks.empty()) {
			continue;
		}
		task = std::move(this->m_qTasks.front());
		this->m_qTasks.pop_front();
		lock.unlock();
		task();
	}
}

template<class F, class... Args>
auto CThreadPool::PushTaskBack(F &&f, Args &&... args)
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	if (task == NULL) {
		throw std::runtime_error("Create task error ");
	}

	std::future<return_type> res = task->get_future();

	//如果在当前线程内，则直接执行
	if (IsInThisThread()) {
		(*task)();
	}
	else {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_stop)
			throw std::runtime_error("ThreadPool has stopped");
		m_qTasks.emplace_back([task]()
							  { (*task)(); });
		m_condition.notify_one();
	}
	return res;
}

template<class F, class... Args>
auto CThreadPool::PushTaskFront(F &&f, Args &&... args)
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	if (task == NULL) {
		throw std::runtime_error("Create task error ");
	}

	std::future<return_type> res = task->get_future();
	//如果在当前线程内，则直接执行
	if (IsInThisThread()) {
		(*task)();
	}
	else {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_stop)
			throw std::runtime_error("ThreadPool has stopped");
		else {
			m_qTasks.emplace_front([task]()
								   { (*task)(); });
			m_condition.notify_one();
		}
	}
	return res;
}
