#ifndef __LOCK_FREE_LIMIT_QUEUE_TEST_H__
#define __LOCK_FREE_LIMIT_QUEUE_TEST_H__

#include <functional>
#include <tuple>
#include <iomanip>
#include <iostream>
#include "thread_task.h"
#include "log.h"
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

template <typename T = uint64_t,typename int cap = 5>
void test(int count, int rThreads, int wThreads)
{
    LockFreeLimitQueue<T,cap> queue;
    std::atomic_int lastCount{0};
    volatile bool done = false;

#if IS_UNIT_TEST
    AssertCount(0, (T*)nullptr);

    std::vector<std::atomic<uint64_t>*> check(wThreads, nullptr);
    for (auto & p : check)
        p = new std::atomic<uint64_t>{0};
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

                        uint64_t l = val;
                        uint32_t threadNumber = l >> 32;
                        uint32_t idx = l & 0xffffffff;
#if IS_DEBUG
		                CACHE_LOG(DEBUG_CACHE, "pop thread = {}, idx = {} ",threadNumber, idx);
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
                        T val((uint32_t)j | (uint32_t)i << 32);
                        while (!queue.Push(std::move(val)).success) ;
#if IS_DEBUG
		                CACHE_LOG(DEBUG_CACHE, "push thread = {}, idx = {} ",i, j);
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

void TestLockFreeLimitQueue()
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

#endif