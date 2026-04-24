#include "task.h"
#include "thread_scheduler.h"

CTask::CTask(CSafePtr<CTaskScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature)
{
	SetState(enTaskState::eTaskInit);
	m_pCombinedArgs = NULL;
};

CTask::~CTask()
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

void CTask::AddChildTask(TaskPtr pTask)
{
	CSafeLock guard(m_childTaskLock);
	m_childTaskQueue.push(pTask);
}

void CTask::OnFinish()
{
	SetState(enTaskState::eTaskDone);
	RunChildTask();
}

void CTask::OnFailed() 
{
	SetState(enTaskState::eTaskFailed);
	RunChildTask();
	CACHE_LOG(THREAD_ERROR, "Task[{}] execute failed", m_TaskSignature);
}

void CTask::Run()
{
	try
	{
		//如果就在当前的执行shcheler中，直接执行
		if(g_thread_data.own_scheduler == m_pScheduler)
		{
			SetState(enTaskState::eTaskDoing);
			SetStartTime(CTimeHelper::GetSingletonPtr()->GetMSTime());
			Execute();
			OnFinish();
		}else//不在当前的执行shcheler中，push到对应scheduler的队列中
		{
			m_pScheduler->PushTask(GetShared());
		}
	}
	catch (std::exception e)
	{
		CACHE_LOG(THREAD_ERROR, "Task[{}] caught exception,exception msg:{}",m_TaskSignature,e.what());
		OnFailed();
	}
}

void CTask::RunChildTask()
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
			if(pTask->CombinedType() != enCombineType::eCombineNone)
			{
				pTask->CombineTaskDone(GetShared());
			}else
			{
				ExecuteChildTask(pTask);
			}
		}
	}
}

void CTask::SetAcceptCombineInfo(CSafePtr<IArgsHolder> pArgs)
{
	if (m_pCombinedArgs != NULL)
	{
		ASSERT_EX(false, "This Task has combined once");
	}
	m_pCombinedArgs = pArgs;
}

void CTask::FillCombineTaskArgs(TaskPtr pChildTask)
{
	if (m_pCombinedArgs != NULL)
	{
		m_pCombinedArgs->FillWaitTaskParm(GetShared(),pChildTask);
	}
}
