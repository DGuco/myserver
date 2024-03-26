//
//  main.cpp
//  gameserver
//  Created by DGuco on 17-3-1.
//  Copyright ? 2017年 DGuco. All rights reserved.
//

#include <signal.h>
#include "signal_handler.h"
#include "inc/game_server.h"
#include "game_ctrl.h"

int main(int argc, char *argv[])
{
	//信号处理注册
	CSignalHandler::GetSingletonPtr()->RegisterHandler("gameserver");
	
	if (!INIT_LOG("gameserver"))
	{
		exit(0);
	}

	try
	{

		int iTmpRet = CGameCtrl::GetSingletonPtr()->PrepareToRun();
		if (!iTmpRet)
		{
			DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
			exit(0);
		}
	}
	catch (const std::exception& e)
	{
		DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,get exception = {}", e.what());
		exit(0);
	}

	CGameCtrl::GetSingletonPtr()->Run();
	SHUTDOWN_ALL_LOG();

	return 0;
}

