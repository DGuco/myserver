#ifndef _SCHEDULER_TEST_H_
#define _SCHEDULER_TEST_H_

#include "task_helper.h"
#include "thread_scheduler.h"

CSafePtr<CThreadScheduler> g_LogicScheduler = new CThreadScheduler("GameLogicScheduler");
CSafePtr<CThreadScheduler> g_DBScheduler = new CThreadScheduler("DBScheduler");
CSafePtr<CThreadScheduler> g_HttpScheduler = new CThreadScheduler("DBScheduler");

void schedler_test()
{
    int a = 1;
	int b = 2;
	int c = 10;
	int d = 20;
	CThreadScheduler::CreateTask(g_LogicScheduler, "InitTcpServer",
		[a, b]
		{
			return a + b;
		}
		)
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
			})
        .Run();


	auto task1 = CThreadScheduler::CreateTask(g_LogicScheduler, "CombineTask1",
		[a, b]
		{
			return a + b;
		});

	auto task2 = CThreadScheduler::CreateTask(g_LogicScheduler, "CombineTask2",
		[]
		{
			return "Task2 execute done ";
		});

	auto task3 = CThreadScheduler::CreateTask(g_DBScheduler, "CombineTask3",

		[a, b]
		{
			return a / b;
		});

	auto task4 = CThreadScheduler::CreateTask(g_DBScheduler, "CombineTask4",
		[a, b]
		{
			return a % b;
		});

	CThreadScheduler::AcceptCombine(task1, task2, task3, task4)
		.AcceptAll(g_HttpScheduler,
			[](int res1, std::string res2, int res3, int res4)
			{
				int nRes = res1 + res3 + res4;
				CACHE_LOG(DEBUG_CACHE, "Task res :{},combineres = {}", res2, nRes);
			})
        .Run();
}

#endif