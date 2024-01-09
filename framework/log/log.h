//
//  log.h
//  Created by DGuco on 18/02/28.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <stdarg.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace spdlog;
using namespace spdlog::level;

#define  CONSOLE_LOG_NAME "console"

int INIT_BASE_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,						/*日志等级*/
				 bool vAppend = true);					 /*是否截断(默认即可)*/


int INIT_ROATING_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				    const char *vLogDir,                        /*文件名称(路径)*/
				   level_enum level,        /*日志等级*/
				   unsigned int vMaxFileSize = 10 * 1024 * 1024,    /*回卷文件最大长度*/
				   unsigned int vMaxBackupIndex = 5);            /*回卷文件个数*/

int INIT_DAILY_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,        /*日志等级*/
				 unsigned int hour,
				 unsigned int minute);

int LOG_SHUTDOWN_ALL();

template<typename... Args>
int LOG_NOTICE(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LOG_DEBUG(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LOG_INFO(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LOG_WARN(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LOG_ERROR(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LOG_FATAL(const char *vLogName, const char *vFmt, const Args &... args);

template<typename... Args>
int LogTrace(const char *vLogName, const char *vFmt, const Args &... args)
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
int LogDebug(const char *vLogName, const char *vFmt, const Args &... args)
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
int LogInfo(const char *vLogName, const char *vFmt, const Args &... args)
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
int LogWarn(const char *vLogName, const char *vFmt, const Args &... args)
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
int LogError(const char *vLogName, const char *vFmt, const Args &... args)
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
int LogCritical(const char *vLogName, const char *vFmt, const Args &... args)
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

#endif // _LOG_H_
