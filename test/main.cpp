#include "lock_free_limit_queue_test.h"

using namespace std;
int main(int argc, char **argv)
{
	if (!INIT_LOG("test"))
	{
		exit(0);
	}

    TestLockFreeLimitQueue();
	// πÿ±’»’÷æ
	SHUTDOWN_ALL_LOG();
	return 0;
}

