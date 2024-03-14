#include "log.h"

stLogInfo g_DisLogFile[] =
{
	{"assert",level_enum::critical},
	{"debug_disk",level_enum::debug},
	{"error_disk",level_enum::err},
	{"tcp_debug",level_enum::debug},
	{"tcp_error",level_enum::err},
	{"db_error",level_enum::err},
	{"shm_debug",level_enum::debug},
	{"shm_error",level_enum::err},
};

stLogInfo g_CacheLogFile[] =
{
	{"debug_cache",level_enum::debug},
	{"error_cache",level_enum::err},
	{"trace_cache",level_enum::trace},
	{"lua_cache",level_enum::debug},
	{"lua_error",level_enum::err},
	{"perf_cache",level_enum::debug},
};

CLog::CLog()
{
}

CLog::~CLog()
{
}

bool CLog::Init(const char* modulename)
{
	for (int index = ASSERT_DISK; index < DIS_LOG_MAX; index++)
	{
		stLogInfo logInfo = g_DisLogFile[index];
		std::string filename = "../log/";
		filename = filename + std::string(logInfo.logName) + std::string(".") + std::string(modulename) + ".log";
		InitHourLog(logInfo.logName, filename.c_str(), logInfo.level);
	}

	spdlog::init_thread_pool(MAX_SPDLOG_QUEUE_SIZE, MAX_SPDLOG_THREAD_POOL);
	for (int index = DEBUG_CACHE; index < CACHE_LOG_MAX; index++)
	{
		stLogInfo logInfo = g_CacheLogFile[index - DEBUG_CACHE];
		std::string filename = "../log/";
		filename = filename + std::string(logInfo.logName) + std::string(".") + std::string(modulename) + ".log";
		InitHourLog(logInfo.logName, filename.c_str(), logInfo.level,true);
	}

#ifdef __WINDOWS__
	auto console = spdlog::create<spdlog::sinks::stdout_color_sink_mt>(CONSOLE_LOG_NAME);
	console->set_level(level_enum::trace);
	console->flush_on(level_enum::trace);
#endif
	return true;
}

stLogInfo* CLog::GetLogInfo(int log_type)
{
	stLogInfo* logInfo = NULL;
	if (log_type >= (int)ASSERT_DISK && log_type < int(DIS_LOG_MAX))
	{
		return &g_DisLogFile[log_type];
	}
	else if (log_type >= (int)DEBUG_CACHE && log_type < int(CACHE_LOG_MAX))
	{
		return &g_CacheLogFile[log_type - DEBUG_CACHE];
	}
	return NULL;
}

/**
 * vLogName 日志类型的名称(关键字,由此定位到日志文件)
 * vLogDir 文件名称(路径)
 * level 日志等级
 */
int CLog::InitHourLog(const char* vLogName, const char* vLogDir, level_enum level, bool async)
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}

	if (async)
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create_async<spdlog::sinks::hour_file_sink_mt>(vLogName, vLogDir);
		if (logger == NULL)
		{
			return -1;
		}

		logger->set_level(level);
		logger->flush_on(level);
	}
	else
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::hour_file_sink_mt>(vLogName, vLogDir);
		if (logger == NULL)
		{
			return -1;
		}

		logger->set_level(level);
		logger->flush_on(level);
	}

	return 0;
}

// 初始一种类型的日志
int CLog::InitBaseLog(const char* vLogName, const char* vLogDir, level_enum level, bool vAppend, bool async)
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}
	if (async)
	{
		auto logger = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(vLogName, vLogDir, vAppend);
		if (logger == nullptr)
		{
			return -1;
		}
		logger->set_level(level);
		logger->flush_on(level);
	}
	else
	{
		auto logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>(vLogName, vLogDir, vAppend);
		if (logger == nullptr)
		{
			return -1;
		}
		logger->set_level(level);
		logger->flush_on(level);
	}

	return 0;
}

int CLog::Init_Roating_Log(const char* vLogName, const char* vLogDir, level_enum level, int vMaxFileSize, int vMaxBackupIndex, bool async)          /*回卷文件个数*/
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}
	if (async)
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
		if (logger == NULL)
		{
			return -1;
		}
		logger->set_level(level);
		logger->flush_on(level);
	}
	else
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::rotating_file_sink_mt>(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
		if (logger == NULL)
		{
			return -1;
		}
		logger->set_level(level);
		logger->flush_on(level);
	}

	return 0;
}

int CLog::Init_Daily_Log(const char* vLogName, const char* vLogDir, level_enum level, int hour, int minute, bool async)
{
	if (NULL == vLogName || NULL == vLogDir)
	{
		return -1;
	}

	if (async)
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create_async<spdlog::sinks::daily_file_sink_mt>(vLogName, vLogDir, hour, minute);
		if (logger == NULL)
		{
			return -1;
		}

		logger->set_level(level);
		logger->flush_on(level);
	}
	else
	{
		std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::daily_file_sink_mt>(vLogName, vLogDir, hour, minute);
		if (logger == NULL)
		{
			return -1;
		}

		logger->set_level(level);
		logger->flush_on(level);
	}

	return 0;
}

int CLog::ShutDownAll()
{
	spdlog::drop_all();
	return 0;
}