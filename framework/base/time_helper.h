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

#endif //__TIME_HELPER_H__

