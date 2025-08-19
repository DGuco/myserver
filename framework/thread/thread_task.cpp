#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature)
{
	SetState(enTaskState::eTaskInit);
	SetCombineType(enCombineType::eCombineInit);
	m_combineCount = 0;
	m_combineDone = 0;
	m_pCombineTask = NULL;
	m_pCombinedArgs = NULL;
};

CThreadTask::~CThreadTask()
{
	try 
	{
		enCombineType bType = GetCombineType();
		enTaskState bState = GetState();
		//等待执行CombineTaskDone
		if (bType == enCombineType::eCombineAccept || bType == enCombineType::eCombineApply)
		{
			{
				CSafeLock guard(m_combineLock);
				if (m_pCombineTask != NULL &&
					bType == enCombineType::eCombineAccept &&
					bState == enTaskState::eTaskDone)
				{
					m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
				}
			}
			if (bState == enTaskState::eTaskDone)
			{
				m_pCombineTask->CombineTaskDone();
			}
			else
			{
				m_pCombineTask->OnFailed();
			}
			SetCombineType(enCombineType::eCombineDone);
		}
		RunChildTask();
		m_pCombinedArgs.Free();
	}
	catch(std::exception e)
	{}
}

void CThreadTask::SetCombineCount(BYTE value)
{
	m_combineCount = value;
	store_release(m_combineDone,(unsigned char)(0));
}

void CThreadTask::CombineTaskDone()
{
	enTaskState bState = GetState();
	//其中有一个前置任务失败了，后续的不用执行了
	if (bState == enTaskState::eTaskFailed)
	{
		return;
	}
	
	// fetch_add(1) 保证原子性递增
    const int oldValue = m_combineDone.fetch_add(1, std::memory_order_acq_rel);
    const int newValue = oldValue + 1;
    // 修改判断条件为严格相等
    if (newValue == m_combineCount) 
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
	CSafeLock guard(m_childTaskLock);
	m_childTaskQueue.push(pTask);
	// if (!m_childTaskQueue.push(pTask).success)
	// {
	// 	CACHE_LOG(ERROR_CACHE, "task is full,can not add");
	// 	ASSERT_EX(false, "task is full,can not add");
	// }
}

void CThreadTask::OnFinish()
{
	enCombineType bType = GetCombineType();
	//等待执行CombineTaskDone
	if(bType == enCombineType::eCombineAccept || bType == enCombineType::eCombineApply)
	{
		{
			CSafeLock guard(m_combineLock);
			if (m_pCombineTask != NULL && bType == enCombineType::eCombineAccept)
			{
				m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
			}
		}
		m_pCombineTask->CombineTaskDone();
		SetCombineType(enCombineType::eCombineDone);
	}
	SetState(enTaskState::eTaskDone);
	RunChildTask();
}

void CThreadTask::OnFailed() 
{
	enCombineType bType = GetCombineType();
	//等待执行CombineTaskDone
	if (bType == enCombineType::eCombineAccept || bType == enCombineType::eCombineApply)
	{
		m_pCombineTask->OnFailed();
		SetCombineType(enCombineType::eCombineDone);
	}
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
			ExecuteChildTask(pTask);
		}
	}
}

void CThreadTask::SetCombineTask(TaskPtr ptr,enCombineType combineType)
{
	enCombineType bType = GetCombineType();
	if (bType != enCombineType::eCombineInit)
	{
		ASSERT_EX(false, "This task {%s} has been combined", m_TaskSignature.c_str());
	}
	enTaskState bState = GetState();
	{
		CSafeLock guard(m_combineLock);
		m_pCombineTask = ptr;
		if (bState == enTaskState::eTaskDone)
		{
			if (m_pCombinedArgs != NULL && combineType == enCombineType::eCombineAccept)
			{
				m_pCombinedArgs->FillWaitTaskParm(GetShared(), m_pCombineTask);
			}
		}
	}
	//此时任务已经完成，则直接CombineTaskDone
	if (bState == enTaskState::eTaskDone)
	{
		m_pCombineTask->CombineTaskDone();
		SetCombineType(enCombineType::eCombineDone);
	}
	//此时任务失败，则直接OnFailed
	else if (bState == enTaskState::eTaskFailed)
	{
		m_pCombineTask->OnFailed();
		SetCombineType(enCombineType::eCombineDone);
	}
	else  //任务还没有完成，正在执行中，标记任务等待执行CombineTaskDone
	{
		SetCombineType(combineType);
	}
}