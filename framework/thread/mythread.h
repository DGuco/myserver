//
// Created by DGuco on 17-10-12.
// 线程基类
//
#ifndef _MY_THREAD_HPP_
#define _MY_THREAD_HPP_

#include <pthread.h>
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

typedef struct
{
	char szThreadKey[32];
	char szLogBaseName[200];
	long lMaxLogSize;
	int iMaxLogNum;
} TLogCfg;

void *ThreadProc(void *pvArgs);

class CMyThread
{
public:
	CMyThread(const std::string &threadName);
	virtual ~CMyThread();

	virtual int PrepareToRun() = 0;
	virtual int RunFunc() = 0;
	virtual bool IsToBeBlocked() = 0;

	int Run();
	int WakeUp();
	int StopThread();
	void StopForce();
	void Join();
	int GetStatus();
protected:
	int CondBlock();
public:
	int m_iRunStatus;
	char m_abyRetVal[64];
	TLogCfg m_stLogCfg;

private:
	std::mutex m_condMut;
	std::condition_variable data_cond;
	std::thread m_th;
	std::string m_sThreadName;
};


#endif
