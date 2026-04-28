/*****************************************************************
* FileName:my_thread.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __THREAD_H__
#define __THREAD_H__

#include <mutex>
#include <atomic>
#include <functional>
#include <memory>
#include "base.h"
#include "my_lock.h"
#include "time_helper.h"
#include "safe_pointer.h"

class CMyThread;
class CTask;
class CTaskScheduler;

struct thread_data
{
	std::tm 						m_CacheTime;	
	TimePoint						m_CacheTimePoint;
	CSafePtr<CTaskScheduler>		own_scheduler;
};

typedef std::function<void(void*)> ThreadFuncParam;
struct ThreadFuncParamWrapper 
{
	ThreadFuncParam func;
	void* args;
	ThreadFuncParamWrapper(): func(), args(NULL) {}
	ThreadFuncParamWrapper(ThreadFuncParam f, void* a) : func(f), args(a) {}
	void operator()() const 
	{
		if(func != NULL)
		{
			func(args); 
		}
	}
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
	virtual bool			PrepareEnd() = 0;
	virtual void			Run() = 0;
	virtual void			Exit();
	void					Stop();
	void 					Join();
	bool					CreateThread();
	void					SetThreadData(CSafePtr<thread_data> pdata);
	CSafePtr<thread_data>	GetThreadData();
	bool					IsStoped();
	void					SetThreadInitFunc(ThreadFuncParamWrapper func);
	void 					SetThreadTickFunc(ThreadFuncParamWrapper func);
public:
	TID getTID() { return m_TID; }

	ThreadStatus getStatus() { return m_Status; }
	void SetStatus(ThreadStatus status) { m_Status = status; }
private:
	TID								m_TID;
	CSafePtr<thread_data>			m_ThreadData;
	CACHE_LINE_ALIGN ThreadStatus	m_Status;
	std::atomic_bool				m_bStoped;
#if defined(__LINUX__)
	pthread_t						m_hThread;
	pthread_attr_t					m_stAttr;  // 添加这一行
#else
	HANDLE							m_hThread;
#endif
protected:
	ThreadFuncParamWrapper			m_funcInit;
	ThreadFuncParamWrapper			m_funcTick;
};

#if defined(__LINUX__)
void* ThreadProc(void* pvArgs);
#else
DWORD WINAPI ThreadProc(void* pvArgs);
#endif
#endif //__THREAD_H__
