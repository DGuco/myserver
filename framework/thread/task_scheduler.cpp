#include "task_scheduler.h"

CTaskScheduler::CTaskScheduler(std::string signature)
	:m_Signature(signature)
{
    time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	debug_timer.BeginTimer(nNow, THREAD_TASK_DEBUG_TIME);
}

CTaskScheduler::~CTaskScheduler()
{
    while (!m_Tasks.empty())
    {
        TaskPtr pTask = m_Tasks.front();
        m_Tasks.pop();
        pTask = NULL;
    }
}

void CTaskScheduler::ConsumeTask()
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
        DebugTask();
	}
}

void CTaskScheduler::PushTask(TaskPtr pTask)
{
	CSafeLock guard(m_queue_mutex);
	m_Tasks.push(pTask);
}

void CTaskScheduler::ScheduleTask(TaskPtr pTask)
{
    if(pTask->GetState() != enTaskState::eTaskInit)
    {
        ASSERT_EX(false,"CThreadScheduler Schedule task failed,the task has been scheduled");
        return;
    }
    pTask->SetState(enTaskState::eTaskWaitingFoDoing);
    PushTask(pTask);
}


void CTaskScheduler::DebugTask()
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
		CACHE_LOG(THREAD_CACHE, "=========================End================================");
	}
}
