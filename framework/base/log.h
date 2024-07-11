/*****************************************************************
* FileName:log.h
* Summary :
* Date	  :2024-2-27
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __LOG_H__
#define __LOG_H__

#include "singleton.h"
#include "log_def.h"

using namespace std;
using namespace spdlog;
using namespace spdlog::level;

#define  CONSOLE_LOG_NAME "console"
#define  MAX_SPDLOG_QUEUE_SIZE (102400)
#define  MAX_SPDLOG_THREAD_POOL (4)

class CLog : public CSingleton<CLog>
{
public:
	CLog();
	~CLog();
	bool Init(const char* modulename);
	//关闭所有日志
	int ShutDownAll();
	template<typename... Args>
	int DiskLog(enDiskLog log_type, const char* vFmt, const Args &... args);
	template<typename... Args>
	int CacheLog(enCacheLog log_type, const char* vFmt, const Args &... args);
private:
	stLogInfo* GetLogInfo(int log_type);
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 */
	int InitHourLog(const char* vLogName, const char* vLogDir, level_enum level,bool async = false);
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vAppend 是否截断(默认即可)
	 */
	int InitBaseLog(const char* vLogName,const char* vLogDir,level_enum level,bool vAppend = true, bool async = false);
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vMaxFileSize 回卷文件最大长度
	 * vMaxFileSize 回卷文件个数
	 */
	int Init_Roating_Log(const char* vLogName,const char* vLogDir,level_enum level,int vMaxFileSize,int vMaxBackupIndex, bool async = false);
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vMaxFileSize 回卷文件最大长度
	 * vMaxFileSize 回卷文件个数
	 */
	int Init_Daily_Log(const char* vLogName, const char* vLogDir,level_enum level,int hour = 0,int minute = 0, bool async = false);
};

template<typename... Args>
int CLog::DiskLog(enDiskLog log_type, const char* vFmt, const Args &... args)
{
	stLogInfo* pLogInfo = GetLogInfo((int)log_type);
	if (pLogInfo == NULL)
	{
		return -1;
	}
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(pLogInfo->level, vFmt, args...);
	}
#endif
	auto log = spdlog::get(pLogInfo->logName);
	if (NULL == log)
	{
		return -1;
	}
	log->log(pLogInfo->level, vFmt, args...);
	return 0;
}

template<typename... Args>
int CLog::CacheLog(enCacheLog log_type, const char* vFmt, const Args &... args)
{
	stLogInfo* pLogInfo = GetLogInfo((int)log_type);
	if (pLogInfo == NULL)
	{
		return -1;
	}
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(pLogInfo->level, vFmt, args...);
	}
#endif

	auto log = spdlog::get(pLogInfo->logName);
	if (NULL == log) 
	{
		return -1;
	}
	log->log(pLogInfo->level, vFmt, args...);
	return 0;
}

#define INIT_LOG(modulename)  CLog::GetSingletonPtr()->Init(modulename)
#define DISK_LOG  CLog::GetSingletonPtr()->DiskLog
#define CACHE_LOG  CLog::GetSingletonPtr()->CacheLog
#define SHUTDOWN_ALL_LOG()  CLog::GetSingletonPtr()->ShutDownAll()

#endif //__LOG_H__

