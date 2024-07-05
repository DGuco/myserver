//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright ? 2016�� DGuco. All rights reserved.
//

#include "proxy_ctrl.h"
#include "signal_handler.h"

using namespace std;
int main(int argc, char **argv)
{
	//�źŴ���ע��
	CSignalHandler::GetSingletonPtr()->RegisterHandler("proxyserver");
	
	if (!INIT_LOG("proxyserver"))
	{
		exit(0);
	}

	try
	{
		int iTmpRet = CProxyCtrl::GetSingletonPtr()->PrepareToRun();
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

	CProxyCtrl::GetSingletonPtr()->Run();
	// �ر���־
	SHUTDOWN_ALL_LOG();
	return 0;
}

