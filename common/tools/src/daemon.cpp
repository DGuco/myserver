#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "../inc/daemon.h"


// 初始化后台运行模式
int InitDaemon(int iInitDaemon, const char* szLockFile)
{
	if (szLockFile == NULL)
		return -1;

	if (SERVER_INIT_DAEMON != iInitDaemon)
	{
		// 如果不需要后台方式运行，返回成功
		return 0;
	}

	int lock_fd;
	lock_fd = open(szLockFile, O_RDWR|O_CREAT, 0640);
	if (lock_fd < 0)
	{
		printf("lock file failed. open file failed, lock_fd = %d, error = %s.\n",
				lock_fd, strerror(errno));
		return -2;
	}

	int ret;
	ret = flock(lock_fd, LOCK_EX|LOCK_NB);
	if (ret < 0)
	{
		printf("lock file failed. flock failed, ret = %d, error = %s.\n",
				ret, strerror(errno));
		return -3;
	}

	if (SERVER_INIT_DAEMON != iInitDaemon)
	{
		// 如果不需要后台方式运行，返回成功
		return 0;
	}

//	int i;
	pid_t pid;

	pid = fork();
	if (pid != 0)
	{
		if (pid < 0)
		{
			printf("first fork failed, pid = %d, error = %s.\n", pid, strerror(errno));
		}
		exit(0);
	}

	setsid();

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGHUP, &sig, NULL);

	pid = fork();
	if (pid != 0)
	{
		if (pid < 0)
		{
			printf("second fork failed, pid = %d, error = %s.\n", pid, strerror(errno));
		}
		exit(0);
	}

	char tPath[256];
	long tPathLen = sizeof(tPath) - 1;
	char* cRet = getcwd(tPath, tPathLen);
	if (cRet == NULL)
	{
		printf("getcwd failed.\n");
		exit(0);
	}

	if (chdir((const char*)tPath))
	{
		printf("can't change run dir to %s after init daemon, error = %s, exit.\n", tPath, strerror(errno));
		exit(0);
	}

	umask(0);
	setpgrp();

	return 0;
}

