#include "thread_task.h"
#include "thread_scheduler.h"

CThreadTask::CThreadTask(CSafePtr<CThreadScheduler> scheduler, std::string signature)
	:m_pScheduler(scheduler),
	m_TaskSignature(signature),
	m_childTaskVec(5)
{};

CThreadTask::~CThreadTask()
{
	RunChildTask();
}

void CThreadTask::AddChildTask(TaskPtr pTask)
{
	if (!m_childTaskVec.Push(pTask).success)
	{
		CACHE_LOG(ERROR_CACHE, "task is full,can not add");
		throw std::runtime_error("task is full,can not add");
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
		Execute();
		bFinish = true;
	}
	catch (std::exception e)
	{
		OnFailed();
		CACHE_LOG(THREAD_ERROR, "Task caught exception,signature : {},msg: {}", e.what());
	}

	if (bFinish)
	{
		OnFinish();
		RunChildTask();
		{
			CSafeLock guard(g_thread_data.task_mutex);
			g_thread_data.curren_task = NULL;
		}
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
			ExecuteChild(pTask);
		}
	}
}