#include <atomic>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <chrono>

const int TEST_ROUNDS = 10000;
const int THREAD_PAIRS = 100;
const int TEST_DELAY = 10;

// 全局原子变量
std::atomic<bool> x[THREAD_PAIRS]{false};
std::atomic<bool> y[THREAD_PAIRS]{false};
// 结果统计
std::atomic<int> reorder_count{0};
std::atomic<int> total_runs{0};


// 全局原子变量
std::atomic<bool> xx_[THREAD_PAIRS]{false};
std::atomic<bool> yy_[THREAD_PAIRS]{false};
// 结果统计
std::atomic<int> reorder_count_xx{0};
std::atomic<int> total_runs_xx{0};

// 线程1: 存储x→存储y
void thread_store(int index) 
{
    x[index].store(false, std::memory_order_relaxed);
    y[index].store(false, std::memory_order_relaxed);

    /*
    1. 打破执行节奏的确定性
    无延迟时，线程1和线程2的操作可能呈现固定时序关系（如严格交替执行），CPU重排序难以被观测：
    线程1: x.store(true) → y.store(true)  (快速连续执行)
    线程2: y.load() → x.load()            (几乎同时完成)
    加入随机延迟后，线程执行节奏被打乱，创造出时间窗口差异，使CPU重排序更易暴露：

    线程1: x.store(true) → [随机延迟10ns] → y.store(true)
    线程2: [随机延迟5ns] → y.load() → x.load()
    // 此时可能观察到 y=true 但 x=false 的重排结果
    2. 延长指令调度的不确定性窗口
    现代CPU采用乱序执行引擎（Out-of-Order Execution），会动态调整指令执行顺序。延迟操作（如std::this_thread::sleep_for）会：

    强制CPU刷新指令流水线
    导致缓存行状态变化（MESI协议状态切换）
    增加分支预测失误的概率 这些因素都会扩大指令重排序的可能性，使原本可能被"隐藏"的重排行为变得可观测。
    3. 放大内存操作的竞争窗口
    在memory_order_relaxed模式下，原子操作仅保证原子性而不保证可见性顺序。延迟操作会：

    增加两个线程操作交叉执行的概率
    延长共享变量在CPU缓存中的驻留时间
    提高不同核心间缓存一致性协议的交互复杂度 例如，线程1的x.store可能因延迟未及时刷新到主存，而线程2已读取到y.store的结果。
    4. 统计学效应：增加样本空间
    随机延迟本质上是一种压力测试手段，通过引入不确定性来模拟更广泛的执行场景：
    短延迟（1-100ns）可触发CPU微架构级重排序（如StoreLoad重排）
    长延迟（1-10us）可触发系统级重排序（如总线事务重排） 在足够多的测试轮次下，这些场景的重排概率会被显著放大。
    */
    if(TEST_DELAY > 0)
    {
        // 随机延迟增加重排序概率
        int delay = rand() % TEST_DELAY;
        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
    }

    x[index].store(true, std::memory_order_relaxed);
    // 关键存储操作: 若发生重排序，可能先于x的存储被观察到
    y[index].store(true, std::memory_order_relaxed);
}

// 线程2: 加载y→加载x
void thread_load(int index) 
{
    bool y_loaded, x_loaded;
    do {
        
        y_loaded = y[index].load(std::memory_order_relaxed);

        // if(TEST_DELAY > 0)
        // {
        //     // 随机延迟增加观察窗口
        //     int delay = rand() % TEST_DELAY;
        //     std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        // }

        x_loaded = x[index].load(std::memory_order_relaxed);
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
    xx_[index].store(false, std::memory_order_release);
    yy_[index].store(false, std::memory_order_release);

    if(TEST_DELAY > 0)
    {
        // 随机延迟增加重排序概率
        int delay = rand() % TEST_DELAY;
        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
    }

    xx_[index].store(true, std::memory_order_release);
    // 关键存储操作: 若发生重排序，可能先于x的存储被观察到
    yy_[index].store(true, std::memory_order_release);
}

// 线程2: 加载y→加载x
void thread_load_acquire(int index) 
{
    bool y_loaded, x_loaded;
    do {
        y_loaded = yy_[index].load(std::memory_order_acquire);

        // if(TEST_DELAY > 0)
        // {
        //     // 随机延迟增加观察窗口
        //     int delay = rand() % TEST_DELAY;
        //     std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        // }

        x_loaded = xx_[index].load(std::memory_order_acquire);

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


