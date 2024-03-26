#ifndef _STATISTICS_H_
#define _STATISTICS_H_

class CProcessMsgDelay
{
public:
	enum ETimeGrade
	{
		ETimeGrade_Lv1	= 0,	// С��1����
		ETimeGrade_Lv2,			// 1�� ~ 2��
		ETimeGrade_Lv3,			// 2�� ~ 3��
		ETimeGrade_Lv4,			// 3�� ~ 5��
		ETimeGrade_Lv5,			// 5�� ~ 10��
		ETimeGrade_Lv6,			// 10�� ~ 30��
		ETimeGrade_Lv7,			// 30�� ~ 60��
		ETimeGrade_Lv8,			// ����60��

		ETimeGrade_Num			// �ܵ���
	};

public:
	CProcessMsgDelay();
	~CProcessMsgDelay();

private:
	unsigned long m_nTotalMsgNum;
	unsigned long m_aryStatistics[ETimeGrade_Num];

public:
	void CountMsg(long tmDelay);
	void PrintStatistics(char* pBuffer, unsigned int& unLen);
};

#endif // _STATISTICS_H_


