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

void sigpipe_handle(int sig)
{
    LOG_ERROR("default", "receive sigpipe,do sigpipe_handle");
}

int main(int argc, char **argv)
{
	std::shared_ptr<CProxyCtrl> &pProxyCtrl = CProxyCtrl::GetSingletonPtr( );
	if (pProxyCtrl->PrepareToRun( )) {
		LOG_ERROR("default", "ProxyServer prepare to fun failed.");
		exit(3);
	}

    struct sigaction sa;
    sa.sa_handler = sigpipe_handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE,&sa,NULL);

	pProxyCtrl->Run( );
	// 关闭日志
	LOG_SHUTDOWN_ALL;
	return 0;
}
