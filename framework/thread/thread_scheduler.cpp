#include "thread_scheduler.h"

CThreadScheduler::CThreadScheduler(std::string signature)
	:CTaskScheduler(signature)
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

bool CThreadScheduler::Init(size_t threads,
							ThreadFuncParam initFunc,
							ThreadFuncParam tickFunc,
							void**		    initFuncArgs,
							void**		    tickFuncArgs)
{
	for (size_t i = 0; i < threads; ++i)
	{
		CSafePtr<CTaskThread> pTaskThread = new CTaskThread(this);
		ThreadFuncParamWrapper initFuncWrapper;
		if(initFuncArgs == NULL)
		{
			initFuncWrapper.args = NULL;	
		}else
		{
			initFuncWrapper.args = initFuncArgs[i];
		}
		initFuncWrapper.func = initFunc;
		pTaskThread->SetThreadInitFunc(initFuncWrapper);
		ThreadFuncParamWrapper tickFuncWrapper;
		if(tickFuncArgs == NULL)
		{
			tickFuncWrapper.args = NULL;
		}else
		{
			tickFuncWrapper.args = tickFuncArgs[i];	
		}
		tickFuncWrapper.func = tickFunc;
		pTaskThread->SetThreadTickFunc(tickFuncWrapper);
		pTaskThread->CreateThread();
		m_Workers.emplace_back(pTaskThread.DynamicCastTo<CMyThread>());
	}
	return true;
}

void CThreadScheduler::StopScheduler()
{
	for (size_t i = 0; i < m_Workers.size(); ++i)
	{
		m_Workers[i]->Stop();
	}
}

void CThreadScheduler::Join()
{
	for (size_t i = 0; i < m_Workers.size(); ++i)
	{
		m_Workers[i]->Join();
	}
}


void CThreadScheduler::ScheduleTask(TaskPtr pTask)
{
	if(pTask->GetState() != enTaskState::eTaskInit)
	{
		ASSERT_EX(false,"CThreadScheduler Schedule task failed,the task has been scheduled");
		return;
	}
	pTask->SetState(enTaskState::eTaskWaitingFoDoing);
	//如果就在当前的执行shcheler中，直接执行
	if (g_thread_data.own_scheduler == pTask->GetScheduler())
	{
		pTask->Run();
	}else
	{
		PushTask(pTask);
	}
}