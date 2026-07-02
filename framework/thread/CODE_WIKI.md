# Thread 模块 Code Wiki

> 仓库路径：`framework/thread`
> 作者：DGuco (1139140929@qq.com)
> 文档生成日期：2026-07-02

---

## 目录

1. [项目概述](#1-项目概述)
2. [整体架构](#2-整体架构)
3. [模块职责](#3-模块职责)
4. [文件清单](#4-文件清单)
5. [关键类与函数说明](#5-关键类与函数说明)
6. [核心流程](#6-核心流程)
7. [依赖关系](#7-依赖关系)
8. [并发与同步原语](#8-并发与同步原语)
9. [项目运行方式](#9-项目运行方式)
10. [设计要点与注意事项](#10-设计要点与注意事项)

---

## 1. 项目概述

`thread` 模块是 `myserver` 框架下的一个**跨平台、基于任务（Task）抽象的线程调度库**。它在 C++11 基础上提供：

- **跨平台线程封装**（Windows `CreateThread` / Linux `pthread`）。
- **互斥锁、读写锁、自旋锁**等多种同步原语。
- **任务（Task）抽象**：把可执行单元封装为带状态、可组合的对象，支持返回值模板化。
- **任务调度器（TaskScheduler）**：单线程驱动的任务队列。
- **线程调度器（ThreadScheduler）**：多线程工作池，基于调度器扩展。
- **任务链式编排（TaskHelper）**：类似 `Future` 的 `ThenApply` / `ThenAccept` 与 `AcceptAll` / `AcceptAny` / `ApplyAll` / `ApplyAny` 组合语义。

整体定位是为上层 server（网络、DB、Lua 等）提供统一的异步任务执行与编排能力。

---

## 2. 整体架构

```
            ┌──────────────────────────────────────────────────────┐
            │                  上层业务 (server)                     │
            └───────────────────────┬──────────────────────────────┘
                                    │  Schedule / ThenApply / AcceptAll ...
                                    ▼
   ┌──────────────────────────────────────────────────────────────────┐
   │                     thread 模块                                  │
   │                                                                  │
   │   ┌───────────────────┐        ┌────────────────────────────┐    │
   │   │  CThreadScheduler │ ─────▶ │  CTaskScheduler            │    │
   │   │  (多线程工作池)    │  继承  │  (单线程任务队列 + 模板API) │    │
   │   └─────────┬─────────┘        └──────────┬─────────────────┘    │
   │             │ 持有                         │ 持有 / 调度           │
   │             ▼                              ▼                      │
   │   ┌───────────────────┐        ┌────────────────────────────┐    │
   │   │  CTaskThread      │ ─────▶ │  CTask (任务基类)           │    │
   │   │  (CMyThread 子类) │  Run   │  ├─ CCombineTask<N>        │    │
   │   └─────────┬─────────┘        │  ├─ CWithReturnTask<...>   │    │
   │             │ 继承              │  └─ CNoReturnTask<...>     │    │
   │             ▼                  └──────────┬─────────────────┘    │
   │   ┌───────────────────┐                   │ 由 TaskHelper 编排    │
   │   │  CMyThread        │                   ▼                      │
   │   │  (跨平台线程)      │        ┌────────────────────────────┐    │
   │   └───────────────────┘        │  CTaskHelper<R>            │    │
   │                                 │  CAcceptCombineTaskHelper │    │
   │   ┌──────────────────────────┐  │  CApplyCombineTaskHelper  │    │
   │   │  锁原语                  │  └────────────────────────────┘    │
   │   │  CMyLock/CSafeLock       │                                    │
   │   │  CMyRWLock/CSafeRLock... │                                    │
   │   │  CSpinLock/CSafeSpLock   │                                    │
   │   │  CSpinRWLock/...         │                                    │
   │   └──────────────────────────┘                                    │
   └──────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼ 依赖
            ┌──────────────────────────────────────────────────────┐
            │  framework/base (base.h, log.h, time_helper.h,       │
            │                   platform_def.h, my_assert.h ...)    │
            │  framework/std  (safe_pointer.h, t_array.h ...)       │
            └──────────────────────────────────────────────────────┘
```

**核心设计思路**：

- **线程 = 执行载体**：`CMyThread` / `CTaskThread` 只负责跑循环、调用调度器。
- **任务 = 工作单元**：`CTask` 体系把函数+参数+返回值+状态机+子任务链打包。
- **调度器 = 队列 + 线程池**：`CTaskScheduler` 维护一个 `std::queue<TaskPtr>`；`CThreadScheduler` 持有 N 个 `CTaskThread` 消费同一个队列。
- **链式调用 = Future 风格**：`CTaskHelper` 通过子任务队列实现 `Then*` 与 `Accept/Apply *`。

---

## 3. 模块职责

| 模块/文件 | 职责 |
|-----------|------|
| [my_thread.h](file:///e:/workspace/github/myserver/framework/thread/my_thread.h) / [my_thread.cpp](file:///e:/workspace/github/myserver/framework/thread/my_thread.cpp) | 跨平台线程抽象基类 `CMyThread`，封装线程创建、退出、状态机、线程局部数据。 |
| [my_lock.h](file:///e:/workspace/github/myserver/framework/thread/my_lock.h) | 互斥锁 `CMyLock`、读写锁 `CMyRWLock` 及其 RAII 包装类；Windows 下退化为 `std::mutex`。 |
| [spin_lock.h](file:///e:/workspace/github/myserver/framework/thread/spin_lock.h) | 自旋锁 `CSpinLock`、自旋读写锁 `CSpinRWLock` 及 RAII 包装类。 |
| [task.h](file:///e:/workspace/github/myserver/framework/thread/task.h) / [task.cpp](file:///e:/workspace/github/myserver/framework/thread/task.cpp) | 任务体系：`CTask` 基类、`CCombineTask<N>` 组合任务、`CWithReturnTask` / `CNoReturnTask` 模板任务、`TaskCaller` 调用辅助。 |
| [task_helper.h](file:///e:/workspace/github/myserver/framework/thread/task_helper.h) | 任务创建工厂 `TaskCreater` / `CombineTaskCreater`、链式 API `CTaskHelper<R>`、组合 API `CAcceptCombineTaskHelper` / `CApplyCombineTaskHelper`、参数类型信息 `IArgsTypeInfo` / `CArgsTypeList`。 |
| [task_scheduler.h](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.h) / [task_scheduler.cpp](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.cpp) | 任务调度器 `CTaskScheduler`（队列消费 + 模板调度 API）、调度线程 `CTaskThread`。 |
| [thread_scheduler.h](file:///e:/workspace/github/myserver/framework/thread/thread_scheduler.h) / [thread_scheduler.cpp](file:///e:/workspace/github/myserver/framework/thread/thread_scheduler.cpp) | 多线程调度器 `CThreadScheduler`，持有多个 `CTaskThread` 组成工作线程池。 |

---

## 4. 文件清单

```
framework/thread/
├── my_thread.h          # CMyThread 线程基类 + ThreadProc 入口
├── my_thread.cpp        # 线程创建/退出/Join 实现
├── my_lock.h            # 互斥锁 / 读写锁 (Linux 原生, Windows 退化)
├── spin_lock.h          # 自旋锁 / 自旋读写锁 (全平台, atomic_flag/atomic)
├── task.h               # CTask 体系 (模板任务)
├── task.cpp             # CTask 非模板成员实现
├── task_helper.h        # CTaskHelper / TaskCreater / CombineTaskCreater / IArgsTypeInfo
├── task_scheduler.h     # CTaskScheduler + CTaskThread
├── task_scheduler.cpp   # 调度器与调度线程实现
├── thread_scheduler.h   # CThreadScheduler (工作线程池)
└── thread_scheduler.cpp # 线程池 Init/Stop/Join 实现
```

---

## 5. 关键类与函数说明

### 5.1 `CMyThread`（线程基类）

文件：[my_thread.h](file:///e:/workspace/github/myserver/framework/thread/my_thread.h#L48-L92)

跨平台线程封装，定义线程生命周期模板方法。

**状态机** `ThreadStatus`：
- `READY` → `RUNNING` → `EXITING` → `EXIT`

**关键成员/方法**：

| 成员 | 说明 |
|------|------|
| `virtual bool PrepareToRun() = 0` | 纯虚，子类在线程启动后、`Run` 前调用，做初始化。 |
| `virtual bool PrepareEnd() = 0` | 纯虚，子类在 `Run` 退出后调用，做清理。 |
| `virtual void Run() = 0` | 纯虚，线程主循环。 |
| `bool CreateThread()` | 创建底层线程（Linux `pthread_create` / Windows `CreateThread`）。 |
| `void Stop()` | 设置 `m_bStoped = true`，通知线程退出循环。 |
| `void Join()` | 等待线程结束。 |
| `void Exit()` | 退出当前线程。 |
| `void SetThreadInitFunc(...)` / `SetThreadTickFunc(...)` | 设置每轮循环前的 init / tick 回调。 |
| `CSafePtr<thread_data> GetThreadData()` | 获取绑定到本线程的 `thread_data`。 |

**线程入口** `ThreadProc`（[my_thread.cpp](file:///e:/workspace/github/myserver/framework/thread/my_thread.cpp#L93-L137)）：
绑定全局 `thread_local thread_data g_thread_data` 到当前 `CMyThread`，依次调用 `PrepareToRun → Run → PrepareEnd → Exit`，并更新状态机。

**`thread_data` 结构**（[my_thread.h:23-28](file:///e:/workspace/github/myserver/framework/thread/my_thread.h#L23-L28)）：
```cpp
struct thread_data {
    std::tm                  m_CacheTime;       // 缓存时间
    TimePoint                m_CacheTimePoint;  // 缓存时间点
    CSafePtr<CTaskScheduler> own_scheduler;     // 当前线程所属调度器
};
```
通过 `thread_local` 让每个线程知道"自己归属的调度器"，`CTask::Run()` 据此判断是直接执行还是入队。

---

### 5.2 锁原语

#### `CMyLock` / `CSafeLock`（[my_lock.h](file:///e:/workspace/github/myserver/framework/thread/my_lock.h)）
- Linux：原生 `pthread_mutex_t` 封装。
- Windows：直接 `#define CMyLock std::mutex`、`#define CSafeLock std::lock_guard<std::mutex>`。
- `CSafeLock` 是 RAII 自动加解锁。

#### `CMyRWLock` / `CSafeRLock` / `CSafeWLock`
- Linux：`pthread_rwlock_t`；Windows 退化为 `std::lock_guard<std::mutex>`（不区分读写）。

#### `CSpinLock` / `CSafeSpLock`（[spin_lock.h](file:///e:/workspace/github/myserver/framework/thread/spin_lock.h#L7-L49)）
- 基于 `std::atomic_flag` + `test_and_set(acquire)` / `clear(release)`。
- `TryLock()` 非阻塞尝试。
- `CSafeSpLock` RAII 包装。

#### `CSpinRWLock` / `CSafeSpinRLock` / `CSafeSpinWLock`（[spin_lock.h:51-145](file:///e:/workspace/github/myserver/framework/thread/spin_lock.h#L51-L145)）
- 单个 `std::atomic<uint32_t> state`：
  - 最高位（`0x80000000`）= 写标志。
  - 低 31 位 = 读计数。
- `RLock`：CAS 自旋，等待写标志清除后读计数 +1。
- `WLock`：先 CAS 置写标志（自旋），再自旋等待所有读者释放。
- `UnlockR` / `UnlockW` 分别减计数 / 清零。
- `CACHE_LINE_ALIGN` 填充避免 false sharing。

---

### 5.3 `CTask` 任务体系

#### 任务状态 `enTaskState`（[task.h:26-33](file:///e:/workspace/github/myserver/framework/thread/task.h#L26-L33)）
```
eTaskInit → eTaskWaitingFoDoing → eTaskDoing → eTaskDone
                                              └→ eTaskFailed
```

#### 组合类型 `enCombineType`（[task.h:35-40](file:///e:/workspace/github/myserver/framework/thread/task.h#L35-L40)）
- `eCombineNone`：普通任务。
- `eCombineAll`：所有父任务完成后触发。
- `eCombineAny`：任一父任务完成后触发。

#### `CTask` 基类（[task.h:129-189](file:///e:/workspace/github/myserver/framework/thread/task.h#L129-L189)）

| 方法 | 说明 |
|------|------|
| `void Run()` | 核心入口：若当前线程归属调度器 == 任务调度器，则同步执行；否则 `PushTask` 入对端队列（跨线程投递）。捕获异常 → `OnFailed()`。 |
| `virtual void Execute() = 0` | 子类实现真正逻辑。 |
| `virtual void ExecuteChildTask(TaskPtr) = 0` | 把父任务结果传给子任务并触发。 |
| `virtual void ExecuteFromParent(void* pRes, bool sucess) = 0` | 作为子任务，被父任务回调。 |
| `virtual void* GetRes() = 0` | 获取返回值地址。 |
| `void AddChildTask(TaskPtr)` | 加入子任务队列。 |
| `void RunChildTask()` | 遍历子任务队列，普通任务调 `ExecuteChildTask`，组合任务调 `CombineTaskDone`。 |
| `void OnFinish()` / `OnFailed()` | 设置终态并触发子任务。 |
| `enTaskState GetState()` / `SetState()` | 原子读写状态（acquire/release）。 |

**`Run()` 的跨调度器投递逻辑**（[task.cpp:51-72](file:///e:/workspace/github/myserver/framework/thread/task.cpp#L51-L72)）：
```cpp
if (g_thread_data.own_scheduler == m_pScheduler) {
    // 同调度器：直接执行
    SetState(eTaskDoing); Execute(); OnFinish();
} else {
    // 跨调度器：投递到目标调度器队列
    m_pScheduler->PushTask(GetShared());
}
```
这是**跨线程消息传递**的关键：任务对象可在任意线程被持有，调用 `Run()` 会自动路由到归属线程执行。

#### `CCombineTask<combine_count>`（[task.h:191-293](file:///e:/workspace/github/myserver/framework/thread/task.h#L191-L293)）
- 模板参数 `combine_count` = 父任务数量。
- `CombineTaskDone(pParentTask)`：父任务完成回调，用 `std::atomic_int m_combineDone.fetch_add(1, acq_rel)` 计数：
  - `eCombineAll`：计数 == `combine_count` 时 `Run()`。
  - `eCombineAny`：计数 == 1 时通过 `pParentTask->ExecuteChildTask(this)` 触发。

#### `CWithReturnTask` / `CNoReturnTask`
按返回值类型与参数个数特化的任务模板：

| 特化 | 参数 | 说明 |
|------|------|------|
| `CWithReturnTask<N, Func, Args...>` | 多参数 | 用 `std::tuple<Args...>` 存参，`TaskCaller` 展开。 |
| `CWithReturnTask<N, Func, Par>` | 单参数 | 直接存 `Par m_Param`，可从父任务结果填充。 |
| `CWithReturnTask<N, Func, void>` | 无参数 | 直接调用。 |
| `CNoReturnTask<...>` | 同上三种 | `return_type = void` 版本。 |

#### `TaskCaller` 调用辅助（[task.h:71-113](file:///e:/workspace/github/myserver/framework/thread/task.h#L71-L113)）
- 自实现 `IndexSequence` / `MakeIndexSequence`（C++11 兼容）。
- `invoke(func, tuple)` 通过 index 展开元组调用 `func`。
- 对 `arity=0`、`arity=1` 做特化优化。

---

### 5.4 `CTaskHelper` 与组合 API（task_helper.h）

#### `CTaskHelper<Res>`（[task_helper.h:84-160](file:///e:/workspace/github/myserver/framework/thread/task_helper.h#L84-L160)）
Future 风格的链式句柄，持有 `TaskPtr`。

| 方法 | 说明 |
|------|------|
| `ThenAccept(scheduler, func)` | `Res != void` 版：注册接父任务返回值（`Res`）的回调，返回新的 `CTaskHelper<return_type>`。 |
| `ThenApply(scheduler, func)` | `Res == void` 特化版：父任务无返回值，注册续作回调。 |
| `GetTask()` | 取底层 `TaskPtr`。 |

**关键容错**：添加子任务后，若父任务已完成/失败，会再次调用 `RunChildTask()` 防止子任务丢失。

#### `CAcceptCombineTaskHelper<Args...>`（[task_helper.h:183-255](file:///e:/workspace/github/myserver/framework/thread/task_helper.h#L183-L255)）
收集多个 `TaskHelper`，提供：
- `AcceptAll(scheduler, func)`：所有父任务完成，把各返回值作为参数传入 `func`。
- `AcceptAny(scheduler, func)`：任一父任务完成，`func` 接收其返回值（要求所有父任务返回类型相同，`static_assert are_all_same`）。

#### `CApplyCombineTaskHelper<combine_count>`（[task_helper.h:257-301](file:///e:/workspace/github/myserver/framework/thread/task_helper.h#L257-L301)）
- `ApplyAll(scheduler, func)` / `ApplyAny(scheduler, func)`：与 Accept 系列类似，但 `func` 不接收父任务返回值（`void` 参数）。

#### `IArgsTypeInfo` / `CArgsTypeList<combineIndex, Args...>`（[task_helper.h:303-373](file:///e:/workspace/github/myserver/framework/thread/task_helper.h#L303-L373)）
- 多态基类，用于在运行时把父任务返回值填入子任务的参数元组对应位置。
- `FillWaitTaskParm(pParentTask, pChildTask)`：从父任务 `GetRes()` 取值，写入子任务 `GetCombinedArgsTuple()` 的第 `combineIndex` 位。
- 这是实现 `AcceptAll`（多父任务返回值组装）的类型擦除桥梁。

#### 工厂 `TaskCreater` / `CombineTaskCreater`（[task_helper.h:15-81](file:///e:/workspace/github/myserver/framework/thread/task_helper.h#L15-L81)）
根据 `return_type` 是否为 `void` 选择 `CWithReturnTask` 或 `CNoReturnTask`。

---

### 5.5 `CTaskScheduler`（task_scheduler.h）

任务调度器，**单线程语义**的任务队列 + 模板调度入口。

| 方法 | 说明 |
|------|------|
| `CTaskScheduler(signature)` | 构造，初始化 debug timer（`THREAD_TASK_DEBUG_TIME = 20s`）。 |
| `void ScheduleTask(TaskPtr)` | 校验状态为 `eTaskInit`，置 `eTaskWaitingFoDoing`，入队。 |
| `void PushTask(TaskPtr)` | 加锁入队（供跨线程投递）。 |
| `void ConsumeTask()` | 循环取出并 `pTask->Run()`，直至队列空；每轮调 `DebugTask()`。 |
| `void DebugTask()` | 定时打印队列长度（`CACHE_LOG`）。 |
| `template Schedule(signature, f)` | 便捷模板：创建无参任务并调度，返回 `CTaskHelper<R>`。 |
| `static Schedule(pScheduler, signature, f)` | 静态版本。 |
| `static ApplyCombine(args...)` | 构造 `CApplyCombineTaskHelper`。 |
| `static AcceptAllCombine(tasks...)` / `AcceptAnyCombine(tasks...)` | 构造 `CAcceptCombineTaskHelper`，并通过 `CombineArgs<0, RT...>` 给每个父任务设置 `CArgsTypeList` 参数类型信息。 |

私有模板 `CombineArgs<N, Args...>`（[task_scheduler.h:113-131](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.h#L113-L131)）：递归可变参数展开，为第 N 个父任务 `new CArgsTypeList<N, Args...>` 并 `SetAcceptCombineInfo`。

#### `CTaskThread`（[task_scheduler.h:141-151](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.h#L141-L151) / [task_scheduler.cpp:76-108](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.cpp#L76-L108)）
`CMyThread` 子类，调度器的工作线程：
```cpp
bool PrepareToRun() {
    g_thread_data.own_scheduler = m_pScheduler;  // 绑定归属调度器
    m_funcInit();                                 // 用户 init 回调
    return true;
}
void Run() {
    while (!IsStoped()) {
        CTimeHelper::GetSingletonPtr()->SetTime();  // 刷新缓存时间
        m_funcTick();                                // 用户 tick 回调
        m_pScheduler->ConsumeTask();                 // 消费队列
        SLEEP(1);                                    // 让出 CPU
    }
}
```

---

### 5.6 `CThreadScheduler`（thread_scheduler.h）

`CTaskScheduler` 的多线程扩展：**线程池 + 共享队列**。

| 方法 | 说明 |
|------|------|
| `Init(threads, initFunc, tickFunc, initFuncArgs, tickFuncArgs)` | 创建 `threads` 个 `CTaskThread`，分别设置 init/tick 回调与参数，`CreateThread` 启动，存入 `m_Workers`。 |
| `StopScheduler()` | 对所有 worker 调 `Stop()`。 |
| `Join()` | 对所有 worker 调 `Join()`。 |
| `int ThreadCount()` | 返回 worker 数量。 |
| 析构 | 逐个 `Stop` + `Join` + `Free`。 |

> 注意：所有 worker 共享同一个 `CTaskScheduler` 队列与 `m_queue_mutex`，因此**多个 worker 之间是竞争消费**同一队列，任务由加锁互斥取出。

---

## 6. 核心流程

### 6.1 线程启动与运行循环

```
CreateThread
   └─ ThreadProc
        ├─ SetThreadData(&g_thread_data)   // 绑定 thread_local
        ├─ SetStatus(RUNNING)
        ├─ PrepareToRun()                  // CTaskThread: 设置 own_scheduler + 调 init 回调
        ├─ Run()                           // 主循环: SetTime → tick → ConsumeTask → SLEEP(1)
        ├─ SetStatus(EXITING)
        ├─ PrepareEnd()
        ├─ Exit()
        └─ SetStatus(EXIT)
```

### 6.2 任务调度（同线程 vs 跨线程）

```
用户调用 CTaskScheduler::Schedule(sig, []{...})
   ├─ TaskCreater::CreateTask → TaskPtr (eTaskInit)
   ├─ ScheduleTask → 状态置 eTaskWaitingFoDoing → PushTask 入队
   └─ 返回 CTaskHelper<R>

worker 线程 ConsumeTask
   └─ pTask->Run()
        ├─ if g_thread_data.own_scheduler == m_pScheduler
        │     ├─ SetState(eTaskDoing)
        │     ├─ Execute()       // 真正执行
        │     └─ OnFinish()      // 置 eTaskDone → RunChildTask
        └─ else
              └─ m_pScheduler->PushTask(this)  // 跨线程投递到目标队列
```

### 6.3 链式任务 `ThenAccept`

```
taskA.ThenAccept(schedulerB, [](A_Res){ return B_Res; })
   ├─ 创建 CWithReturnTask<0, Func, A_Res> 作为子任务 B
   ├─ taskA.AddChildTask(B)
   ├─ 若 taskA 已 Done/Failed → 立即 RunChildTask (防丢失)
   └─ 返回 CTaskHelper<B_Res>(B)

taskA 完成后 OnFinish → RunChildTask
   └─ 对子任务 B 调 ExecuteChildTask(B)
        └─ B.ExecuteFromParent(&taskA.m_Res)
              ├─ B.m_Param = *(A_Res*)pRes
              └─ B.Run()  // 若 B 归属 schedulerB 则同步执行，否则投递
```

### 6.4 组合任务 `AcceptAll`

```
CTaskScheduler::AcceptAllCombine(taskA, taskB, taskC)
   ├─ CombineArgs<0, RA,RB,RC>(taskA,taskB,taskC)
   │     ├─ taskA.SetAcceptCombineInfo(new CArgsTypeList<0,RA,RB,RC>)
   │     ├─ taskB.SetAcceptCombineInfo(new CArgsTypeList<1,RA,RB,RC>)
   │     └─ taskC.SetAcceptCombineInfo(new CArgsTypeList<2,RA,RB,RC>)
   └─ 返回 CAcceptCombineTaskHelper<RA,RB,RC>

.AcceptAll(scheduler, [](RA,RB,RC){...})
   ├─ 创建 CCombineTask<3> 子任务 combineTask (eCombineAll)
   ├─ taskA/B/C 各自 AddChildTask(combineTask)
   └─ 返回 CTaskHelper<R>(combineTask)

每个父任务完成 → RunChildTask → combineTask.CombineTaskDone(parent)
   ├─ parent.FillCombineTaskArgs(combineTask)
   │     └─ CArgsTypeList<N>.FillWaitTaskParm: 写入 combineTask 参数元组第 N 位
   ├─ m_combineDone.fetch_add(1)
   └─ if newValue == 3 → combineTask.Run()
```

---

## 7. 依赖关系

### 7.1 模块内依赖

```
thread_scheduler.h ──> task_scheduler.h ──> task.h ──> my_thread.h ──> my_lock.h
                         │                     │
                         └──> task_helper.h ───┘  (CTaskScheduler 的模板 API 用 TaskHelper)
                                                  │
spin_lock.h                                      my_thread.h ──> base.h, time_helper.h, safe_pointer.h
```

### 7.2 模块外依赖（framework 内）

| 头文件 | 来源 | 用途 |
|--------|------|------|
| `base.h` | framework/base | `TID`、`CACHE_LINE_ALIGN`、`SAFE_DELETE`、`load_acquire/store_release` 等基础宏与类型。 |
| `platform_def.h` | framework/base | 平台宏 `__LINUX__` / `__WINDOWS__`、`SLEEP`、`pthread`/`HANDLE` 抽象。 |
| `log.h` | framework/base | `DISK_LOG`、`CACHE_LOG`、`THREAD_ERROR`、`THREAD_CACHE` 日志宏。 |
| `time_helper.h` | framework/base | `CTimeHelper` 单例（`GetMSTime`、`SetTime`）、`CMyTimer`、`TimePoint`。 |
| `my_assert.h` | framework/base | `ASSERT_EX` 宏。 |
| `safe_pointer.h` | framework/std | `CSafePtr<T>` 智能指针（侵入式引用计数，非 `shared_ptr`）。 |
| `t_array.h` | framework/std | `TArray` 定长数组模板（部分注释代码用到）。 |

### 7.3 标准库依赖

`<thread>` `<atomic>` `<mutex>` `<functional>` `<memory>` `<queue>` `<vector>` `<tuple>` `<string>` `<type_traits>`（Linux 还用 `<pthread.h>`）。

---

## 8. 并发与同步原语

| 原语 | 实现 | 适用场景 |
|------|------|----------|
| `CMyLock` | Linux `pthread_mutex` / Win `std::mutex` | 一般互斥，如 `CTaskScheduler::m_queue_mutex`、`CTask::m_childTaskLock`。 |
| `CSafeLock` | RAII 包装 `CMyLock` | 作用域自动解锁。 |
| `CMyRWLock` | Linux `pthread_rwlock` / Win 退化 | 读多写少。 |
| `CSafeRLock` / `CSafeWLock` | RAII 包装 | 读写作用域。 |
| `CSpinLock` | `std::atomic_flag` | 临界区极短、不可睡眠场景。 |
| `CSafeSpLock` | RAII 包装 | 自旋作用域。 |
| `CSpinRWLock` | `std::atomic<uint32_t>` 位编码 | 高并发读、短写。 |
| `std::atomic<enTaskState>` | acquire/release | `CTask::m_nState` 跨线程状态可见性。 |
| `std::atomic_int` | `acq_rel` | `CCombineTask::m_combineDone` 组合计数。 |
| `std::atomic_bool` | - | `CMyThread::m_bStoped` 停止标志。 |

**内存序约定**（代码注释，[task.h:139-141](file:///e:/workspace/github/myserver/framework/thread/task.h#L139-L141)）：
- `release` store：保证此前的读写对其他线程可见。
- `acquire` load：保证此后的读写看到最新数据。

---

## 9. 项目运行方式

### 9.1 构建

`thread` 模块不是独立库，而是 `framework` 静态库的一部分（见 [framework/CMakeLists.txt:72-82](file:///e:/workspace/github/myserver/framework/CMakeLists.txt#L72-L82)）。

在仓库根目录执行 CMake：

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release   # Windows
# 或 Linux:
make -j
```

**编译选项**：
- Linux：`-std=c++11 -O2 -fPIC`，定义 `__LINUX__`，链接 `pthread spdlog protobuf lua`。
- Windows：C++11，定义 `__WINDOWS__ _DEBUG_`，MSVC 用 `/MT`(`/MTd`)，链接 `ws2_32 spdlogd protobufd lua`。

### 9.2 使用示例

#### 单调度器（单线程任务队列）

```cpp
#include "task_scheduler.h"

CSafePtr<CTaskScheduler> scheduler = new CTaskScheduler("main");
// 单线程驱动（伪代码：在某个线程里循环调用 ConsumeTask，
// 或用下面的 CThreadScheduler 自动驱动）
```

#### 多线程工作池

```cpp
CSafePtr<CThreadScheduler> pool = new CThreadScheduler("worker-pool");
pool->Init(4,                  // 4 个 worker
           nullptr,            // init 回调（可选）
           nullptr,            // tick 回调（可选）
           nullptr, nullptr);  // 回调参数

// 提交无参任务
auto t1 = CTaskScheduler::Schedule(pool, "task1", []{
    std::cout << "hello from pool\n";
});

// 链式：t1 完成后在 pool 上执行 t2
auto t2 = t1.ThenApply(pool, []{
    std::cout << "after t1\n";
    return 42;
});

// 停止
pool->StopScheduler();
pool->Join();
```

#### 组合任务

```cpp
auto a = CTaskScheduler::Schedule(pool, "a", []{ return 1; });
auto b = CTaskScheduler::Schedule(pool, "b", []{ return 2; });
auto c = CTaskScheduler::Schedule(pool, "c", []{ return 3; });

// 全部完成
auto all = CTaskScheduler::AcceptAllCombine(a, b, c)
    .AcceptAll(pool, [](int ra, int rb, int rc){
        std::cout << "sum=" << ra + rb + rc << "\n";
    });

// 任一完成
auto any = CTaskScheduler::AcceptAnyCombine(a, b, c)
    .AcceptAny(pool, [](int r){
        std::cout << "first=" << r << "\n";
    });
```

### 9.3 运行时注意

- `CTaskScheduler::DebugTask()` 每 20 秒打印一次队列长度（`THREAD_TASK_DEBUG_TIME`）。
- worker 每轮 `SLEEP(1)` 毫秒让出 CPU，**非高吞吐设计**。
- `CTask` 析构时会再次 `RunChildTask()`，确保已 Done/Failed 任务的子任务被触发。

---

## 10. 设计要点与注意事项

### 10.1 设计亮点

1. **跨平台抽象**：`#if defined(__LINUX__)` 分支覆盖 `pthread`/`Win32`，业务代码无感。
2. **`thread_local own_scheduler` 路由**：`CTask::Run()` 自动判断同/跨线程，统一了"同步执行"与"异步投递"的接口。
3. **Future 风格链式 API**：`ThenAccept` / `ThenApply` / `AcceptAll` / `AcceptAny` / `ApplyAll` / `ApplyAny`，语义接近 `std::future` / Java `CompletableFuture`。
4. **类型擦除的组合参数填充**：`IArgsTypeList` + 模板 `CArgsTypeList<Index, Args...>` 实现多父任务返回值按位置注入子任务元组。
5. **自实现 `IndexSequence`**：兼容旧编译器，不依赖 C++14 `std::index_sequence`。
6. **自旋读写锁位编码**：单原子变量同时管理读计数与写标志，`CACHE_LINE_ALIGN` 防 false sharing。

### 10.2 潜在坑点

- [task.h:220-236](file:///e:/workspace/github/myserver/framework/thread/task.h#L220-L236) `CCombineTask::SetCombineTask` 整段被注释，组合任务的"父任务列表"实际未保存，仅靠 `AddChildTask` + `CombineTaskDone` 计数驱动，业务侧不能通过 `SetCombineTask` 反查父任务。
- [task.h:352-355](file:///e:/workspace/github/myserver/framework/thread/task.h#L352-L355) 多参版 `CWithReturnTask::ExecuteFromParent` 直接 `ASSERT_EX(false)`，即**多参任务不能作为组合子任务**接收父返回值；只有单参/无参特化可参与组合。
- [task.cpp:14-30](file:///e:/workspace/github/myserver/framework/thread/task.cpp#L14-L30) `CTask` 析构调用 `RunChildTask()`，若子任务在此刻又被其他路径触发，需注意重复执行风险（依赖子任务自身幂等性）。
- [task.cpp:51-72](file:///e:/workspace/github/myserver/framework/thread/task.cpp#L51-L72) `Run()` 捕获 `std::exception` 但用 `catch (std::exception e)`（值捕获，会切片），建议改为 `const std::exception&`。
- [task_scheduler.cpp:97-107](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.cpp#L97-L107) `CTaskThread::Run` 每轮 `SLEEP(1)`，延迟敏感场景需调整或改为条件变量唤醒。
- [my_lock.h:126-130](file:///e:/workspace/github/myserver/framework/thread/my_lock.h#L126-L130) Windows 下 `CMyRWLock` 退化为普通互斥，**无读写分离语义**；Windows 高并发读场景应改用 SRWLock。
- [spin_lock.h:92](file:///e:/workspace/github/myserver/framework/thread/spin_lock.h#L92) `CSpinRWLock::WLock` 等待读者释放为纯自旋，读者长时间持锁会**忙等浪费 CPU**。
- 任务返回值通过 `void* GetRes()` 传递，类型安全依赖调用方正确推导，误用易崩溃。
- `CArgsTypeList` 在 `CombineArgs` 中 `new` 出来，依赖 `CSafePtr` 托管释放（`m_pArgsTypeList.Free()`），需确认无泄漏路径。

### 10.3 扩展建议

- 用 `std::condition_variable` 替换 `SLEEP(1)` 轮询，降低空转延迟与 CPU 占用。
- Windows 读写锁改用 `SRWLOCK`（`AcquireSRWLockExclusive/Shared`）。
- `catch` 改为引用捕获，保留异常类型信息。
- 多参任务支持组合：实现 `CWithReturnTask<N, Func, Args...>::ExecuteFromParent` 按位置填充元组。
- 考虑给 `CTask` 增加取消（cancel）与超时语义。

---

## 附录：关键宏与类型速查

| 宏/类型 | 来源 | 含义 |
|---------|------|------|
| `__LINUX__` / `__WINDOWS__` | platform_def.h | 平台编译宏 |
| `TID` | platform_def.h | 线程 ID 类型（Linux `pthread_t` / Win `DWORD`） |
| `CACHE_LINE_ALIGN` | platform_def.h | 缓存行对齐（防 false sharing） |
| `SLEEP(ms)` | platform_def.h | 跨平台睡眠 |
| `CSafePtr<T>` | safe_pointer.h | 侵入式智能指针 |
| `TaskPtr` | task.h | `std::shared_ptr<CTask>` |
| `WeakTaskPtr` | task.h | `std::weak_ptr<CTask>` |
| `ThreadFuncParam` | my_thread.h | `std::function<void(void*)>` |
| `ThreadFuncParamWrapper` | my_thread.h | 函数+参数包装，可调用 |
| `CACHE_LOG` / `DISK_LOG` | log.h | 日志宏 |
| `ASSERT_EX` | my_assert.h | 断言宏 |
| `THREAD_TASK_DEBUG_TIME` | task_scheduler.h | 调试打印间隔 = 20s |

---

*本文档基于 `framework/thread` 目录源码生成，涵盖 11 个文件，约 1100 行 C++ 代码。*
