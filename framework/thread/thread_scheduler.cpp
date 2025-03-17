#include "thread_scheduler.h"

CThreadScheduler::CThreadScheduler(std::string signature)
	:m_Signature(signature)
{}

CThreadScheduler::~CThreadScheduler()
{
	for (size_t i = 0; i < m_Workers.size(); ++i)
	{
		m_Workers[i]->Stop();
		m_Workers[i]->Join();
		m_Workers[i].Free();
		m_Workers[i] = NULL;
	}
	m_Workers.clear();
}

bool CThreadScheduler::Init(size_t threads)
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	debug_timer.BeginTimer(nNow, THREAD_TASK_DEBUG_TIME);
	for (size_t i = 0; i < threads; ++i)
	{
		CSafePtr<CTaskThread> pTaskThread = new CTaskThread(this);
		pTaskThread->CreateThread();
		m_Workers.emplace_back(pTaskThread.DynamicCastTo<CMyThread>());
	}
	return true;
}

void CThreadScheduler::ConsumeTask()
{
	while (true)
	{
		TaskPtr pTask;
		{
			CSafeLock guard(m_queue_mutex);
			if (m_Tasks.empty())
			{
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
					//tm* logTime = localtime(&logtime);
					std::tm logTime = CTimeHelper::LocalTime(logtime);
					CACHE_LOG(THREAD_CACHE, "Thread[{}] running task[{}],task begintime[{}-{}-{} {}:{}:{}.{}] ",i, pTask->GetSignature(),
						logTime.tm_year + 1900, logTime.tm_mon + 1, logTime.tm_mday, logTime.tm_hour, logTime.tm_min, logTime.tm_sec, startTime % 1000);
				}
			}
		}
		CACHE_LOG(THREAD_CACHE, "=========================End================================");
	}
}

void CThreadScheduler::PushTask(TaskPtr pTask)
{
	CSafeLock guard(m_queue_mutex);
	m_Tasks.push(pTask);
}
