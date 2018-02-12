//
// Created by DGuco on 17-7-12.
//

#include "share_mem.h"
#include "config.h"
#include "./inc/dbctrl.h"
#include <signal.h>

int Initialize(int iInitFlag = 0);

void sigusr1_handle(int iSigVal)
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag(CDBCtrl::EFLG_CTRL_RELOAD);
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int iSigVal)
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag(CDBCtrl::EFLG_CTRL_QUIT);
	signal(SIGUSR2, sigusr2_handle);
}

void sigusr_handle(int iSigVal)
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag(CDBCtrl::EFLG_CTRL_SHUTDOWN);
	signal(SIGQUIT, sigusr_handle);
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
	INIT_ROLLINGFILE_LOG("default", "../log/dbserver.log", LEVEL_DEBUG, 10 * 1024 * 1024, 20);
//	INIT_ROLLINGFILE_LOG( "dbctrl", "../log/dbserver.log", LEVEL_DEBUG, 10 * 1024 * 1024, 20 );

	CDBCtrl *tpDBCtrl = new CDBCtrl;
	if (tpDBCtrl->Initialize() < 0)  // 读取数据库配置文件
	{
		exit(-1);
	}

	if (tpDBCtrl->PrepareToRun() < 0)  // 创建处理线程
	{
		exit(-1);
	}

	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);
	signal(SIGQUIT, sigusr_handle);

	{
		printf("-------------------------------------------------\n");
		printf("|          dbserver startup success!          |\n");
		printf("-------------------------------------------------\n");
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

