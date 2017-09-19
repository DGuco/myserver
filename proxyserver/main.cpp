//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include <signal.h>

#include "../framework/net/runflag.h"
#include "inc/proxy_ctrl.h"
#include "../framework/log/log.h"
#include "../framework/base/base.h"

CRunFlag g_byRunFlag;


void sigusr1_handle(int iSigVal)
{
	g_byRunFlag.SetRunFlag(ERF_RELOAD);
	signal(SIGUSR1, sigusr1_handle);
}


void ignore_pipe()
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);
}

int main(int argc, char **argv)
{
	// 初始化日志信息(临时)
	INIT_ROLLINGFILE_LOG("default", "../log/proxyserver.log", LEVEL_DEBUG, 10*1024*1024, 5);

	// 读取配置
	CServerConfig* pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingleton().LoadFromFile(filepath))
	{
		LOG_ERROR("default","Get TcpserverConfig failed");
		delete pTmpConfig;
		pTmpConfig = NULL;
		exit(0);
	}

	CProxyCtrl* pProxyCtrl = new CProxyCtrl();
	if (NULL == pProxyCtrl)
	{
		LOG_ERROR("default", "new CGateCtrl failed. exit!");
		exit(1);
	}

	if (pProxyCtrl->Initialize())
	{
		LOG_ERROR("default", "CGateCtrl initialize failed.");
		if (pProxyCtrl)
		{
			delete pProxyCtrl;
			pProxyCtrl = NULL;
		}
		exit(2);
	}

	// 创建EHandleType_NUM个线程
	if (pProxyCtrl->PrepareToRun())
	{
		LOG_ERROR("default", "CGateCtrl prepare to fun failed.");
		if (pProxyCtrl)
		{
			delete pProxyCtrl;
			pProxyCtrl = NULL;
		}
		exit(3);
	}

	// 安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);

	LOG_INFO("default", "CGateCtrl is ready now.");

	// GateServer Run
	pProxyCtrl->Run();

	// 服务器退出
	if (pTmpConfig != NULL)
	{
		delete pTmpConfig;
		pTmpConfig = NULL;
	}

	if (pProxyCtrl != NULL)
	{
		delete pProxyCtrl;
		pProxyCtrl = NULL;
	}

	return 0;
}
