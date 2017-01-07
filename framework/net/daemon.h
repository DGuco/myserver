#ifndef __DAEMON_H__
#define __DAEMON_H__

enum EDAEMON
{
	SERVER_INIT_UNDAEMON 	= 0,
	SERVER_INIT_DAEMON 		= 1
};

// 初始化后台运行模式
int InitDaemon(int iInitDaemon, const char* szLockFile);

#endif // __DAEMON_H__
