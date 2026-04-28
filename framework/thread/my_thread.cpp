#include "my_thread.h"
#include "task_scheduler.h"
#include "log.h"
#include "time_helper.h"

thread_local thread_data g_thread_data;

CMyThread::CMyThread()
{
	m_TID = 0;
	m_Status = CMyThread::READY;
	m_bStoped.store(false);
#if defined(__WINDOWS__) || defined(_WIN32)
	m_hThread = NULL;
#endif
}


CMyThread::~CMyThread()
{
}

void CMyThread::Exit()
{
	try
	{
#if defined(__LINUX__)
		pthread_exit(NULL);
#else
		::CloseHandle(m_hThread);
#endif
	}
	catch (std::exception e)
	{

	}
}

void CMyThread::Stop()
{
	m_bStoped.store(true);
}

void CMyThread::Join()
{
#if defined(__LINUX__)
	pthread_join(m_hThread, NULL);	
#else
	::WaitForSingleObject(m_hThread, INFINITE);
#endif
}

bool CMyThread::CreateThread()
{
#if defined(__LINUX__)
	pthread_attr_init(&m_stAttr);
	pthread_attr_setscope(&m_stAttr, PTHREAD_SCOPE_SYSTEM);  // 设置线程状态为与系统中所有线程一起竞争CPU时间
	//pthread_attr_setdetachstate( &m_stAttr, PTHREAD_CREATE_DETACHED );
	pthread_attr_setdetachstate(&m_stAttr, PTHREAD_CREATE_JOINABLE);  // 设置非分离的线程

	pthread_create(&m_hThread, &m_stAttr, ThreadProc, (void*)this);
#else
	m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, &m_TID);
#endif
	return true;
}

void CMyThread::SetThreadData(CSafePtr<thread_data> pdata)
{
	m_ThreadData = pdata;
}

CSafePtr<thread_data> CMyThread::GetThreadData()
{
	return m_ThreadData;
}

bool CMyThread::IsStoped()
{
	return m_bStoped.load() == true;
}

void CMyThread::SetThreadInitFunc(ThreadFuncParamWrapper func)
{
	m_funcInit = func;
}

void CMyThread::SetThreadTickFunc(ThreadFuncParamWrapper func)
{
	m_funcTick = func;	
}

#if defined(__LINUX__)
void* ThreadProc(void* pvArgs)
{
	CMyThread* pThread = (CMyThread*)pvArgs;
	if (pThread == NULL)
		return 0;

	pThread->SetThreadData(CSafePtr<thread_data>(&g_thread_data));
	pThread->SetStatus(CMyThread::RUNNING);
	if (!pThread->PrepareToRun())
	{
		DISK_LOG(ERROR_DISK, "Thread PrepareToRun failed");
		return 0;
	}

	pThread->Run();
	pThread->SetStatus(CMyThread::EXITING);
	pThread->PrepareEnd();
	pThread->Exit();
	pThread->SetStatus(CMyThread::EXIT);
	return 0;	
}
#else
DWORD WINAPI ThreadProc(void* pvArgs)
{
	CMyThread* pThread = (CMyThread*)pvArgs;
	if (pThread == NULL)
		return 0;

	pThread->SetThreadData(CSafePtr<thread_data>(&g_thread_data));
	pThread->SetStatus(CMyThread::RUNNING);
	if (!pThread->PrepareToRun())
	{
		DISK_LOG(ERROR_DISK, "Thread PrepareToRun failed");
		return 0;
	}
	
	pThread->Run();
	pThread->SetStatus(CMyThread::EXITING);
	pThread->PrepareEnd();
	pThread->Exit();
	pThread->SetStatus(CMyThread::EXIT);
	return 0;	
}

#endif