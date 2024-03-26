//
// Created by DGuco on 17-7-12.
//

#include "share_mem.h"
#include "config.h"
#include "./inc/dbctrl.h"
#include <signal.h>

int Initialize(int iInitFlag = 0);


void sigpipe_handle(int sig)
{
    LOG_ERROR("default", "receive sigpipe,do sigpipe_handle");
}


int main(int argc, char **argv)
{
	CDBCtrl *tpDBCtrl = new CDBCtrl;
	if (tpDBCtrl->PrepareToRun() < 0)  // 创建处理线程
	{
		exit(-1);
	}

    struct sigaction sa;
    sa.sa_handler = sigpipe_handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE,&sa,NULL);

	{
		LOG_INFO("default", "-------------------------------------------------");
		LOG_INFO("default", "|          dbserver startup success!          |");
		LOG_INFO("default", "-------------------------------------------------");
	}

	tpDBCtrl->Run();

	if (tpDBCtrl) {
		delete tpDBCtrl;
		tpDBCtrl = NULL;
	}

	// 关闭所有日志
	LOG_SHUTDOWN_ALL;

	return 0;
}

