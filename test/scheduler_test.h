#ifndef _SCHEDULER_TEST_H_
#define _SCHEDULER_TEST_H_

#include "task_helper.h"
#include "thread_scheduler.h"
#include "t_array.h"

CSafePtr<CThreadScheduler> g_LogicScheduler = new CThreadScheduler("GameLogicScheduler");
CSafePtr<CThreadScheduler> g_DBScheduler = new CThreadScheduler("DBScheduler");
CSafePtr<CThreadScheduler> g_HttpScheduler = new CThreadScheduler("DBScheduler");


#define MAX_TEST_SCHEDULER 10
#define MAX_TEST_COUNT 1000000

TArray<CSafePtr<CThreadScheduler>,MAX_TEST_SCHEDULER> g_SchedulerList;


CSafePtr<CThreadScheduler> RandomScheduler()
{
	int index = rand() % MAX_TEST_SCHEDULER;
	return g_SchedulerList[index];
}


CTaskHelper<int> test_scheduler_task()
{
	return RandomScheduler()->Schedule("test_scheduler_task", 
	[]
	{
		int value = 0;
		value++;
		return value; //1
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value; //2
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//3
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//4
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//5
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//6
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//7
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//8
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;
		return value;//9
	}).ThenAccept(RandomScheduler(),[](int value)
	{
		value++;//10
		return value;
	});
}

void schedler_test()
{
	for (size_t i = 0; i < MAX_TEST_SCHEDULER; i++)
	{
		CSafePtr<CThreadScheduler> pScheduler = new CThreadScheduler("TestScheduler");
		g_SchedulerList[i] = pScheduler;
	}

	for (size_t i = 0; i < MAX_TEST_SCHEDULER; i++)
	{
		if (!g_SchedulerList[i]->Init(2))
		{
			return;
		}
	}

	std::atomic<int> count(0);
	std::atomic<int> count_ok(0);
	for (int index = 0; index < MAX_TEST_COUNT; index++)
	{
		auto task1 = test_scheduler_task();
		auto task2 = test_scheduler_task();
		auto task3 = test_scheduler_task();
		auto task4 = test_scheduler_task();
		auto task5 = test_scheduler_task();
		auto task6 = test_scheduler_task();
		auto task7 = test_scheduler_task();
		auto task8 = test_scheduler_task();
		auto task9 = test_scheduler_task();
		auto task10 = test_scheduler_task();
		CThreadScheduler::AcceptCombine(task1,task2,task3,task4,task5,task6,task7,task8,task9,task10)
			.AcceptAll(RandomScheduler(),
			[&count,&count_ok,index](int value1,int value2,int value3,int value4,int value5,int value6,int value7,int value8,int value9,int value10)
			{
				int sum = value1 + value2 + value3 + value4 + value5 + value6 + value7 + value8 + value9 + value10;
				if(sum == 100)
				{
					count_ok++;
				}else
				{
					CACHE_LOG(DEBUG_CACHE, "index = {} sum = {}",index,sum);
				}
				count++;

				if(count == MAX_TEST_COUNT)
				{
					for (size_t i = 0; i < MAX_TEST_SCHEDULER; i++)
					{
						g_SchedulerList[i]->StopScheduler();
					}
				}

				if(count % 1000 == 0)
				{
					CACHE_LOG(DEBUG_CACHE, "index = {} count = {} count_ok = {}",index,count,count_ok);
				}
			});

		CTimeHelper::GetSingletonPtr()->SetTime();
		for (size_t i = 0; i < MAX_TEST_SCHEDULER; i++)
		{
			g_SchedulerList[i]->DebugTask();
		}
	}

	for (size_t i = 0; i < MAX_TEST_SCHEDULER; i++)
	{
		g_SchedulerList[i]->Join();
	}

	CACHE_LOG(DEBUG_CACHE, "testcount = {} count_ok = {}",count,count_ok);
}

#endif