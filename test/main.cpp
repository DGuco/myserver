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

/*
=================================================================
==3262==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x60400000dff8 at pc 0x0000004007db bp 0x7ffc1f3104a0 sp 0x7ffc1f310490
WRITE of size 4 at 0x60400000dff8 thread T0
    #0 0x4007da in main /home/dguco/Workspace/cpp/ascan/program.c:6
    #1 0x7f0d8be5a83f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x2083f)
    #2 0x4006b8 in _start (/home/dguco/Workspace/cpp/ascan/program+0x4006b8)

0x60400000dff8 is located 0 bytes to the right of 40-byte region [0x60400000dfd0,0x60400000dff8)
allocated by thread T0 here:
    #0 0x7f0d8c29c662 in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.2+0x98662)
    #1 0x400797 in main /home/dguco/Workspace/cpp/ascan/program.c:5
    #2 0x7f0d8be5a83f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x2083f)

SUMMARY: AddressSanitizer: heap-buffer-overflow /home/dguco/Workspace/cpp/ascan/program.c:6 main
Shadow bytes around the buggy address:
  0x0c087fff9ba0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9bb0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9bc0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9bd0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9be0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
=>0x0c087fff9bf0: fa fa fa fa fa fa fa fa fa fa 00 00 00 00 00[fa]
  0x0c087fff9c00: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9c10: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9c20: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9c30: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff9c40: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
==3262==ABORTING


*/