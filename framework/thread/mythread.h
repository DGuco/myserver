#ifndef _MY_THREAD_HPP_
#define _MY_THREAD_HPP_

#include <pthread.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#define TRACE_DEBUG        ThreadLogDebug
#define TRACE_INFO            ThreadLogInfo
#define TRACE_NOTICE        ThreadLogNotice
#define TRACE_WARN        ThreadLogWarn
#define TRACE_ERROR        ThreadLogError
#define TRACE_FATAL        ThreadLogFatal


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
	CMyThread();
	virtual ~CMyThread();

	virtual int PrepareToRun() = 0;
	virtual int RunFunc() = 0;
	virtual bool IsToBeBlocked() = 0;

	int Run();
	int WakeUp();
	int StopThread();
	void ThreadLogInit(char *sPLogBaseName, long lPMaxLogSize, int iPMaxLogNum, int iShow, int iLevel = 0);
	void Join();
protected:
	int CondBlock();
	void ThreadLogDebug(const char *sFormat, ...);
	void ThreadLogInfo(const char *sFormat, ...);
	void ThreadLogNotice(const char *sFormat, ...);
	void ThreadLogWarn(const char *sFormat, ...);
	void ThreadLogError(const char *sFormat, ...);
	void ThreadLogFatal(const char *sFormat, ...);

public:
	int m_iRunStatus;
	char m_abyRetVal[64];
	TLogCfg m_stLogCfg;

private:
	std::mutex m_condMut;
	std::condition_variable data_cond;
	std::thread m_th;
};


#endif
