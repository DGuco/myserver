#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature),
	m_childTaskVec(5)
{
	SetState(enTaskState::eTaskInit);
	SetCombineType(enCombineType::eCombineNone);
	m_combineCount = 0;
	m_combineDone = 0;
	m_pCombineTask = NULL;
	m_pCombinedArgs = NULL;
};

CThreadTask::~CThreadTask()
{
	RunChildTask();
	m_pCombinedArgs.Free();
}

void CThreadTask::SetCombineTask(TaskPtr ptr)
 {
	enCombineType bType = GetCombineType();
	if (bType == enCombineType::eCombineAccept)
	{
		CSafeLock guard(m_combineLock);
		m_pCombineTask = ptr;
		enTaskState bState = GetState();
		if (bState == enTaskState::eTaskDone || bState == enTaskState::eTaskFailed)
		{
			if (bState == enTaskState::eTaskDone)
			{
				if (m_pCombinedArgs != NULL)
				{
					m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
				}
				m_pCombineTask->CombineTaskDone();
			}
			else
			{
				m_pCombineTask->OnFailed();
			}
		}
	}
	else if(bType == enCombineType::eCombineApply)
	{
		{
			CSafeLock guard(m_combineLock);
			m_pCombineTask = ptr;
		}
		enTaskState bState = GetState();
		if (bState == enTaskState::eTaskDone || bState == enTaskState::eTaskFailed)
		{
			if (bState == enTaskState::eTaskDone)
			{
				m_pCombineTask->CombineTaskDone();
			}
			else
			{
				m_pCombineTask->OnFailed();
			}
		}
	}

}

void CThreadTask::SetCombineCount(BYTE value)
{
	m_combineCount.store(memory_order_release);
}

void CThreadTask::CombineTaskDone()
{
	try
	{

		CACHE_LOG(THREAD_ERROR, "Task[{}] CombineTaskDone", m_TaskSignature);
		int oldValue = m_combineDone.fetch_add(1);
		//最后一个完成
		int nTmCombineCount = m_combineCount.load(memory_order_acquire);
		int nCombineDone = m_combineDone.load(memory_order_acquire);
		if (nCombineDone == nTmCombineCount && oldValue == nTmCombineCount - 1)
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
	catch (std::exception* e)
	{
		int a = 1;
	}

}

void CThreadTask::AddChildTask(TaskPtr pTask)
{
	if (!m_childTaskVec.Push(pTask).success)
	{
		CACHE_LOG(ERROR_CACHE, "task is full,can not add");
		ASSERT_EX(false, "task is full,can not add");
	}
}

void CThreadTask::OnFinish()
{
	enCombineType bType = GetCombineType();
	if (bType == enCombineType::eCombineNone)
	{
		SetState(enTaskState::eTaskDone);
		RunChildTask();
	}
	else if(bType == enCombineType::eCombineAccept)
	{
		CSafeLock guard(m_combineLock);
		SetState(enTaskState::eTaskDone);
		RunChildTask();
		if (m_pCombineTask != NULL)
		{
			if (m_pCombinedArgs != NULL)
			{
				m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
			}
			m_pCombineTask->CombineTaskDone();
		}
	}
	else if (bType == enCombineType::eCombineApply)
	{

	}
}

void CThreadTask::OnFailed() 
{
	if (m_pCombinedArgs == NULL)
	{
		SetState(enTaskState::eTaskFailed);
		CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
		RunChildTask();
	}
	else
	{
		CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
		RunChildTask();
		if (m_pCombineTask != NULL)
		{
			m_pCombineTask->OnFailed();
		}
	}

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