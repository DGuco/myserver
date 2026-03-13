#include "lock_free_limit_queue_test.h"
#include "rbt_hash_map_test.h"
#include "atomic_test.h"
#include "scheduler_test.h"
#include "time_helper.h"

#pragma pack(push, 8)
struct  AAA
{
    int a;
    char a_;
    double d;
    int b;
    char b_;
};
#pragma pack(pop)

char arr[1];

using namespace std;
int main(int argc, char **argv)
{
	if (!INIT_LOG("test"))
	{
		exit(0);
	}
    AAA aaa;
    int size = sizeof(arr);
    float count = 0;
    for(int i = 0; i < 70; ++i)
    {
        float count_ = 1 * 20 / 100.0f;
        count += count_;
    }
    int final_count = (int)count;
    // 测试lock_free_limit_queue
    //TestLockFreeLimitQueue();

    // 测试rbt_hash_map
    //Test_RbtHashMap();

    // 测试atomic 内存序
    //test_memory_order();

    // 测试线程调度器
    schedler_test();

    INT testDay = CTimeHelper::GetSingletonPtr()->Time2DayAfter(20260101, -40);
	// 关闭日志
	SHUTDOWN_ALL_LOG();
    
    getchar();

	return 0;
}
