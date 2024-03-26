#ifndef _STATISTICS_H_
#define _STATISTICS_H_

class CProcessMsgDelay
{
public:
	enum ETimeGrade
	{
		ETimeGrade_Lv1	= 0,	// –°”⁄1√Î÷”
		ETimeGrade_Lv2,			// 1√Î ~ 2√Î
		ETimeGrade_Lv3,			// 2√Î ~ 3√Î
		ETimeGrade_Lv4,			// 3√Î ~ 5√Î
		ETimeGrade_Lv5,			// 5√Î ~ 10√Î
		ETimeGrade_Lv6,			// 10√Î ~ 30√Î
		ETimeGrade_Lv7,			// 30√Î ~ 60√Î
		ETimeGrade_Lv8,			// ¥Û”⁄60√Î

		ETimeGrade_Num			// ◊‹µµ ˝
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


