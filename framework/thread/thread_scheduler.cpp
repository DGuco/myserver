#include "thread_scheduler.h"
#include "time_helper.h"

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
/*
	for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
			{
				for (;;)
				{
					CSafePtr<CThreadTask> pTask;
					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						if (this->stop && this->tasks.empty())
							return;
						pTask = std::move(this->tasks.front());
						this->tasks.pop();
					}
					if (pTask)
					{
						try
						{
							pTask->Execute();
							pTask->OnFinish();
						}
						catch (std::exception* e)
						{
							pTask->OnFailed();
						}
					}
				}
			}
			);
*/
	for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
			{
				time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
				//30ÃëÊä³öÒ»´Î
				g_thread_data.debug_timer.BeginTimer(nNow, 30 * 1000);
				for (;;)
				{
					this->ConsumeTask();
					this->DebugTask();
				}
			}
			);
}

inline ThreadPool::~ThreadPool()
{
}

template<class Func, class... Args>
void ThreadPool::Schedule(std::string signature,Func&& f, Args&&... args)
{
	CSafePtr<CThreadTask> pTask = new CParamTask<Func, Args...>(signature, f, std::make_tuple(args...));
	std::lock_guard<std::mutex> guard(queue_mutex);
	tasks.emplace(pTask);
}

void ThreadPool::ConsumeTask()
{
	while (true)
	{
		CSafePtr<CThreadTask> pTask;
		{
			std::lock_guard<std::mutex> guard(queue_mutex);
			if (tasks.empty())
			{
				return;
			}
			pTask = std::move(this->tasks.front());
			this->tasks.pop();
		}
		if (pTask != NULL)
		{
			try
			{
				pTask->Execute();
			}
			catch (std::exception* e)
			{
				pTask->OnFailed();
				pTask.Free();
				continue;
			}

			try
			{
				pTask->OnFinish();
			}
			catch (std::exception* e)
			{
				pTask.Free();
			}
			pTask.Free();
		}
	}
}

void ThreadPool::DebugTask()
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	if (g_thread_data.debug_timer.IsTimeout(nNow))
	{
		int nSize = 0;
		{
			std::lock_guard<std::mutex> guard(queue_mutex);
			nSize = tasks.size();
		}
		CACHE_LOG(THREAD_CACHE, "Thread task queuesize = {}", nSize);
	}
}