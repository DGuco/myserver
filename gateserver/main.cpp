//
//  main.cpp
//  tcpserver 
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include "inc/gate_ctrl.h"

using namespace std;

CGateCtrl *g_pGateServer;

int main(int argc, char **argv)
{
	int iTmpRet;
	g_pGateServer = new CGateCtrl;
	iTmpRet = g_pGateServer->PrepareToRun();
	if (!iTmpRet) {
		delete g_pGateServer;
		LOG_ERROR("default", "Tcpserver PrepareToRun failed,iRet = %d", iTmpRet);
		exit(0);
	}
	g_pGateServer->Run();
}
