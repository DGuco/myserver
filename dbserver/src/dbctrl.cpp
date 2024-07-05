#include "dbctrl.h"

CDBCtrl::CDBCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
}

int CDBCtrl::PrepareToRun()
{
	return 0;
}