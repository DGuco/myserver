//
// mythread.cpp
// �̻߳���
// Created by DGuco on 17-10-12.
// Copyright ? 2016�� DGuco. All rights reserved.
//

#include "base.h"
#include "mythread.h"
#include "log.h"

CMyThread::CMyThread(const std::string &threadName,
					 long timeOut)
	: m_iRunStatus(rt_init),
	  m_pThread(NULL),
	  m_sThreadName(threadName),
	  m_lTimeOut(timeOut)
{
}

CMyThread::~CMyThread()
{
}

int CMyThread::Run()
{
	m_iRunStatus = rt_running;
	//�����߳�
	if (m_pThread != NULL) 
	{
		m_pThread = std::make_shared<thread>(mem_fn(&CMyThread::ThreadFunc), this);
	}
	return 0;
}

void CMyThread::ThreadFunc()
{
	while (true) 
	{
		std::unique_lock<std::mutex> lk(m_condMut);
		// �ù�����Ҫ���߳��������
		m_iRunStatus = rt_blocked;
		//��ʱ
		if (m_lTimeOut > 0) 
		{
			data_cond.wait_for(lk, std::chrono::microseconds(m_lTimeOut),
							   [this]
							   {
								   return (m_iRunStatus == rt_stopped || !IsToBeBlocked());
							   });
		}
		else
		{
			std::unique_lock<std::mutex> lk(m_condMut);
			data_cond.wait(lk, [this]
			{
				return (m_iRunStatus == rt_stopped || !IsToBeBlocked());
			});
		}
		// ����߳���Ҫֹͣ����ֹ�߳�
		if (m_iRunStatus == rt_stopped)
		{
			//�˳��߳�
			DISK_LOG(DEBUG_DISK, "Thread [{}] exit.", m_sThreadName);
			//pthread_exit((void *) 0);
		}

		//���⻽���߳�,��������
		if (IsToBeBlocked()) 
		{
			continue;
		}
		// �߳�״̬��Ϊrt_running
		m_iRunStatus = rt_running;
		lk.unlock();
		RunFunc();
	}
}

int CMyThread::WakeUp()
{
	std::lock_guard<std::mutex> lk(m_condMut);
	if (!IsToBeBlocked() && m_iRunStatus == rt_blocked) 
	{
		std::lock_guard<std::mutex> guard(m_condMut);
		// ���̷߳����ź��Ի���
		data_cond.notify_one();
	}

	return 0;
}

int CMyThread::StopThread()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	m_iRunStatus = rt_stopped;
	data_cond.notify_one();
	// �ȴ����߳���ֹ
	Join();
	DISK_LOG(DEBUG_DISK, "Thread [{}] stopped.", m_sThreadName);
	return 0;
}

void CMyThread::StopForce()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	//�˳��߳�
	DISK_LOG(DEBUG_DISK, "Thread [{}] exit.", m_sThreadName);
	//pthread_exit((void *) 0);
}

void CMyThread::Join()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	if (m_pThread->joinable())
	{
		m_pThread->join();
	}
}

int CMyThread::GetStatus()
{
	std::lock_guard<std::mutex> guard(m_condMut);
	return m_iRunStatus;
}