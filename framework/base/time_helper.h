/*****************************************************************
* FileName:time_helper.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__
#include "base.h"
#include "server_tool.h"

class CTimeHelper : public CSingleton<CTimeHelper>
{
public:
	CTimeHelper();
	~CTimeHelper();

	//格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数
	time_t			GetANSITime();
	//毫秒
	uint64			GetMSTime();
	//更新缓存时间
	void			SetTime();
	//更新缓存时间
	void			SetTime(std::chrono::time_point<std::chrono::system_clock> time);
	//取得设置时间时候的“年、月、日、小时、分、秒、星期的值”
	int				GetYear() { return m_TM.tm_year + 1900; };	//[1900,????]
	int				GetMonth() { return m_TM.tm_mon; };		//[0,11]
	int				GetDay() { return m_TM.tm_mday; };		//[1,31]
	int				GetHour() { return m_TM.tm_hour; };		//[0,23]
	int				GetMinute() { return m_TM.tm_min; };		//[0,59]
	int				GetSecond() { return m_TM.tm_sec; };		//[0,59]
	//取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
	int				GetWeek() { return m_TM.tm_wday; };
private:
	std::chrono::time_point<std::chrono::system_clock> m_CacheTime;
	tm m_TM;
};

#endif //__TIME_HELPER_H__

