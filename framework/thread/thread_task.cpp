#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature)
{
	SetState(enTaskState::eTaskInit);
	m_pCombinedArgs = NULL;
};

CThreadTask::~CThreadTask()
{
	try 
	{
		enTaskState bState = GetState();
		if(bState == enTaskState::eTaskDone || bState == enTaskState::eTaskFailed)
		{
			RunChildTask();
		}
		m_pCombinedArgs.Free();
	}
	catch(std::exception e)
	{
		m_pCombinedArgs.Free();
	}
}

//添加到执行队列
void CThreadTask::AddToSchedulerQueue()
{
	if(m_pScheduler != NULL)
	{
		m_pScheduler->Schedule(GetShared());
	}
}

void CThreadTask::AddChildTask(TaskPtr pTask)
{
	CSafeLock guard(m_childTaskLock);
	m_childTaskQueue.push(pTask);
}

void CThreadTask::OnFinish()
{
	SetState(enTaskState::eTaskDone);
	RunChildTask();
}

void CThreadTask::OnFailed() 
{
	SetState(enTaskState::eTaskFailed);
	RunChildTask();
	CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
}

void CThreadTask::Run()
{
	bool bFinish = false;
	try
	{
		{
			CSafeLock guard(g_thread_data.task_mutex);
			g_thread_data.curren_task = GetShared();
		}
		SetState(enTaskState::eTaskDoing);
		SetStartTime(CTimeHelper::GetSingletonPtr()->GetMSTime());
		Execute();
		OnFinish();
	}
	catch (std::exception e)
	{
		CACHE_LOG(THREAD_ERROR, "Task[{}] caught exception,exception msg:{}",m_TaskSignature,e.what());
		OnFailed();
	}

	{
		CSafeLock guard(g_thread_data.task_mutex);
		g_thread_data.curren_task = NULL;
	}
}

void CThreadTask::RunChildTask()
{
	TaskPtr pTask;
	while (true)
	{
		{
			CSafeLock guard(m_childTaskLock);
			if (m_childTaskQueue.empty())
			{
				break;
			}
			pTask = m_childTaskQueue.front();
			m_childTaskQueue.pop();
		}
		if (pTask != NULL)
		{
			if(pTask->IsCombinedTask())
			{
				pTask->CombineTaskDone(GetShared());
			}else
			{
				ExecuteChildTask(pTask);
			}
		}
	}
}