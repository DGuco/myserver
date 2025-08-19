#include "lock_free_limit_queue_test.h"
#include "rbt_hash_map_test.h"
#include "atomic_test.h"

using namespace std;
int main(int argc, char **argv)
{
	if (!INIT_LOG("test"))
	{
		exit(0);
	}
    
    // ≤‚ ‘lock_free_limit_queue
    //TestLockFreeLimitQueue();

    // ≤‚ ‘rbt_hash_map
    //Test_RbtHashMap();

    // ≤‚ ‘atomic ƒ⁄¥Ê–Ú
    test_memory_order();

	// πÿ±’»’÷æ
	SHUTDOWN_ALL_LOG();
    getchar();
	return 0;
}
