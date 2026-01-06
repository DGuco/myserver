/*****************************************************************
* FileName:time_helper.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__
#include <chrono>
#include <ctime>
#include <ratio>
#include <map>
#include "base.h"
#include "singleton.h"

typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

class CTimeHelper : public CSingleton<CTimeHelper>
{
public:
	CTimeHelper();
	~CTimeHelper();

	//格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数
	time_t			GetANSITime(bool realTime = false);
	//毫秒
	uint64			GetMSTime(bool realTime = false);
	//微妙
	time_t			GetMicroTime(bool realTime = false);
	//更新缓存时间
	void			SetTime();
	//取得设置时间时候的“年、月、日、小时、分、秒、星期的值”
	int				GetYear();	//[1900,????]
	int				GetMonth();		//[0,11]
	int				GetDay();		//[1,31]
	int				GetHour();		//[0,23]
	int				GetMinute();		//[0,59]
	int				GetSecond();		//[0,59]
	//取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
	int				GetWeek();
	static std::tm	LocalTime( std::time_t& time_tt);
	//获取天数差
	int 			DiffDay(time_t tm1, time_t tm2);
	//
	int				DiffWeek(time_t tm1, time_t tm2);
	//
	int				GetDayOfYear();
};


class CMyTimer
{
protected:
	time_t mNextTimeout;    // 下一次超时时间（毫秒）
	time_t mDuration;        // 单次超时间隔（毫秒）
public:
	CMyTimer()
	{
		mDuration = 0;
		mNextTimeout = 0;
	}

	void BeginTimer(time_t uNow,time_t vDuration)
	{
		mNextTimeout = uNow + mDuration;
		mDuration = vDuration;
	}

public:
	// 传入当前时间（毫秒）返回是否超时，如果超时将计算下一次超时时间，并补偿
	inline bool IsTimeout(time_t tNow)
	{
		if (mDuration <= 0) 
		{
			return false;
		}

		if (tNow >= mNextTimeout) 
		{
			if (mNextTimeout == 0)
			{
				mNextTimeout = (tNow + mDuration);
			}
			else 
			{
				mNextTimeout += mDuration;
			}
			return true;
		}

		return false;
	}

	// 重置timer超时时间
	void ResetTimeout(time_t tNow)
	{
		mNextTimeout = (tNow + mDuration);
	}

	// 重置timer超时时间
	bool IsBeginTimer()
	{
		return mNextTimeout > 0;
	}

	void StopTimer()
	{
		mNextTimeout = 0;
	}
};

#endif //__TIME_HELPER_H__

