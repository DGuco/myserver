//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include <signal.h>

#include "../framework/net/runflag.h"
#include "inc/gate_ctrl.h"
#include "../framework/log/log.h"
#include "../framework/base/base.h"
#include "../framework/json/config.h"


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
	for (int i = 1; i < argc; i++)
	{
		if (!strcasecmp(argv[i], "-v"))
		{
			// 支持版本信息查询
#ifdef _DEBUG_
			printf("gateserver debug build at %s %s\n", __DATE__, __TIME__);
#else
			printf("gateserver release build at %s %s\n", __DATE__, __TIME__);
#endif
			exit(0);
		}
	}

	// 初始化日志信息(临时)
	INIT_ROLLINGFILE_LOG("default", "../log/gateserver.log", LEVEL_DEBUG, 10*1024*1024, 5);

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

	CGateCtrl* pGateCtrl = new CGateCtrl;
	if (NULL == pGateCtrl)
	{
		LOG_ERROR("default", "new CGateCtrl failed. exit!");
		exit(1);
	}

	if (pGateCtrl->Initialize())
	{
		LOG_ERROR("default", "CGateCtrl initialize failed.");
		if (pGateCtrl)
		{
			delete pGateCtrl;
			pGateCtrl = NULL;
		}
		exit(2);
	}

	// 创建EHandleType_NUM个线程
	if (pGateCtrl->PrepareToRun())
	{
		LOG_ERROR("default", "CGateCtrl prepare to fun failed.");
		if (pGateCtrl)
		{
			delete pGateCtrl;
			pGateCtrl = NULL;
		}
		exit(3);
	}

	// 安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);

	LOG_INFO("default", "CGateCtrl is ready now.");

	// GateServer Run
	pGateCtrl->Run();

	// 服务器退出
	if (pTmpConfig != NULL)
	{
		delete pTmpConfig;
		pTmpConfig = NULL;
	}

	if (pGateCtrl != NULL)
	{
		delete pGateCtrl;
		pGateCtrl = NULL;
	}

	return 0;
}
