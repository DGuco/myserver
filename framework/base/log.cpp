#include "log.h"

// 初始一种类型的日志
int CLog::InitBaseLog(const char* vLogName, const char* vLogDir, level_enum level, bool vAppend)
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

int CLog::Init_Roating_Log(const char* vLogName, const char* vLogDir, level_enum level, int vMaxFileSize, int vMaxBackupIndex)          /*回卷文件个数*/
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

int CLog::Init_Daily_Log(const char* vLogName, const char* vLogDir, level_enum level, int hour, int minute)
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