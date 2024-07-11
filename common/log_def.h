/*****************************************************************
* FileName:log_def.h
* Summary :
* Date	  :2024-7-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __LOG_DEF_H__
#define __LOG_DEF_H__

#include <string>
#include <stdarg.h>
#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/hour_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;
using namespace spdlog;
using namespace spdlog::level;

struct stLogInfo
{
	int	  log_type;
	char* logName;
	level_enum level;
};

enum enDiskLog
{
	ASSERT_DISK = 0,
	DEBUG_DISK = 1,
	ERROR_DISK = 2,
	DB_ERROR = 3,
	SHM_DEBUG = 4,
	SHM_ERROR = 5,
	DIS_LOG_MAX,
};

enum enCacheLog
{
	DEBUG_CACHE = 100,
	ERROR_CACHE = 101,
	TRACE_CACHE = 102,
	LUA_CACHE = 103,
	LUA_ERROR = 104,
	PERF_CACHE = 105,
	TCP_DEBUG = 106,
	TCP_ERROR = 107,
	DB_CACHE = 108,
	THREAD_CACHE = 109,
	CACHE_LOG_MAX,
};

static stLogInfo g_DisLogFile[] =
{
	{enDiskLog::ASSERT_DISK,"assert",level_enum::critical},
	{enDiskLog::DEBUG_DISK,"debug_disk",level_enum::debug},
	{enDiskLog::ERROR_DISK,"error_disk",level_enum::err},
	{enDiskLog::DB_ERROR,"db_error",level_enum::err},
	{enDiskLog::SHM_DEBUG,"shm_debug",level_enum::debug},
	{enDiskLog::SHM_ERROR,"shm_error",level_enum::err},
};

static stLogInfo g_CacheLogFile[] =
{
	{enCacheLog::DEBUG_CACHE,"debug",level_enum::debug},
	{enCacheLog::ERROR_CACHE,"error",level_enum::err},
	{enCacheLog::TRACE_CACHE,"trace",level_enum::trace},
	{enCacheLog::LUA_CACHE,"lua",level_enum::debug},
	{enCacheLog::LUA_ERROR,"lua_error",level_enum::err},
	{enCacheLog::PERF_CACHE,"perf",level_enum::debug},
	{enCacheLog::TCP_DEBUG,"tcp",level_enum::debug},
	{enCacheLog::TCP_ERROR,"tcp_error",level_enum::err},
	{enCacheLog::DB_CACHE,"db",level_enum::debug},
	{enCacheLog::THREAD_CACHE,"thread_info",level_enum::debug},
};
#endif //__LOG_DEF_H__
