#include "task_scheduler.h"

CTaskScheduler::CTaskScheduler(std::string signature)
	:m_Signature(signature)
{}

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
    // //如果就在当前的执行shcheler中，直接执行
    // if (g_thread_data.own_scheduler == pTask->GetScheduler())
    // {
    // 	pTask->Run();
    // }else
    // {
    // 	PushTask(pTask);
    // }
}
