#include "thread_scheduler.h"
#include "time_helper.h"

CTaskThread::CTaskThread(CSafePtr<CThreadScheduler> scheduler) 
	: m_pScheduler(scheduler)
{
}


CTaskThread::~CTaskThread()
{

}

bool CTaskThread::PrepareToRun()
{
	return true;
}

void CTaskThread::Run()
{
	while(!IsStoped())
	{
		m_pScheduler->ConsumeTask();
	}
}

CThreadScheduler::CThreadScheduler(std::string signature) :	m_Signature(signature)
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

}

inline CThreadScheduler::~CThreadScheduler()
{
}

bool CThreadScheduler::Init(size_t threads)
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	debug_timer.BeginTimer(nNow, 10 * 1000);
	for (size_t i = 0; i < threads; ++i)
	{
		CSafePtr<CTaskThread> pTaskThread = new CTaskThread(this);
		pTaskThread->CreateThread();
		m_Workers.emplace_back(pTaskThread);
	}
	return true;
}

void CThreadScheduler::ConsumeTask()
{
	bool bHasTask = false;
	while (true)
	{
		std::shared_ptr<CThreadTask> pTask;
		{
			std::lock_guard<std::mutex> guard(m_queue_mutex);
			if (m_Tasks.empty())
			{
				break;
			}
			pTask = std::move(this->m_Tasks.front());
			this->m_Tasks.pop();
		}
		if (pTask != NULL)
		{
			bHasTask = true;
			try
			{
				{
					std::lock_guard<std::mutex> guard(g_thread_data.task_mutex);
					g_thread_data.curren_task = pTask;
				}
				pTask->Execute();
			}
			catch (std::exception* e)
			{
				pTask->OnFailed();
				continue;
			}

			try
			{
				pTask->OnFinish();
			}
			catch (std::exception* e)
			{

			}
		}
	}
}

void CThreadScheduler::DebugTask()
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	if (debug_timer.IsTimeout(nNow))
	{
		int nSize = 0;
		{
			std::lock_guard<std::mutex> guard(m_queue_mutex);
			nSize = m_Tasks.size();
		}
		CACHE_LOG(THREAD_CACHE, "Thread task queuesize = {}", nSize);

		for (size_t i = 0; i < m_Workers.size(); ++i)
		{
			std::lock_guard<std::mutex> guard(g_thread_data.task_mutex);
			if (m_Workers[i]->GetThreadData() != NULL)
			{
				std::shared_ptr<CThreadTask> pTask = m_Workers[i]->GetThreadData()->curren_task;
				if (pTask != NULL)
				{
					CACHE_LOG(THREAD_CACHE, "Thread run task signature = {}", pTask->GetSignature());
				}
			}
		}
	}
}