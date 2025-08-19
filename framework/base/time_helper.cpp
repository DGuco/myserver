#include "time_helper.h"

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
		std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
		SetTime(tmTime);
	}
	return std::chrono::system_clock::to_time_t(m_CacheTime);
}

//毫秒
uint64 CTimeHelper::GetMSTime(bool realTime)
{
	if (realTime)
	{
		std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
		SetTime(tmTime);
	}
	return duration_cast<milliseconds>(m_CacheTime.time_since_epoch()).count();
}

time_t CTimeHelper::GetMicroTime(bool realTime)
{
	if (realTime)
	{
		std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
		SetTime(tmTime);
	}
	return duration_cast<microseconds>(m_CacheTime.time_since_epoch()).count();
}

void CTimeHelper::SetTime()
{
	m_CacheTime = std::chrono::system_clock::now();
	time_t nowTime = std::chrono::system_clock::to_time_t(m_CacheTime);
	std::tm ptm = LocalTime(nowTime);//获得std::tm的结构
	m_TM = ptm;
}


void CTimeHelper::SetTime(std::chrono::time_point<std::chrono::system_clock> time)
{
	m_CacheTime = time;
	time_t nowTime = std::chrono::system_clock::to_time_t(m_CacheTime);
	std::tm ptm = LocalTime(nowTime);//获得std::tm的结构
	m_TM = ptm;
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

int CTimeHelper::GetDayOfYear()
{
	return m_TM.tm_yday;
}