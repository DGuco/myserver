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

//当月有多少天
int CTimeHelper::GetMonthDay(int year,int month)
{
	if (month < 1 || month > 12)
	{
		return 0;
	}
	if (month == 2)
	{
		return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) ? 29 : 28;
	}
	if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		return 30;
	}
	return 31;
}

//20260306把日期转换为固定格式YYYYMMDD
unsigned int CTimeHelper::Time2Day()
{
	unsigned int year = GetYear();
	unsigned int month = GetMonth() + 1;
	unsigned int day = GetDay();
	return year * 10000 + month * 100 + day;
}

//20260306根据日期和天数差计算新的日期
unsigned int CTimeHelper::Time2DayAfter(unsigned int time2day,int diffDay)
{
    // 假设 day 是一个格式为 YYYYMMDD 的整数
    int year = time2day / 10000;
    int month = (time2day % 10000) / 100;
    int dayOfMonth = time2day % 100;

    // 处理天数差异
    dayOfMonth += diffDay;

    // 处理月份进位和退位
    while (true)
    {
        // 获取当前月份的天数
        int daysInMonth = GetMonthDay(year, month);

        if (dayOfMonth > 0 && dayOfMonth <= daysInMonth)
        {
            // 天数在有效范围内，退出循环
            break;
        }

        if (dayOfMonth > daysInMonth)
        {
            // 天数超过当前月份，处理进位
            dayOfMonth -= daysInMonth;
            month++;
            
            if (month > 12)
            {
                // 月份超过12，年份加1，月份重置为1
                year++;
                month = 1;
            }
        }
        else
        {
            // 天数为负数或0，处理退位
            month--;
            
            if (month < 1)
            {
                // 月份小于1，年份减1，月份重置为12
                year--;
                month = 12;
            }
            
            // 获取上一个月的天数
            daysInMonth = GetMonthDay(year, month);
            dayOfMonth += daysInMonth;
        }
    }

    // 重新组合成 YYYYMMDD 格式并返回
    return year * 10000 + month * 100 + dayOfMonth;
}
