#include <string.h>
#include "base.h"
#include "mythread.h"
#include "log.h"

void *ThreadProc(void *pvArgs)
{
	if (!pvArgs) {
		return NULL;
	}

	CMyThread *pThread = (CMyThread *) pvArgs;
	pThread->RunFunc();

	return NULL;
}

CMyThread::CMyThread(const std::string &threadName)
	: m_sThreadName(threadName)
{
	m_iRunStatus = rt_init;
	memset((void *) &m_stLogCfg, 0, sizeof(m_stLogCfg));
}

CMyThread::~CMyThread()
{
}

int CMyThread::Run()
{
	m_iRunStatus = rt_running;
	//创建线程
	m_th = std::thread(ThreadProc, (void *) this);
	return 0;
}

int CMyThread::CondBlock()
{
	// 该过程需要在线程锁内完成
	std::unique_lock<std::mutex> lk(m_condMut);
	// 如果线程需要停止则终止线程
	if (m_iRunStatus == rt_stopped) {
		//退出线程
		LOG_DEBUG("default", "Thread [{}] exit.", m_sThreadName);
		pthread_exit((void *) m_abyRetVal);
	}
	m_iRunStatus = rt_blocked;
	LOG_DEBUG("default", "Thread [{}] would blocked.", m_sThreadName);
	data_cond.wait(lk, [this]
	{ return !IsToBeBlocked(); });

	if (m_iRunStatus != rt_running) {
		LOG_DEBUG("default", "Thread [{}] waked up.", m_sThreadName);
	}
	// 线程状态变为rt_running
	m_iRunStatus = rt_running;
	return 0;
}

int CMyThread::WakeUp()
{
	// 该过程需要在线程锁内完成
	std::lock_guard<std::mutex> guard(m_condMut);

	if (!IsToBeBlocked() && m_iRunStatus == rt_blocked) {
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
	Join();
	LOG_DEBUG("default", "Thread [{}] stopped.", m_sThreadName);
	return 0;
}

void CMyThread::StopForce()
{
	//退出线程
	LOG_DEBUG("default", "Thread [{}] exit.", m_sThreadName);
	pthread_exit((void *) m_abyRetVal);
}

void CMyThread::Join()
{
	if (m_th.joinable()) {
		m_th.join();
	}
}

int CMyThread::GetStatus()
{
	return m_iRunStatus;
}
