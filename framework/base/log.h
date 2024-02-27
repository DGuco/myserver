//
//  log.h
//  Created by DGuco on 18/02/28.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LOG_DEF_H_
#define _LOG_DEF_H_

#include <string>
#include <stdarg.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "server_tool.h"

using namespace std;
using namespace spdlog;
using namespace spdlog::level;

#define  CONSOLE_LOG_NAME "console"

class CLog : public CSingleton<CLog>
{
public:
	CLog() {};
	~CLog() {};
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vAppend 是否截断(默认即可)
	 */
	int InitBaseLog(const char* vLogName,const char* vLogDir,level_enum level,bool vAppend = true);					   

	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vMaxFileSize 回卷文件最大长度
	 * vMaxFileSize 回卷文件个数
	 */
	int Init_Roating_Log(const char* vLogName,const char* vLogDir,level_enum level,int vMaxFileSize,int vMaxBackupIndex);          
	/**
	 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
	 * vLogDir 文件名称(路径)
	 * level 日志等级
	 * vMaxFileSize 回卷文件最大长度
	 * vMaxFileSize 回卷文件个数
	 */
	int Init_Daily_Log(const char* vLogName, const char* vLogDir,level_enum level,int hour,int minute);
	
	//关闭所有日志
	int ShutDownAll();

	template<typename... Args>
	int LogTrace(const char* vLogName, const char* vFmt, const Args &... args);
	template<typename... Args>
	int LogDebug(const char* vLogName, const char* vFmt, const Args &... args);
	template<typename... Args>
	int LogInfo(const char* vLogName, const char* vFmt, const Args &... args);
	template<typename... Args>
	int LogWarn(const char* vLogName, const char* vFmt, const Args &... args);
	template<typename... Args>
	int LogError(const char* vLogName, const char* vFmt, const Args &... args);
	template<typename... Args>
	int LogCritical(const char* vLogName, const char* vFmt, const Args &... args);
};

// 初始一种类型的日志
int CLog::InitBaseLog(const char* vLogName,const char* vLogDir,level_enum level,bool vAppend)
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}
	auto logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>(vLogName, vLogDir, vAppend);
	if (logger == nullptr)
	{
		return -1;
	}
	logger->set_level(level);
	logger->flush_on(level);
#ifdef __WINDOWS__
	auto console = spdlog::create<spdlog::sinks::stdout_color_sink_mt>(CONSOLE_LOG_NAME);
	console->set_level(level);
	console->flush_on(level);
#endif
	return 0;
}

int CLog::Init_Roating_Log(const char* vLogName,const char* vLogDir,level_enum level,int vMaxFileSize,int vMaxBackupIndex)          /*回卷文件个数*/
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}
	std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::rotating_file_sink_mt>(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
	if (logger == NULL)
	{
		return -1;
	}
	logger->set_level(level);
	logger->flush_on(level);
#ifdef __WINDOWS__
	std::shared_ptr<spdlog::logger> console = spdlog::create<spdlog::sinks::stdout_color_sink_mt>(CONSOLE_LOG_NAME);
	console->set_level(level);
	console->flush_on(level);
#endif
	return 0;
}

int CLog::Init_Daily_Log(const char* vLogName,const char* vLogDir,level_enum level,int hour,int minute)
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}
	std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::daily_file_sink_mt>(vLogName, vLogDir, hour, minute);
	if (logger == NULL)
	{
		return -1;
	}

	logger->set_level(level);
	logger->flush_on(level);

#ifdef __WINDOWS__
	std::shared_ptr<spdlog::logger> console = spdlog::create<spdlog::sinks::stdout_color_sink_mt>(CONSOLE_LOG_NAME);
	console->set_level(level);
	console->flush_on(level);
#endif
	return 0;
}

int CLog::ShutDownAll()
{
	spdlog::drop_all();
	return 0;
}

template<typename... Args>
int CLog::LogTrace(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::trace, vFmt, args...);
	}
#endif
	auto log = spdlog::get(vLogName);
	log->log(level_enum::trace, vFmt, args...);
	if (NULL == vLogName)
	{
		return -1;
	}

	if (NULL == log) {
		return -1;
	}

	return 0;
}

template<typename... Args>
int CLog::LogDebug(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::debug, vFmt, args...);
	}
#endif
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::debug, vFmt, args...);
	return 0;
}

template<typename... Args>
int CLog::LogInfo(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::info, vFmt, args...);
	}
#endif
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::info, vFmt, args...);
	return 0;
}

template<typename... Args>
int CLog::LogWarn(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::warn, vFmt, args...);
	}
#endif
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::warn, vFmt, args...);
	return 0;
}

template<typename... Args>
int CLog::LogError(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::err, vFmt, args...);
	}
#endif
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::err, vFmt, args...);
	return 0;
}

template<typename... Args>
int CLog::LogCritical(const char* vLogName, const char* vFmt, const Args &... args)
{
#ifdef __WINDOWS__
	auto consoleLog = spdlog::get(CONSOLE_LOG_NAME);
	if (consoleLog)
	{
		consoleLog->log(level_enum::critical, vFmt, args...);
	}
#endif
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::critical, vFmt, args...);
	return 0;
}

#define INIT_BASE_LOG CLog::GetSingletonPtr()->InitBaseLog
#define INIT_ROATING_LOG CLog::GetSingletonPtr()->Init_Roating_Log
#define INIT_DAILY_LOG  CLog::GetSingletonPtr()->Init_Daily_Log
#define SHUTDOWN_ALL_LOG CLog::GetSingletonPtr()->ShutDownAll
#define LOG_TRACE  CLog::GetSingletonPtr()->LogTrace
#define LOG_DEBUG  CLog::GetSingletonPtr()->LogDebug
#define LOG_INFO   CLog::GetSingletonPtr()->LogInfo
#define LOG_WARN  CLog::GetSingletonPtr()->LogWarn
#define LOG_ERROR  CLog::GetSingletonPtr()->LogError
#define LOG_CRITICAL CLog::GetSingletonPtr()->LogCritical
			
#endif // _LOG_DEF_H_
