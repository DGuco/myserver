//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include "gate_ctrl.h"

using namespace std;
int main(int argc, char **argv)
{
	int iTmpRet = CGateCtrl::GetSingletonPtr()->PrepareToRun();
	if (iTmpRet != 0) {
		LOG_ERROR("default", "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
		exit(0);
	}
	CGateCtrl::GetSingletonPtr()->Run();
	// 关闭日志
	LOG_SHUTDOWN_ALL;
}

