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
	try
	{
		int iTmpRet = CGateCtrl::GetSingletonPtr()->PrepareToRun();
		if (!iTmpRet)
		{
			LOG_ERROR("default", "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
			exit(0);
		}
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("default", "CGateCtrl PrepareToRun failed,get exception = {}", e.what());
		exit(0);
	}

	CGateCtrl::GetSingletonPtr()->Run();
	// 关闭日志
	SHUTDOWN_ALL_LOG();
}

