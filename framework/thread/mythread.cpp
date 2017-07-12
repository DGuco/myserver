#include <stdarg.h>
#include <string.h>
#include "../base/base.h"
#include "mythread.h"
#include "../log/log.h"

void* ThreadProc( void *pvArgs )
{
	if( !pvArgs )
	{
		return NULL;
	}

    CMyThread *pThread = (CMyThread *)pvArgs;

	if( pThread->PrepareToRun() )  // handle
	{
		return NULL;
	}

	pThread->Run();

	return NULL;
}

CMyThread::CMyThread()
{
	m_iRunStatus = rt_init;
	memset((void *)&m_stLogCfg, 0, sizeof(m_stLogCfg));
}
CMyThread::~CMyThread()
{
}

int CMyThread::CreateThread()
{
	m_iRunStatus = rt_running;
	//创建线程
	mt = std::thread(ThreadProc, (void *)this );
	return 0;
}

int CMyThread::CondBlock()
{
	// 该过程需要在线程锁内完成
	std::unique_lock<std::mutex> lk(m_condMut);
	// 线程被阻塞或者停止，这里的while等待主要防止多个线程等待时被意外唤醒，保证当条件满足时，只有一个线程在处理
	while( IsToBeBlocked() || m_iRunStatus == rt_stopped )
	{
		// 如果线程需要停止则终止线程
		if( m_iRunStatus == rt_stopped )
		{
			//退出线程
			ThreadLogDebug( "Thread exit.");
			pthread_exit( (void *)m_abyRetVal );
		}
		ThreadLogDebug( "Thread would blocked." );
		m_iRunStatus = rt_blocked;
		// 进入休眠状态,直到条件满足
        data_cond.wait(lk);
	}

	if( m_iRunStatus != rt_running )  
	{
		ThreadLogDebug( "Thread waked up.");
	}

	// 线程状态变为rt_running
	m_iRunStatus = rt_running;
    lk.unlock();
	return 0;
}

int CMyThread::WakeUp()
{
	// 该过程需要在线程锁内完成
    std::lock_guard<std::mutex> guard(m_condMut);

	if( !IsToBeBlocked() && m_iRunStatus == rt_blocked )
    {
		// 向线程发出信号以唤醒
        data_cond.notify_one();
	}

	return 0;
}

int CMyThread::StopThread()
{
    std::lock_guard<std::mutex> guard(m_condMut);
	m_iRunStatus = rt_stopped;
    data_cond.notify_one();
	// 等待该线程终止
	if (mt.joinable()) {
		mt.join();
	}
	ThreadLogDebug("Thread stopped.");

	return 0;
}

void CMyThread::ThreadLogInit(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum, int iShow, int iLevel /*= 0*/)
{
	memset(m_stLogCfg.szLogBaseName, 0, sizeof(m_stLogCfg.szLogBaseName));
	strncpy(m_stLogCfg.szLogBaseName, sPLogBaseName, sizeof(m_stLogCfg.szLogBaseName)-1);
	m_stLogCfg.lMaxLogSize = lPMaxLogSize;
	m_stLogCfg.iMaxLogNum = iPMaxLogNum;
	strncpy( m_stLogCfg.szThreadKey, m_stLogCfg.szLogBaseName, sizeof( m_stLogCfg.szThreadKey )-1 ) ;
	
	INIT_ROLLINGFILE_LOG( m_stLogCfg.szThreadKey, m_stLogCfg.szLogBaseName, (LogLevel) iLevel, m_stLogCfg.lMaxLogSize, m_stLogCfg.iMaxLogNum ); 
}

void CMyThread::ThreadLogDebug(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogDebug_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}

void CMyThread::ThreadLogInfo(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogInfo_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}

void CMyThread::ThreadLogNotice(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogNotice_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}

void CMyThread::ThreadLogWarn(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogWarn_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}

void CMyThread::ThreadLogError(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogError_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}

void CMyThread::ThreadLogFatal(const char *sFormat, ...)
{
	va_list va;
	va_start( va, sFormat );
	LogFatal_va( m_stLogCfg.szThreadKey, sFormat, va );
	va_end( va );
}
