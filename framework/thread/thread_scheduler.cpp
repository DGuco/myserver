#include "thread_scheduler.h"

CThreadScheduler::CThreadScheduler(std::string signature)
	:m_Signature(signature)
{}

CThreadScheduler::~CThreadScheduler()
{}

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
	bool needsleep = false;
	while (true)
	{
		if (needsleep)
		{
			needsleep = false;
			SLEEP(10);
		}
		TaskPtr pTask;
		{
			CSafeLock guard(m_queue_mutex);
			if (m_Tasks.empty())
			{
				needsleep = true;
				break;
			}
			pTask = this->m_Tasks.front();
			this->m_Tasks.pop();
		}
		if (pTask != NULL)
		{
			pTask->Run();
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
			CSafeLock guard(m_queue_mutex);
			nSize = m_Tasks.size();
		}
		CACHE_LOG(THREAD_CACHE, "=========================Begin===============================");
		CACHE_LOG(THREAD_CACHE, "Scheduler[{}] : Thread task queuesize = {}",m_Signature,nSize);
		for (size_t i = 0; i < m_Workers.size(); ++i)
		{
			if (m_Workers[i]->GetThreadData() != NULL)
			{
				CSafeLock guard(m_Workers[i]->GetThreadData()->task_mutex);
				TaskPtr pTask = m_Workers[i]->GetThreadData()->curren_task;
				if (pTask != NULL)
				{
					time_t startTime = pTask->GetStartTime();
					time_t logtime = startTime / 1000;
					//localtime非线程安全
					tm* p = localtime(&logtime);
					CACHE_LOG(THREAD_CACHE, "Thread[{}] running task[{}],task begintime[{}-{}-{} {}:{}:{}.{}] ",i, pTask->GetSignature(),
						p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, startTime % 1000);
				}
			}
		}
		CACHE_LOG(THREAD_CACHE, "=========================End================================");
	}
}

void CThreadScheduler::PushTask(TaskPtr pTask)
{
	std::lock_guard<std::mutex> guard(m_queue_mutex);
	m_Tasks.push(pTask);
};