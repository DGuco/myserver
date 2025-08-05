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
#include <iomanip>
#include "thread_task.h"
#include "lock_free_limit_queue.h"

using namespace std;
using namespace std::chrono;

#define OUT(x) cout << #x << " = " << x << endl
#define O(x) cout << x << endl

struct Timer 
{ 
	Timer() : tp(system_clock::now()) {} 
	virtual ~Timer()
	{ 
		auto dur = system_clock::now() - tp; 
		O("Cost " << duration_cast<milliseconds>(dur).count() << " ms"); 
	} 
	system_clock::time_point tp; 
};

struct Bench : public Timer 
{
	 Bench() : val(0) {} 
	 virtual ~Bench() 
	 { 
		auto dur = system_clock::now() - tp;
		auto value = duration_cast<nanoseconds>(dur).count() / MAX(val, 1L);
		O("Per op: " << value << " ns");
		auto perf = (double)val / duration_cast<milliseconds>(dur).count() / 10;
		if (perf < 1) 
		{
			O("Performance: " << std::setprecision(3) << perf << " w/s");
		}
		else 
		{
			O("Performance: " << perf << " w/s");
		}
	} 
	Bench& operator++() { ++val; return *this; } 
	Bench& operator++(int) { ++val; return *this; } 
	Bench& add(long v) { val += v; } 
	long val; 
};

#define IS_UNIT_TEST 1
#define IS_DEBUG 1

struct A
{
    static std::atomic<long> sCount;
    long val_;
    A() : val_(0) {
        ++sCount;
    }
    A(long val) : val_(val) {
        ++sCount;
    }
    A(A const& a) : val_(a.val_) {
        ++sCount;
    }
    A(A && a) : val_(a.val_) {
        a.val_ = 0;
        ++sCount;
    }
    ~A() {
        --sCount;
    }
    A& operator=(A const& a) {
        val_ = a.val_;
    }
    A& operator=(A && a) 
	{
        val_ = a.val_;
        a.val_ = 0;
		return *this;
    }
    operator long() const { return val_; }
};
std::atomic<long> A::sCount{0};

void AssertCount(long c, A*) {
    ASSERT_EX(A::sCount == c,"AssertCount FALSE");
}

template <typename T>
void AssertCount(long c, T*) {
    (void)c;
}

template <typename T = long,typename int cap = 5>
void test(int count, int rThreads, int wThreads)
{
    LockFreeLimitQueue<T,cap> queue;
    std::atomic_int lastCount{0};
    volatile bool done = false;

#if IS_UNIT_TEST
    AssertCount(0, (T*)nullptr);

    std::vector<std::atomic<long>*> check(wThreads, nullptr);
    for (auto & p : check)
        p = new std::atomic<long>{0};
#endif

    std::vector<thread*> tg;

    // read threads
    for (int i = 0; i < rThreads; ++i) {
        thread *t = new thread([&]{
                    T val;
                    while (!done) {
                        my_std::LockFreeResult result = queue.Pop(val);
                        if (!result.success)
                            continue;

                        long l = val;
                        int threadNumber = l >> 32;
                        int idx = l & 0xffffffff;
#if IS_DEBUG
                        printf("pop thread=%d, idx=%d\n", threadNumber, idx);
#endif
                        if (idx == count) {
                            if (++lastCount == wThreads)
                                done = true;
                        }
#if IS_UNIT_TEST
                        *check[threadNumber] += idx;
#endif
                    }
                });
        tg.push_back(t);
    }

    // write threads
    for (int i = 0; i < wThreads; ++i) {
        thread *t = new thread([&, i]{
                    for (int j = 1; j <= count; ++j) {
                        T val((long)j | (long)i << 32);
                        while (!queue.Push(std::move(val)).success) ;
#if IS_DEBUG
                        printf("push thread=%d, idx=%d\n", i, j);
#endif
                    }
                });
        tg.push_back(t);
    }

    // join
    for (auto pt : tg)
        pt->join();

#if IS_UNIT_TEST
    AssertCount(0, (T*)nullptr);

    long checkTotal = (1 + count) * count / 2;
    for (auto & p : check)
    {
        assert(*p == checkTotal);
        delete p;
    }
#endif
}

using namespace std;
int main(int argc, char **argv)
{
	{
		{
			Timer t;
			test<A,10000>(10000, 10, 10);
		}

		{
			Timer t;
			test<long,10000>(10000, 10, 10);
		}
	}
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

