/*****************************************************************
* FileName:thread.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __THREAD_H__
#define __THREAD_H__

#include <mutex>
#include <atomic>
#include "base.h"
#include "my_lock.h"
#include "time_helper.h"
#include "safe_pointer.h"

class CMyThread;
class CThreadTask;
class CThreadScheduler;
typedef std::shared_ptr<CThreadTask> TaskPtr;
struct thread_data
{
	CMyLock							task_mutex;
	TaskPtr							curren_task;
	CSafePtr<CThreadScheduler>		own_scheduler;
};

extern thread_local thread_data g_thread_data;

class CMyThread
{
public:
	enum ThreadStatus
	{
		READY,		// 当前线程处于准备状态
		RUNNING,	// 处于运行状态
		EXITING,	// 线程正在退出
		EXIT		// 已经退出 
	};
public:
	CMyThread();
	virtual ~CMyThread();
	virtual bool			PrepareToRun() = 0;
	virtual void			Run() = 0;
	virtual void			Exit();
	void					Stop();
	bool					CreateThread();
	void					SetThreadData(CSafePtr<thread_data> pdata);
	CSafePtr<thread_data>	GetThreadData();
	bool					IsStoped();
public:
	TID getTID() { return m_TID; }

	ThreadStatus getStatus() { return m_Status; }
	VOID SetStatus(ThreadStatus status) { m_Status = status; }
private:
	TID								m_TID;
	CSafePtr<thread_data>			m_ThreadData;
	CACHE_LINE_ALIGN ThreadStatus	m_Status;
	std::atomic_bool				m_bStoped;
#if defined(__LINUX__)
	pthread_t						m_hThread;
#else
	HANDLE							m_hThread;
#endif
};

#if defined(__LINUX__)
void* ThreadProc(void* pvArgs);
#else
DWORD WINAPI ThreadProc(void* pvArgs);
#endif


class CTaskThread : public CMyThread
{
public:
	CTaskThread(CSafePtr<CThreadScheduler> scheduler)
		: m_pScheduler(scheduler)
	{}
	virtual ~CTaskThread()
	{}
	virtual bool PrepareToRun()
	{
		g_thread_data.own_scheduler = m_pScheduler;
		return true;
	}
	void Run();
private:
	CSafePtr<CThreadScheduler>	m_pScheduler;
};
#endif //__THREAD_H__
