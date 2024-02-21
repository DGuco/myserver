#include "time_helper.h"

CTimeHelper* g_pCTimeHelper = NULL;

CTimeHelper::CTimeHelper()
{
	m_CurrentTime = 0;
}

CTimeHelper::~CTimeHelper()
{
}

bool CTimeHelper::Init()
{

#if defined(__WINDOWS__)
	m_StartTime = GetTickCount();
	m_CurrentTime = GetTickCount();
#elif defined(__LINUX__)
		m_StartTime = 0;
	m_CurrentTime = 0;
	gettimeofday(&_tstart, &tz);
#endif
	SetTime();
	return true;
}

unsigned int CTimeHelper::CurrentTime()
{

#if defined(__WINDOWS__)
		m_CurrentTime = GetTickCount();
#elif defined(__LINUX__)
		gettimeofday(&_tend, &tz);
	double t1, t2;
	t1 = (double)_tstart.tv_sec * 1000 + (double)_tstart.tv_usec / 1000;
	t2 = (double)_tend.tv_sec * 1000 + (double)_tend.tv_usec / 1000;
	m_CurrentTime = (unsigned int)(t2 - t1);
#endif

	return m_CurrentTime;
}

unsigned int	CTimeHelper::CurrentDate()
{
	SetTime();
	unsigned int Date;
	ConvertTU(&m_TM, Date);
	return Date;
}


void CTimeHelper::SetTime()
{
	time(&m_SetTime);
	tm* ptm = localtime(&m_SetTime);
	m_TM = *ptm;
}

// 得到标准时间
time_t CTimeHelper::GetANSITime()
{
	SetTime();
	return m_SetTime;
}

unsigned int CTimeHelper::Time2Dint()
{
	SetTime();
	unsigned int uRet = 0;

	uRet += GetYear();
	uRet -= 2000;
	uRet = uRet * 100;

	uRet += GetMonth() + 1;
	uRet = uRet * 100;

	uRet += GetDay();
	uRet = uRet * 100;

	uRet += GetHour();
	uRet = uRet * 100;

	uRet += GetMinute();
	return uRet;
}

unsigned int CTimeHelper::DiffTime(unsigned int Date1, unsigned int Date2)
{
	tm S_D1, S_D2;
	ConvertUT(Date1, &S_D1);
	ConvertUT(Date2, &S_D2);
	time_t t1, t2;
	t1 = mktime(&S_D1);
	t2 = mktime(&S_D2);
	unsigned int dif = (unsigned int)(difftime(t2, t1) * 1000);
	return dif;
}

void CTimeHelper::ConvertUT(unsigned int Date, tm* TM)
{
	memset(TM, 0, sizeof(tm));
	TM->tm_year = (Date >> 26) & 0xf;
	TM->tm_mon = (Date >> 22) & 0xf;
	TM->tm_mday = (Date >> 17) & 0x1f;
	TM->tm_hour = (Date >> 12) & 0x1f;
	TM->tm_min = (Date >> 6) & 0x3f;
	TM->tm_sec = (Date) & 0x3f;
}

void CTimeHelper::ConvertTU(tm* TM, unsigned int& Date)
{
	Date = 0;
	Date += (TM->tm_yday % 10) & 0xf;
	Date = (Date << 4);
	Date += TM->tm_mon & 0xf;
	Date = (Date << 4);
	Date += TM->tm_mday & 0x1f;
	Date = (Date << 5);
	Date += TM->tm_hour & 0x1f;
	Date = (Date << 5);
	Date += TM->tm_min & 0x3f;
	Date = (Date << 6);
	Date += TM->tm_sec & 0x3f;
}

unsigned int CTimeHelper::GetDayTime()
{
	time_t st;
	time(&st);
	tm* ptm = localtime(&m_SetTime);

	unsigned int uRet = 0;
	uRet = (ptm->tm_year - 100) * 1000;
	uRet += ptm->tm_yday;

	return uRet;
}

int CTimeHelper::GetTodayTime()
{
	
	time_t st;
	time(&st);
	tm* ptm = localtime(&m_SetTime);

	int uRet = 0;

	uRet = ptm->tm_hour * 100;
	uRet += ptm->tm_min;

	return uRet;
}

bool CTimeHelper::FormatTodayTime(int& nTime)
{
	
	bool ret = false;

	int wHour = nTime / 100;
	int wMin = nTime % 100;
	int wAddHour = 0;
	if (wMin > 59)
	{
		wAddHour = wMin / 60;
		wMin = wMin % 60;
	}
	wHour += wAddHour;
	if (wHour > 23)
	{
		ret = true;
		wHour = wHour % 60;
	}
	return ret;
}
