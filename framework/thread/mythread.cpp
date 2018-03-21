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

CMyThread::CMyThread()
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
	// 线程被阻塞或者停止，这里的while等待主要防止多个线程等待时被意外唤醒，保证当条件满足时，只有一个线程在处理
	while (IsToBeBlocked() || m_iRunStatus == rt_stopped) {
		// 如果线程需要停止则终止线程
		if (m_iRunStatus == rt_stopped) {
			//退出线程
			LOG_DEBUG("default", "Thread exit.");
			pthread_exit((void *) m_abyRetVal);
		}
		LOG_DEBUG("default", "Thread would blocked.");
		m_iRunStatus = rt_blocked;
		// 进入休眠状态,直到条件满足
		data_cond.wait(lk);
	}

	if (m_iRunStatus != rt_running) {
		LOG_DEBUG("default", "Thread waked up.");
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
	LOG_DEBUG("default", "Thread stopped.");
	return 0;
}

void CMyThread::StopForce()
{
	//退出线程
	LOG_DEBUG("default", "Thread exit.");
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
