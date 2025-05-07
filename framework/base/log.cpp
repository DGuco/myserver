#include "log.h"

CLog::CLog()
{
}

CLog::~CLog()
{
	// 关闭日志
	ShutDownAll();
}

// 在 CLog 类中添加工具函数
bool CLog::CreateDirectoryRecursive(const std::string& path) 
{
	#ifdef __LINUX__
		// Linux/macOS 实现
		mode_t mode = 0755;
		struct stat st;
		if (stat(path.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode);
		}
		return mkdir(path.c_str(), mode) == 0 || errno == EEXIST;
	#else
		// Windows 实现
		std::wstring wpath(path.begin(), path.end());
		return CreateDirectoryW(wpath.c_str(), NULL) || 
				GetLastError() == ERROR_ALREADY_EXISTS;
	#endif
}

bool CLog::Init(const char* modulename)
{
	for (int index = ASSERT_DISK; index < DIS_LOG_MAX; index++)
	{
		stLogInfo logInfo = g_DisLogFile[index];
		std::string filename = "../log/"+ std::string(modulename) + "/";
		CreateDirectoryRecursive(filename);
		filename = filename + std::string(logInfo.logName) + std::string(".") + std::string(modulename) + ".log";
		InitHourLog(logInfo.logName, filename.c_str(), logInfo.level);
	}

	spdlog::init_thread_pool(MAX_SPDLOG_QUEUE_SIZE, MAX_SPDLOG_THREAD_POOL);
	for (int index = DEBUG_CACHE; index < CACHE_LOG_MAX; index++)
	{
		stLogInfo logInfo = g_CacheLogFile[index - DEBUG_CACHE];
		std::string filename = "../log/" + std::string(modulename) + "/";
		CreateDirectoryRecursive(filename);
		filename = filename + std::string(logInfo.logName) + std::string(".") + std::string(modulename) + ".log";
		InitHourLog(logInfo.logName, filename.c_str(), logInfo.level,true);
	}

#if defined(__WINDOWS__) || defined(_WIN32)
	//这里在控制台左键点击出现光标后，输出日志会卡当前输出日志的线程，ctrl+c恢复正常，或者右键执行程序
	//选择属性 取消选中"快速编辑模式"生效一次,或者右键执行窗口右键默认值中取消选中"快速编辑模式"永久有效
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
		if (logger == NULL)
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