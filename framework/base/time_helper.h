/*****************************************************************
* FileName:time_helper.h
* Summary :
* Date	  :2024-2-21
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__
#include "base.h"

class CTimeHelper : public CSingleton<CTimeHelper>
{
public:
	CTimeHelper();
	~CTimeHelper();

	bool			Init();

	//��ǰʱ�����ֵ����ʼֵ����ϵͳ��ͬ������
	//���ص�ֵΪ��΢�λ��ʱ��ֵ
	time_t			CurrentTime();
	//ֱ�ӷ��أ�������ϵͳ�ӿ�
	time_t			CurrentSavedTime() { return m_CurrentTime; };
	//ȡ�÷������˳�������ʱ��ʱ�����ֵ
	time_t			StartTime() { return m_StartTime; };

	//����ǰ��ϵͳʱ���ʽ����ʱ���������
	void			SetTime();

	// �õ���׼ʱ��
	time_t			GetANSITime();

public:
	//***ע�⣺
	//���½ӿڵ���û��ϵͳ���ã�ֻ���ʱ��������ڵ�����
	//

	//ȡ������ʱ��ʱ��ġ��ꡢ�¡��ա�Сʱ���֡��롢���ڵ�ֵ��
	int				GetYear() { return m_TM.tm_year + 1900; };	//[1900,????]
	int				GetMonth() { return m_TM.tm_mon; };		//[0,11]
	int				GetDay() { return m_TM.tm_mday; };		//[1,31]
	int				GetHour() { return m_TM.tm_hour; };		//[0,23]
	int				GetMinute() { return m_TM.tm_min; };		//[0,59]
	int				GetSecond() { return m_TM.tm_sec; };		//[0,59]
	//ȡ�õ�ǰ�����ڼ���0��ʾ�������죬1��6��ʾ������һ��������
	int			GetWeek() { return m_TM.tm_wday; };
	//����ǰ��ʱ�䣨�ꡢ�¡��ա�Сʱ���֣�ת����һ��UINT����ʾ
	//���磺0,507,211,233 ��ʾ "2005.07.21 12:33"
	int			Time2DWORD();
	//ȡ�õ�ǰ������[4bit 0-9][4bit 0-11][5bit 0-30][5bit 0-23][6bit 0-59][6bit 0-59]
	int			CurrentDate();
	//ȡ�÷����������������ʱ�䣨���룩
	int			RunTime() {
		CurrentTime();
		return (m_CurrentTime - m_StartTime);
	};
	void			RunTick()
	{
		CurrentTime();
		return WORD(m_CurrentTime - m_StartTime);
	};
	//ȡ����������ʱ���ʱ����λ�����룩, Ret = Date2-Data1
	uint32			DiffTime(UINT Date1, UINT Date2);
	//��һ��UINT������ת����һ��tm�ṹ
	void			ConvertUT(UINT Date, tm* TM);
	//��һ��tm�ṹת����һ��UINT������
	void			ConvertTU(tm* TM, UINT& Date);
	//ȡ������Ϊ��λ��ʱ��ֵ, ǧλ��������ݣ�������λ����ʱ�䣨������
	uint32			GetDayTime();
	//�õ���ǰ�ǽ����ʲôʱ��2310��ʾ23��10��
	void			GetTodayTime();
	bool			FormatTodayTime(WORD& nTime);


public:
	time_t			m_StartTime;
	time_t			m_CurrentTime;
	time_t			m_SetTime;
	tm				m_TM;
#ifdef __LINUX__
	struct timeval _tstart, _tend;
	struct timezone tz;
#endif
};

#endif //__TIME_HELPER_H__

