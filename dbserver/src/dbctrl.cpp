#include "dbctrl.h"
#include "common_def.h"

CDBCtrl::CDBCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
	m_pDbScheduler = new CThreadScheduler("DBLogicScheduler");
}

int CDBCtrl::PrepareToRun()
{
	if (!m_pDbScheduler->Init(DB_THREAD_MAX))
	{
		return 0;
	}
	return 1;
}