#include "lock_free_limit_queue_test.h"
#include "rbt_hash_map_test.h"
#include "atomic_test.h"
#include "scheduler_test.h"


using namespace std;
int main(int argc, char **argv)
{
	if (!INIT_LOG("test"))
	{
		exit(0);
	}
    
    // 测试lock_free_limit_queue
    //TestLockFreeLimitQueue();

    // 测试rbt_hash_map
    //Test_RbtHashMap();

    // 测试atomic 内存序
    //test_memory_order();

    // 测试线程调度器
    schedler_test();


	// 关闭日志
	SHUTDOWN_ALL_LOG();
    
    getchar();

	return 0;
}
