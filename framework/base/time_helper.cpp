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
time_t CTimeHelper::GetANSITime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	SetTime(tmTime);
	return std::chrono::system_clock::to_time_t(tmTime);
}

//毫秒
uint64 CTimeHelper::GetMSTime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	SetTime(tmTime);
	return duration_cast<milliseconds>(tmTime.time_since_epoch()).count();
}

void CTimeHelper::SetTime()
{
	m_CacheTime = std::chrono::system_clock::now();
	time_t nowTime = std::chrono::system_clock::to_time_t(m_CacheTime);
	tm* ptm = localtime(&nowTime);//获得std::tm的结构
	m_TM = *ptm;
}


void CTimeHelper::SetTime(std::chrono::time_point<std::chrono::system_clock> time)
{
	m_CacheTime = time;
	time_t nowTime = std::chrono::system_clock::to_time_t(m_CacheTime);
	tm* ptm = localtime(&nowTime);//获得std::tm的结构
	m_TM = *ptm;
}

