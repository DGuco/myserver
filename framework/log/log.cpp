#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "log.h"

// 初始一种类型的日志
int INIT_BASE_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				const char *vLogDir,                        /*文件名称(路径)*/
				level_enum level,        /*日志等级*/
				bool vAppend)
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

int INIT_ROATING_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				   const char *vLogDir,                        /*文件名称(路径)*/
				   level_enum level,        /*日志等级*/
				   unsigned int vMaxFileSize,    /*回卷文件最大长度*/
				   unsigned int vMaxBackupIndex)          /*回卷文件个数*/
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

int INIT_DAILY_LOG(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,        /*日志等级*/
				 unsigned int hour,
				 unsigned int minute)
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

int LOG_SHUTDOWN_ALL()
{
	spdlog::drop_all();
	return 0;
}