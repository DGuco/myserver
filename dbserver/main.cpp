//
// Created by DGuco on 17-7-12.
//

#include "base.h"
#include "log.h"
#include "dbctrl.h"
#include "signal_handler.h"

int main(int argc, char **argv)
{
	//信号处理注册
	CSignalHandler::GetSingletonPtr()->RegisterHandler("dbserver");

	if (!INIT_LOG("dbserver"))
	{
		exit(0);
	}

	if (CDBCtrl::GetSingletonPtr()->PrepareToRun() < 0)  // 创建处理线程
	{
		exit(-1);
	}

	CDBCtrl::GetSingletonPtr()->Run();

	SHUTDOWN_ALL_LOG();
	return 0;
}

