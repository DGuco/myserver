#include "time_helper.h"

using namespace std;
using namespace std::chrono;

CTimeHelper::CTimeHelper()
{
}

CTimeHelper::~CTimeHelper()
{
}

// �õ���׼ʱ��
time_t CTimeHelper::GetANSITime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	SetTime(tmTime);
	return std::chrono::system_clock::to_time_t(tmTime);
}

//����
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
	tm* ptm = localtime(&nowTime);//���std::tm�Ľṹ
	m_TM = *ptm;
}


void CTimeHelper::SetTime(std::chrono::time_point<std::chrono::system_clock> time)
{
	m_CacheTime = time;
	time_t nowTime = std::chrono::system_clock::to_time_t(m_CacheTime);
	tm* ptm = localtime(&nowTime);//���std::tm�Ľṹ
	m_TM = *ptm;
}

