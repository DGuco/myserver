//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <clocale>
#include <signal.h>
#include <runflag.h>
#include <config.h>
#include "inc/proxy_ctrl.h"

CRunFlag g_byRunFlag;

void sigusr1_handle(int iSigVal)
{
	g_byRunFlag.SetRunFlag(ERF_RELOAD);
	signal(SIGUSR1, sigusr1_handle);
}

int main(int argc, char **argv)
{
	std::shared_ptr<CProxyCtrl> &pProxyCtrl = CProxyCtrl::GetSingletonPtr( );
	if (pProxyCtrl->PrepareToRun( )) {
		LOG_ERROR("default", "ProxyServer prepare to fun failed.");
		exit(3);
	}

	// 安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	pProxyCtrl->Run( );
	// 关闭日志
	LOG_SHUTDOWN_ALL;
	return 0;
}
