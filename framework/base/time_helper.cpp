#include "time_helper.h"
#include "my_thread.h"

using namespace std;
using namespace std::chrono;

CTimeHelper::CTimeHelper()
{
}

CTimeHelper::~CTimeHelper()
{
}

// 得到标准时间
time_t CTimeHelper::GetANSITime(bool realTime)
{
	if (realTime)
	{
		SetTime();
	}
	return std::chrono::system_clock::to_time_t(g_thread_data.m_CacheTimePoint);
}

//毫秒
uint64 CTimeHelper::GetMSTime(bool realTime)
{
	if (realTime)
	{
		SetTime();
	}
	return duration_cast<milliseconds>(g_thread_data.m_CacheTimePoint.time_since_epoch()).count();
}

time_t CTimeHelper::GetMicroTime(bool realTime)
{
	if (realTime)
	{
		SetTime();
	}
	return duration_cast<microseconds>(g_thread_data.m_CacheTimePoint.time_since_epoch()).count();
}

void CTimeHelper::SetTime()
{
	g_thread_data.m_CacheTimePoint = std::chrono::system_clock::now();
	time_t nowTime = std::chrono::system_clock::to_time_t(g_thread_data.m_CacheTimePoint);
	std::tm ptm = LocalTime(nowTime);//获得std::tm的结构
	g_thread_data.m_CacheTime = ptm;
}

std::tm CTimeHelper::LocalTime( std::time_t& time_tt)
{
	//localtime非线程安全
	//tm* logTime = localtime(&logtime);
#if defined(__WINDOWS__) || defined(_WIN32)
	std::tm tm;
	localtime_s(&tm, &time_tt);
#else
	std::tm tm;
	localtime_r(&time_tt, &tm);
#endif
	return tm;
}

int CTimeHelper::DiffDay(time_t tm1, time_t tm2)
{
    double secondsDiff = difftime(tm2, tm1);
    int daysDiff = static_cast<int>(secondsDiff / 86400);
    return daysDiff;
}

//获取周数差
int CTimeHelper::DiffWeek(time_t tm1, time_t tm2)
{
    int daysDiff = DiffDay(tm1, tm2);
    int weeksDiff = daysDiff / 7;
    return weeksDiff;
}

int CTimeHelper::GetDayOfYear()
{
	return g_thread_data.m_CacheTime.tm_yday;
}

int CTimeHelper::GetYear()
{
	return g_thread_data.m_CacheTime.tm_year + 1900;
}	//[1900,????]

int CTimeHelper::GetMonth()
{
	return g_thread_data.m_CacheTime.tm_mon;
}		//[0,11]

int CTimeHelper::GetDay()
{
	return g_thread_data.m_CacheTime.tm_mday;
}		//[1,31]

int CTimeHelper::GetHour()
{
	return g_thread_data.m_CacheTime.tm_hour;
}		//[0,23]

int CTimeHelper::GetMinute()
{
	return g_thread_data.m_CacheTime.tm_min;
}		//[0,59]

int CTimeHelper::GetSecond()
{
	return g_thread_data.m_CacheTime.tm_sec;
}		//[0,59]

//取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
int  CTimeHelper::GetWeek()
{
	return g_thread_data.m_CacheTime.tm_wday;
}
