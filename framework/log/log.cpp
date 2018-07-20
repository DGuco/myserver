#include <spdlog/spdlog.h>
#include "log.h"

// 初始一种类型的日志
int InitBaseLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				const char *vLogDir,                        /*文件名称(路径)*/
				level_enum level,        /*日志等级*/
				bool vAppend)
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
	auto log = spdlog::basic_logger_mt(vLogName, vLogDir, vAppend);
#ifdef _DEBUG_
	auto console = spdlog::stdout_color_mt(CONSOLE_LOG_NAME);
#endif
#else
	auto log = spdlog::basic_logger_st(vLogName, vLogDir,vAppend);
#ifdef _DEBUG_
		auto console = spdlog::stdout_color_st(CONSOLE_LOG_NAME);
#else
#endif
#endif

#ifdef _DEBUG_
	console->set_level(level);
	console->flush_on(level);
#endif
	if (log == nullptr) {
		return -1;
	}
	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int InitRoatingLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				   const char *vLogDir,                        /*文件名称(路径)*/
				   level_enum level,        /*日志等级*/
				   unsigned int vMaxFileSize,    /*回卷文件最大长度*/
				   unsigned int vMaxBackupIndex)           /*回卷文件个数*/
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
	auto log = spdlog::rotating_logger_mt(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
#ifdef _DEBUG_
	auto console = spdlog::stdout_color_mt(CONSOLE_LOG_NAME);
#endif
#else
	auto log = spdlog::rotating_logger_st(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
#ifdef _DEBUG_
		auto console = spdlog::stdout_color_st(CONSOLE_LOG_NAME);
#endif
#endif

#ifdef _DEBUG_
	console->set_level(level);
	console->flush_on(level);
#endif
	if (log == NULL) {
		return -1;
	}
	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int InitDailyLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,        /*日志等级*/
				 unsigned int hour,
				 unsigned int minute)
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
	auto log = spdlog::daily_logger_mt(vLogName, vLogDir, hour, minute);
#ifdef _DEBUG_
	auto console = spdlog::stdout_logger_st(CONSOLE_LOG_NAME);
#endif
#else
	auto log = spdlog::daily_logger_st(vLogName, vLogDir, hour, minute);
#endif

#ifdef _DEBUG_
	console->set_level(level);
	console->flush_on(level);
#endif
	if (log == NULL) {
		return -1;
	}

	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int ShutdownAllLog()
{
	spdlog::drop_all();
	return 0;
}