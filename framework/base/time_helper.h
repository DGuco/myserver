/*****************************************************************
* FileName:time_helper.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__
#include <chrono>
#include <ctime>
#include <ratio>
#include "base.h"
#include "singleton.h"

class CTimeHelper : public CSingleton<CTimeHelper>
{
public:
	CTimeHelper();
	~CTimeHelper();

	//��������ʱ��1970��01��01��00ʱ00��00��(����ʱ��1970��01��01��08ʱ00��00��)�������ڵ�������
	time_t			GetANSITime();
	//����
	uint64			GetMSTime();
	//���»���ʱ��
	void			SetTime();
	//���»���ʱ��
	void			SetTime(std::chrono::time_point<std::chrono::system_clock> time);
	//ȡ������ʱ��ʱ��ġ��ꡢ�¡��ա�Сʱ���֡��롢���ڵ�ֵ��
	int				GetYear() { return m_TM.tm_year + 1900; };	//[1900,????]
	int				GetMonth() { return m_TM.tm_mon; };		//[0,11]
	int				GetDay() { return m_TM.tm_mday; };		//[1,31]
	int				GetHour() { return m_TM.tm_hour; };		//[0,23]
	int				GetMinute() { return m_TM.tm_min; };		//[0,59]
	int				GetSecond() { return m_TM.tm_sec; };		//[0,59]
	//ȡ�õ�ǰ�����ڼ���0��ʾ�������죬1��6��ʾ������һ��������
	int				GetWeek() { return m_TM.tm_wday; };
private:
	std::chrono::time_point<std::chrono::system_clock> m_CacheTime;
	tm m_TM;
};


class CMyTimer
{
protected:
	time_t mNextTimeout;    // ��һ�γ�ʱʱ�䣨���룩
	time_t mDuration;        // ���γ�ʱ��������룩
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
	// ���뵱ǰʱ�䣨���룩�����Ƿ�ʱ�������ʱ��������һ�γ�ʱʱ�䣬������
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

	// ����timer��ʱʱ��
	void ResetTimeout(time_t tNow)
	{
		mNextTimeout = (tNow + mDuration);
	}

	// ����timer��ʱʱ��
	bool IsBeginTimer()
	{
		return mNextTimeout > 0;
	}
};

#endif //__TIME_HELPER_H__

