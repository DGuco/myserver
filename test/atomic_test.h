#include <atomic>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <chrono>

const int TEST_ROUNDS = 10000;
const int THREAD_PAIRS = 100;
const int TEST_DELAY = 0;

// 全局原子变量
bool x[THREAD_PAIRS]{false};
std::atomic<bool> y[THREAD_PAIRS]{false};
int cache_filler[1000]{0};

// 结果统计
std::atomic<int> reorder_count{0};
std::atomic<int> total_runs{0};


// 全局原子变量
bool xx_[THREAD_PAIRS]{false};
std::atomic<bool> yy_[THREAD_PAIRS]{false};
// 结果统计
std::atomic<int> reorder_count_xx{0};
std::atomic<int> total_runs_xx{0};

// 线程1: 存储x→存储y
void thread_store(int index) 
{
    x[index] = false;
    for (int i = 0; i < 1000; ++i) 
    {
        volatile int tmp = cache_filler[i]; // 干扰CPU缓存
    }
    x[index] = true;
    // 关键存储操作: 若发生重排序，可能先于x的存储被观察到
    y[index].store(true, std::memory_order_relaxed);
}

// 线程2: 加载y→加载x
void thread_load(int index) 
{
    bool y_loaded, x_loaded;
    do {
        
        y_loaded = y[index].load(std::memory_order_relaxed);

        if(TEST_DELAY > 0)
        {
            // 随机延迟增加观察窗口
            int delay = rand() % TEST_DELAY;
            std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        }

        x_loaded = x[index];
        // 检测重排序现象: y已被设置但x未被设置
        if (y_loaded && !x_loaded) 
        {
            reorder_count.fetch_add(1, std::memory_order_relaxed);
            break;
        }
    } while (!y_loaded || !x_loaded); // 等待两个变量都被设置

    total_runs.fetch_add(1, std::memory_order_relaxed);
}

// 线程1: 存储x→存储y
void thread_store_release(int index) 
{
    xx_[index] = false;
    for (int i = 0; i < 1000; ++i) 
    {
        volatile int tmp = cache_filler[i]; // 干扰CPU缓存
    }
    xx_[index] = true;
    // 关键存储操作: 若发生重排序，可能先于x的存储被观察到
    yy_[index].store(true, std::memory_order_release);
}

// 线程2: 加载y→加载x
void thread_load_acquire(int index) 
{
    bool y_loaded, x_loaded;
    do 
    {
        y_loaded = yy_[index].load(std::memory_order_acquire);
        x_loaded = xx_[index];
        // 检测重排序现象: y已被设置但x未被设置
        if (y_loaded && !x_loaded) 
        {
            reorder_count_xx.fetch_add(1, std::memory_order_relaxed);
            break;
        }
    } while (!y_loaded || !x_loaded); // 等待两个变量都被设置

    total_runs_xx.fetch_add(1, std::memory_order_relaxed);
}

// 测试主函数
void test_memory_order_relaxed() 
{
    for (int i = 0; i < TEST_ROUNDS; ++i) 
    {
        std::thread t1[THREAD_PAIRS];
        std::thread t2[THREAD_PAIRS];

        for (int j = 0; j < THREAD_PAIRS; ++j) 
        {
            t1[j] = std::thread(thread_store, j);
            t2[j] = std::thread(thread_load, j);
        }

        for (int j = 0; j < THREAD_PAIRS; ++j) 
        {
            t1[j].join();
            t2[j].join();
        }

        // 每100轮输出一次统计
        if (i % 100 == 0) 
        {
            printf("memory_order_relaxed Round %d: Reorder failed times %d \n",i, reorder_count.load());
        }
    }
}

void test_memory_order_release_acquire() 
{
    for (int i = 0; i < TEST_ROUNDS; ++i) 
    {
        std::thread t1[THREAD_PAIRS];
        std::thread t2[THREAD_PAIRS];

        for (int j = 0; j < THREAD_PAIRS; ++j) 
        {
            t1[j] = std::thread(thread_store_release, j);
            t2[j] = std::thread(thread_load_acquire, j);
        }

        for (int j = 0; j < THREAD_PAIRS; ++j) 
        {
            t1[j].join();
            t2[j].join();
        }

        // 每100轮输出一次统计
        if (i % 100 == 0) 
        {
            printf("memory_order_release_acquire Round %d: Reorder failed times %d \n",i, reorder_count_xx.load());
        }
    }
}

void test_memory_order()
{
    test_memory_order_relaxed();
    test_memory_order_release_acquire();

    printf("Final result test_memory_order_relaxed : Reorder detected %d times in %d runs (%.4f%%)\n",
        reorder_count.load(), total_runs.load(),
        (float)reorder_count * 100 / total_runs);

    printf("Final result test_memory_order_release_acquire: Reorder detected %d times in %d runs (%.4f%%)\n",
        reorder_count_xx.load(), total_runs_xx.load(),
        (float)reorder_count_xx * 100 / total_runs_xx);
}