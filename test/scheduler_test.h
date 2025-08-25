#ifndef _SCHEDULER_TEST_H_
#define _SCHEDULER_TEST_H_

#include "task_helper.h"
#include "thread_scheduler.h"

CSafePtr<CThreadScheduler> g_LogicScheduler = new CThreadScheduler("GameLogicScheduler");
CSafePtr<CThreadScheduler> g_DBScheduler = new CThreadScheduler("DBScheduler");
CSafePtr<CThreadScheduler> g_HttpScheduler = new CThreadScheduler("DBScheduler");

void schedler_test()
{
	if (!g_LogicScheduler->Init(1))
	{
		return;
	}

	if (!g_DBScheduler->Init(1))
	{
		return ;
	}

	
	if (!g_HttpScheduler->Init(1))
	{
		return ;
	}

    int a = 1;
	int b = 2;
	int c = 10;
	int d = 20;
	g_LogicScheduler->Schedule("InitTcpServer",
		[a, b]
		{
			return a + b;
		})
	    .ThenAccept(g_DBScheduler,
			[c, d] (int res)
			{
				int finres = c + d + res;
				printf("finres = %d\n", finres);
				throw std::runtime_error("run time error");
                //return finres;
			})
		.ThenApply(g_HttpScheduler,
			[c, d]
			{
				int finres = c + d;
				printf("finres = %d\n", finres);
                return finres;
			});


	auto task1 = CThreadScheduler::Schedule(g_LogicScheduler, "CombineTask1",
		[a, b]
		{
			return a + b;
		});

	auto task2 = CThreadScheduler::Schedule(g_LogicScheduler, "CombineTask2",
		[]
		{
			return "Task2 execute done ";
		});

	auto task3 = CThreadScheduler::Schedule(g_DBScheduler, "CombineTask3",

		[a, b]
		{
			return a / b;
		});

	auto task4 = CThreadScheduler::Schedule(g_DBScheduler, "CombineTask4",
		[a, b]
		{
			return a % b;
		});

	CThreadScheduler::AcceptCombine(task1, task2, task3, task4)
		.AcceptAll(g_HttpScheduler,
			[](int res1, std::string res2, int res3, int res4)
			{
				int nRes = res1 + res3 + res4;
				printf("fTask res :{%s},combineres = {%d}\n", res2.c_str(), nRes);
				return nRes;
			});
}

#endif