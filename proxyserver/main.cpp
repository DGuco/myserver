//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#include "proxy_ctrl.h"
#include "signal_handler.h"
#include <functional>
#include <tuple>
#include "thread_task.h"

template<class Func, class... Args>
void Call(const Func f, Args...args)
{
	CThreadTask* pTask = new CParamTask<Func,Args...>("test task",f,std::make_tuple(args...));
	pTask->Execute();
}

using namespace std;
int main(int argc, char **argv)
{
	int a = 2;
	Call([a](int b,int c,std::string str) 
		{
			printf("aaaaaa = %d\n",a + b + c); 
			printf("str = %s\n", str.c_str());
			return a + b + c;
		},3,4,"Hello world");
	//信号处理注册
	CSignalHandler::GetSingletonPtr()->RegisterHandler("proxyserver");

	if (!INIT_LOG("proxyserver"))
	{
		exit(0);
	}

	try
	{
		int iTmpRet = CProxyCtrl::GetSingletonPtr()->PrepareToRun();
		if (!iTmpRet)
		{
			DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
			exit(0);
		}
	}
	catch (const std::exception& e)
	{
		DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,get exception = {}", e.what());
		exit(0);
	}

	CProxyCtrl::GetSingletonPtr()->Run();
	// 关闭日志
	SHUTDOWN_ALL_LOG();
	return 0;
}

