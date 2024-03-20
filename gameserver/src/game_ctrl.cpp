#include "base.h"
#include "game_ctrl.h"
#include "game_server.h"
#include "log.h"

CGameCtrl::CGameCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
#endif
}

CGameCtrl::~CGameCtrl()
{
#ifdef __WINDOWS__
	WSACleanup();
#endif
}

bool CGameCtrl::PrepareToRun()
{
	if (!INIT_LOG("gameserver"))
	{
		return false;
	}
	int a = 0;
	//¶ÁÈ¡ÅäÖÃÎÄ¼ş
	if (!ReadConfig())
	{
		return false;
	}

	if (!CGameServer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}
	return true;
}

int CGameCtrl::Run()
{

}