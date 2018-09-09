//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include "inc/gate_ctrl.h"

using namespace std;
int main(int argc, char **argv)
{
	std::shared_ptr<CGateCtrl> &pGateCtrl = CGateCtrl::CreateInstance();
	int iTmpRet = pGateCtrl->PrepareToRun();
	if (iTmpRet != 0) {
		LOG_ERROR("default", "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
		exit(0);
	}
	pGateCtrl->Run();
	// 关闭日志
	LOG_SHUTDOWN_ALL;
}

