#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature),
	m_childTaskVec(5)
{
	SetState(enTaskState::eTaskInit);
	m_combineCount = 0;
	m_combineDone = 0;
	m_WaitTask = NULL;
	m_pTaskArgs = NULL;
};

CThreadTask::~CThreadTask()
{
	RunChildTask();
}

void CThreadTask::SetWaitTask(TaskPtr ptr)
 {
	CSafeLock guard(m_WaitLock);
	m_WaitTask = ptr; 
	if (m_nState == enTaskState::eTaskDone || m_nState == enTaskState::eTaskFailed)
	{
		if (m_pTaskArgs != NULL)
		{
			m_pTaskArgs->FillWaitTaskParm(GetShared(),m_WaitTask);
		}
		m_WaitTask->CombineTaskDone();
	}
}

void CThreadTask::SetCombineCount(BYTE value)
{
	m_combineCount = value;
}

void CThreadTask::CombineTaskDone()
{
	int oldValue = m_combineDone.fetch_add(1, memory_order_acquire);
	//最后一个完成
	if (m_combineDone == m_combineCount && oldValue == m_combineCount - 1)
	{
		//如果就在当前的执行shcheler中，直接执行
		if (g_thread_data.own_scheduler == m_pScheduler)
		{
			Run();
		}
		else
		{
			//push 到对应scheduler的队列中
			m_pScheduler->PushTask(GetShared());
		}
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
	{
		CSafeLock guard(m_WaitLock);
		if (m_WaitTask != NULL)
		{
			if (m_pTaskArgs != NULL)
			{
				m_pTaskArgs->FillWaitTaskParm(GetShared(), m_WaitTask);
			}
			m_WaitTask->CombineTaskDone();
		}
	}
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