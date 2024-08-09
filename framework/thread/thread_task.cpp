#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature),
	m_childTaskVec(5)
{
	SetState(enTaskState::eTaskInit);
	m_waitCount = 0;
	m_waitDone = 0;
	m_WaitTask = NULL;
};

CThreadTask::~CThreadTask()
{
	RunChildTask();
}

void CThreadTask::SetWaitTask(TaskPtr ptr, int value) 
{
	m_WaitTask = ptr; 
	m_waitCount = value;
}

void CThreadTask::AddWaitDone()
{
	int oldValue = m_waitDone.fetch_add(1);
	if (m_waitDone == m_waitCount)
	{

	}
}

void CThreadTask::AddChildTask(TaskPtr pTask)
{
	if (!m_childTaskVec.Push(pTask).success)
	{
		CACHE_LOG(ERROR_CACHE, "task is full,can not add");
		throw std::runtime_error("task is full,can not add");
	}
}

void CThreadTask::OnFinish()
{
	SetState(enTaskState::eTaskDone); 
	RunChildTask();
}

void CThreadTask::OnFailed() 
{
	SetState(enTaskState::eTaskFailed);
	CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
	RunChildTask();
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
	LockFreeResult result;
	while (true)
	{
		result = m_childTaskVec.Pop(pTask);
		if (!result.success)
		{
			break;
		}
		if (pTask != NULL)
		{
			ExecuteChildTask(pTask);
		}
	}
}