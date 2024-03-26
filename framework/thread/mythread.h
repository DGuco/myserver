//
// mythread.h
// �̻߳���
// Created by DGuco on 17-10-12.
// Copyright ? 2016�� DGuco. All rights reserved.
//

#ifndef _MY_THREAD_HPP_
#define _MY_THREAD_HPP_

#include <condition_variable>
#include <mutex>
#include <thread>

enum eRunStatus
{
	rt_init = 0,
	rt_blocked = 1,
	rt_running = 2,
	rt_stopped = 3
};

class CMyThread
{
public:
	CMyThread(const std::string &threadName,
			  long timeOut = 0);
	virtual ~CMyThread();

	virtual int PrepareToRun() = 0;
	virtual void RunFunc() = 0;
	virtual bool IsToBeBlocked() = 0;

	int Run();
	int WakeUp();
	int StopThread();
	void StopForce();
	void Join();
	int GetStatus();
protected:
	void ThreadFunc();
private:
	int m_iRunStatus;
	std::mutex m_condMut;
	std::condition_variable data_cond;
	std::shared_ptr<std::thread> m_pThread;
	std::string m_sThreadName;
	long m_lTimeOut; //������ʱʱ��(΢��)��Ĭ��û�г�ʱ�������wake up��Զ������
};


#endif
