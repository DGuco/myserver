# MyServer 游戏服务器 Code Wiki

## 目录
- [1. 项目概述](#1-项目概述)
- [2. 整体架构](#2-整体架构)
- [3. 目录结构](#3-目录结构)
- [4. 核心模块详解](#4-核心模块详解)
- [5. 关键类与函数说明](#5-关键类与函数说明)
- [6. 依赖关系](#6-依赖关系)
- [7. 消息协议](#7-消息协议)
- [8. 配置说明](#8-配置说明)
- [9. 项目构建与运行](#9-项目构建与运行)
- [10. 设计特点与技术亮点](#10-设计特点与技术亮点)

---

## 1. 项目概述

### 1.1 项目简介
MyServer 是一个基于 C++11 开发的分布式游戏服务器框架，利用 libevent 和共享内存技术实现高性能的游戏服务端。项目采用多进程架构，包含代理服务器、游戏服务器、数据库服务器等多个独立进程，通过共享内存进行高效的进程间通信。

### 1.2 技术栈
- **开发语言**: C++11
- **网络库**: 自研TCP网络库（底层支持select/epoll）
- **序列化**: Google Protocol Buffers 3.20.0
- **数据库**: MySQL (mysql-connector-c++ 8.4.0)
- **脚本语言**: Lua 5.3.6
- **日志库**: spdlog
- **JSON解析**: RapidJSON
- **加密库**: OpenSSL
- **构建工具**: CMake 3.6+
- **进程间通信**: System V 共享内存

### 1.3 设计目标
- 高性能：基于事件驱动和无锁共享内存队列
- 高可用：多进程架构，单一进程故障不影响整体
- 可扩展：模块化设计，易于新增功能模块
- 跨平台：支持 Linux 和 Windows 双平台

---

## 2. 整体架构

### 2.1 架构图
```
                    +-------------------+
                    |   客户端连接      |
                    +---------+---------+
                              |
                              v
                    +---------+---------+
                    |   ProxyServer     |  <-- 代理/网关服务器
                    |   (端口:9104)    |      消息转发、连接管理
                    +---------+---------+
                              |
              +---------------+---------------+
              |                               |
              v                               v
    +---------+---------+           +---------+---------+
    |   GameServer      |           |   DBServer        |
    |   (端口:9102)     |           |   (端口:9103)     |
    |  游戏逻辑处理      |           |  数据库操作        |
    +---------+---------+           +---------+---------+
              |                               |
              +------- 共享内存队列 ----------+
              (DB2S_SHM / S2DB_SHM)
```

### 2.2 进程组成

| 进程名称 | 服务器类型 | 默认端口 | 主要职责 |
|---------|-----------|---------|---------|
| proxyserver | FE_PROXYSERVER (3) | 9104 | 网关代理，消息转发，连接管理，负载均衡 |
| gameserver | FE_GAMESERVER (1) | 9102 | 游戏逻辑处理，玩家管理，业务模块 |
| dbserver | FE_DBSERVER (2) | 9103 | 数据库读写，数据持久化，缓存管理 |

### 2.3 通信方式
1. **客户端 <-> ProxyServer**: TCP 长连接
2. **ProxyServer <-> GameServer**: 通过 ProxyServer 转发
3. **GameServer <-> DBServer**: 共享内存无锁队列（高性能）
4. **各服务器注册**: 均向 ProxyServer 注册自己的信息

---

## 3. 目录结构

```
myserver/
├── common/                 # 公共代码
│   ├── db/                 # 数据库相关数据结构
│   ├── message/            # 消息协议(protobuf)
│   │   └── proto/          # proto源文件
│   ├── common_def.h        # 公共定义
│   ├── server_config.h     # 服务器配置
│   └── tcp_def.h           # TCP相关定义
├── config/                 # 配置文件
│   └── serverinfo.json     # 服务器信息配置
├── data/                   # 游戏配置数据(JSON)
├── dbserver/               # 数据库服务器
│   ├── inc/                # 头文件
│   ├── src/                # 源文件
│   └── CMakeLists.txt
├── doc/                    # 文档和依赖库
├── framework/              # 核心框架库
│   ├── base/               # 基础组件
│   ├── db/                 # 数据库封装
│   ├── lua/                # Lua绑定
│   ├── net/                # 网络模块
│   ├── shm/                # 共享内存
│   ├── std/                # 标准库扩展
│   └── thread/             # 线程模块
├── gameserver/             # 游戏服务器
│   ├── gamemodule/         # 游戏业务模块
│   │   ├── datamodule/     # 数据模块
│   │   ├── messagemodule/  # 消息模块
│   │   └── metamodule/     # 元数据模块
│   ├── inc/                # 头文件
│   └── src/                # 源文件
├── lib/                    # 预编译库
│   ├── linux/              # Linux库
│   └── win64/              # Windows库
├── lua-5.3.6/              # Lua源码
├── proxyserver/            # 代理服务器
│   ├── inc/
│   ├── src/
│   └── CMakeLists.txt
├── run/                    # 运行脚本
├── script/                 # Lua脚本
├── test/                   # 测试代码
├── thirdparty/             # 第三方库头文件
├── CMakeLists.txt          # 主构建文件
└── README.md
```

---

## 4. 核心模块详解

### 4.1 Framework 框架库

框架库是整个项目的基石，提供了网络、线程、数据库、日志、共享内存等基础能力。

#### 4.1.1 base 基础模块
**文件位置**: [framework/base/](file:///e:/workspace/github/myserver/framework/base/)

| 文件名 | 功能说明 |
|-------|---------|
| [base.h](file:///e:/workspace/github/myserver/framework/base/base.h) | 基础类型定义、通用宏、原子操作辅助函数 |
| [singleton.h](file:///e:/workspace/github/myserver/framework/base/singleton.h) | 单例模式模板（C++11静态局部变量保证线程安全） |
| [log.h](file:///e:/workspace/github/myserver/framework/base/log.h) | 日志系统封装（基于spdlog） |
| [message_factory.h](file:///e:/workspace/github/myserver/framework/base/message_factory.h) | 消息工厂基类，protobuf消息处理 |
| [time_helper.h](file:///e:/workspace/github/myserver/framework/base/time_helper.h) | 时间工具类 |
| [signal_handler.h](file:///e:/workspace/github/myserver/framework/base/signal_handler.h) | 信号处理 |
| [reflection.h](file:///e:/workspace/github/myserver/framework/base/reflection.h) | 反射机制 |
| [json_reader.h](file:///e:/workspace/github/myserver/framework/base/json_reader.h) | JSON配置读取 |
| [performance.h](file:///e:/workspace/github/myserver/framework/base/performance.h) | 性能统计 |

#### 4.1.2 net 网络模块
**文件位置**: [framework/net/](file:///e:/workspace/github/myserver/framework/net/)

| 文件名 | 功能说明 |
|-------|---------|
| [tcp_server.h](file:///e:/workspace/github/myserver/framework/net/tcp_server.h) | TCP服务器基类，支持select/epoll两种模式 |
| [tcp_conn.h](file:///e:/workspace/github/myserver/framework/net/tcp_conn.h) | TCP连接基类（入站连接） |
| [tcp_client.h](file:///e:/workspace/github/myserver/framework/net/tcp_client.h) | TCP客户端基类（出站连接） |
| [tcp_socket.h](file:///e:/workspace/github/myserver/framework/net/tcp_socket.h) | Socket封装 |
| [byte_buff.h](file:///e:/workspace/github/myserver/framework/net/byte_buff.h) | 字节缓冲区 |
| [ccrypto.h](file:///e:/workspace/github/myserver/framework/net/ccrypto.h) | 加密解密 |
| [statistics.h](file:///e:/workspace/github/myserver/framework/net/statistics.h) | 网络统计 |
| [runflag.h](file:///e:/workspace/github/myserver/framework/net/runflag.h) | 运行标志位 |

**关键设计**:
- 支持 select（Windows/Linux）和 epoll（Linux）两种IO多路复用
- 采用事件驱动模型，非阻塞IO
- 连接池管理，自动超时检测
- 动态缓冲区管理

#### 4.1.3 shm 共享内存模块
**文件位置**: [framework/shm/](file:///e:/workspace/github/myserver/framework/shm/)

| 文件名 | 功能说明 |
|-------|---------|
| [shm.h](file:///e:/workspace/github/myserver/framework/shm/shm.h) | 共享内存基础封装 |
| [shm_queue.h](file:///e:/workspace/github/myserver/framework/shm/shm_queue.h) | 无锁共享内存消息队列 |
| [shm_pool.h](file:///e:/workspace/github/myserver/framework/shm/shm_pool.h) | 共享内存对象池 |
| [shm_api.h](file:///e:/workspace/github/myserver/framework/shm/shm_api.h) | 共享内存API封装 |

**核心技术 - 无锁队列设计**:
```
stMemTrunk 结构体（缓存行对齐）:
- m_iBegin: 读指针（读线程修改）
- m_iEnd:   写指针（写线程修改）
- m_iSize:  队列大小（2的幂）
- 各字段间用64字节填充，避免false sharing
```

**特点**:
- 单读单写无锁队列，利用CPU缓存行对齐
- 环形缓冲区设计，size为2的幂用于快速取模
- 适用于GameServer与DBServer之间的高频通信

#### 4.1.4 thread 线程模块
**文件位置**: [framework/thread/](file:///e:/workspace/github/myserver/framework/thread/)

| 文件名 | 功能说明 |
|-------|---------|
| [thread_scheduler.h](file:///e:/workspace/github/myserver/framework/thread/thread_scheduler.h) | 线程调度器，管理工作线程池 |
| [task_scheduler.h](file:///e:/workspace/github/myserver/framework/thread/task_scheduler.h) | 任务调度器 |
| [my_thread.h](file:///e:/workspace/github/myserver/framework/thread/my_thread.h) | 线程封装 |
| [task.h](file:///e:/workspace/github/myserver/framework/thread/task.h) | 任务基类 |
| [my_lock.h](file:///e:/workspace/github/myserver/framework/thread/my_lock.h) | 互斥锁封装 |
| [spin_lock.h](file:///e:/workspace/github/myserver/framework/thread/spin_lock.h) | 自旋锁 |

**线程模型**:
- 支持init函数和tick函数
- 每个线程可独立执行初始化和主循环逻辑
- 任务队列支持异步任务分发

#### 4.1.5 db 数据库模块
**文件位置**: [framework/db/](file:///e:/workspace/github/myserver/framework/db/)

| 文件名 | 功能说明 |
|-------|---------|
| [database.h](file:///e:/workspace/github/myserver/framework/db/database.h) | 数据库抽象接口 |
| [database_mysql.h](file:///e:/workspace/github/myserver/framework/db/database_mysql.h) | MySQL数据库实现 |
| [field.h](file:///e:/workspace/github/myserver/framework/db/field.h) | 字段封装 |
| [query_result.h](file:///e:/workspace/github/myserver/framework/db/query_result.h) | 查询结果抽象 |
| [query_result_mysql.h](file:///e:/workspace/github/myserver/framework/db/query_result_mysql.h) | MySQL查询结果 |

**设计特点**:
- 抽象接口层，便于切换数据库
- 支持格式化SQL（PQuery/PExecute）
- 支持事务操作
- 线程安全的连接管理

#### 4.1.6 lua Lua绑定模块
**文件位置**: [framework/lua/](file:///e:/workspace/github/myserver/framework/lua/)

| 文件名 | 功能说明 |
|-------|---------|
| [lua_bridge.h](file:///e:/workspace/github/myserver/framework/lua/lua_bridge.h) | Lua桥接主类 |
| [lua_vm.h](file:///e:/workspace/github/myserver/framework/lua/core/lua_vm.h) | Lua虚拟机封装 |
| [lua_class.h](file:///e:/workspace/github/myserver/framework/lua/core/lua_class.h) | C++类注册到Lua |
| [lua_stack.h](file:///e:/workspace/github/myserver/framework/lua/core/lua_stack.h) | Lua栈操作 |
| [user_data.h](file:///e:/workspace/github/myserver/framework/lua/core/user_data.h) | 用户数据封装 |

**主要功能**:
- C++函数/类注册到Lua
- Lua函数调用（支持可变参数模板）
- 命名空间支持
- shared_ptr 对象管理
- 普通对象和共享对象两种注册模式

#### 4.1.7 std 标准库扩展
**文件位置**: [framework/std/](file:///e:/workspace/github/myserver/framework/std/)

| 文件名 | 功能说明 |
|-------|---------|
| [t_array.h](file:///e:/workspace/github/myserver/framework/std/t_array.h) | 动态数组 |
| [rb_tree.h](file:///e:/workspace/github/myserver/framework/std/rb_tree.h) | 红黑树 |
| [rbthash_map.h](file:///e:/workspace/github/myserver/framework/std/rbthash_map.h) | 红黑树哈希map |
| [hash_function.h](file:///e:/workspace/github/myserver/framework/std/hash_function.h) | 哈希函数 |
| [safe_pointer.h](file:///e:/workspace/github/myserver/framework/std/safe_pointer.h) | 智能指针封装 |
| [c_string.h](file:///e:/workspace/github/myserver/framework/std/c_string.h) | 字符串封装 |
| [lock_free_limit_queue.h](file:///e:/workspace/github/myserver/framework/std/lock_free_limit_queue.h) | 无锁限长队列 |
| [my_new.h](file:///e:/workspace/github/myserver/framework/std/my_new.h) | 内存分配 |

### 4.2 ProxyServer 代理服务器

**文件位置**: [proxyserver/](file:///e:/workspace/github/myserver/proxyserver/)

#### 4.2.1 核心类

| 类名 | 头文件 | 功能说明 |
|-----|-------|---------|
| CProxyCtrl | [proxy_ctrl.h](file:///e:/workspace/github/myserver/proxyserver/inc/proxy_ctrl.h) | 代理服务器控制器，单例，管理整体生命周期 |
| CProxyServer | [proxy_server.h](file:///e:/workspace/github/myserver/proxyserver/inc/proxy_server.h) | 代理TCP服务器，继承CTCPServer |
| CProxyConn | [proxy_conn.h](file:///e:/workspace/github/myserver/proxyserver/inc/proxy_conn.h) | 代理连接，处理服务器间连接 |
| CProxyTransfer | [proxy_transfer.h](file:///e:/workspace/github/myserver/proxyserver/inc/proxy_transfer.h) | 消息转发器 |
| CMessageFactoryManager | [mfactory_manager.h](file:///e:/workspace/github/myserver/proxyserver/inc/mfactory_manager.h) | 消息工厂管理 |

#### 4.2.2 主要职责
1. **网关接入**: 接收所有客户端连接
2. **消息转发**: 根据消息目标服务器类型进行转发
3. **服务器注册**: 管理各游戏/数据库服务器的注册信息
4. **心跳检测**: 检测各服务器和连接的存活状态
5. **负载均衡**: 多Transfer线程处理消息转发

#### 4.2.3 线程模型
- 1个TCP管理线程（接受连接）
- 4个Transfer线程（消息转发处理）
- 使用轮询方式选择Transfer线程

### 4.3 GameServer 游戏服务器

**文件位置**: [gameserver/](file:///e:/workspace/github/myserver/gameserver/)

#### 4.3.1 核心类

| 类名 | 头文件 | 功能说明 |
|-----|-------|---------|
| CGameCtrl | [game_ctrl.h](file:///e:/workspace/github/myserver/gameserver/inc/game_ctrl.h) | 游戏服务器控制器，单例 |
| CGameServer | [game_server.h](file:///e:/workspace/github/myserver/gameserver/inc/game_server.h) | 游戏TCP服务器 |
| CGamePlayer | [game_player.h](file:///e:/workspace/github/myserver/gameserver/inc/game_player.h) | 玩家连接对象 |
| CServerClient | [server_client.h](file:///e:/workspace/github/myserver/gameserver/inc/server_client.h) | 服务器间客户端连接 |
| CMessageFactoryManager | [mfactory_manager.h](file:///e:/workspace/github/myserver/gameserver/inc/mfactory_manager.h) | 消息工厂管理 |

#### 4.3.2 游戏模块

| 模块 | 目录 | 功能说明 |
|-----|------|---------|
| datamodule | [gamemodule/datamodule/](file:///e:/workspace/github/myserver/gameserver/gamemodule/datamodule/) | 玩家数据管理 |
| messagemodule | [gamemodule/messagemodule/](file:///e:/workspace/github/myserver/gameserver/gamemodule/messagemodule/) | 消息处理 |
| metamodule | [gamemodule/metamodule/](file:///e:/workspace/github/myserver/gameserver/gamemodule/metamodule/) | 配置数据管理 |

#### 4.3.3 主要职责
1. **游戏逻辑处理**: 处理玩家的各种游戏操作
2. **玩家管理**: 玩家在线状态、数据缓存
3. **业务模块**: 各类游戏玩法逻辑
4. **数据交互**: 通过共享内存与DBServer交互

#### 4.3.4 线程模型
- 主线程/逻辑线程：游戏主循环
- DB线程：处理数据库相关操作
- 支持多线程调度器扩展

### 4.4 DBServer 数据库服务器

**文件位置**: [dbserver/](file:///e:/workspace/github/myserver/dbserver/)

#### 4.4.1 核心类

| 类名 | 头文件 | 功能说明 |
|-----|-------|---------|
| CDBCtrl | [db_ctrl.h](file:///e:/workspace/github/myserver/dbserver/inc/db_ctrl.h) | 数据库服务器控制器，单例 |
| CDBServer | [db_server.h](file:///e:/workspace/github/myserver/dbserver/inc/db_server.h) | 数据库TCP服务器 |
| DBClient | [db_client.h](file:///e:/workspace/github/myserver/dbserver/inc/db_client.h) | 数据库客户端连接 |
| CShmManager | [shm_manager.h](file:///e:/workspace/github/myserver/dbserver/inc/shm_manager.h) | 共享内存管理 |

#### 4.4.2 主要职责
1. **数据持久化**: 玩家数据的数据库读写
2. **缓存管理**: 共享内存数据缓存
3. **SQL执行**: 异步数据库操作
4. **数据备份**: 定期数据保存

#### 4.4.3 线程模型
- **SaveHuman线程** (1个): 玩家数据保存
- **SaveGlobal线程** (1个): 全局数据保存
- **Query线程** (4个): 数据库查询
- **Logic线程**: 主逻辑线程

---

## 5. 关键类与函数说明

### 5.1 CSingleton 单例模板

**文件**: [framework/base/singleton.h](file:///e:/workspace/github/myserver/framework/base/singleton.h)

```cpp
template<typename T>
class CSingleton
{
public:
    static CSafePtr<T> GetSingletonPtr();
private:
    CSingleton(const CSingleton &temp) = delete;
    CSingleton &operator=(const CSingleton &temp) = delete;
};
```

**特点**:
- 使用C++11静态局部变量，保证线程安全初始化
- 禁止拷贝和赋值
- 返回CSafePtr智能指针

**使用示例**:
```cpp
// 获取单例
auto pCtrl = CGameCtrl::GetSingletonPtr();
```

### 5.2 CTCPServer TCP服务器基类

**文件**: [framework/net/tcp_server.h](file:///e:/workspace/github/myserver/framework/net/tcp_server.h)

#### 关键成员函数

| 函数 | 说明 |
|-----|------|
| `InitTcpServer()` | 初始化TCP服务器，选择IO模式 |
| `TcpTick(time_t now)` | 主循环tick，处理网络事件 |
| `OnNewConnect()` | 纯虚函数，新连接回调，子类实现 |
| `CreateTcpConn()` | 纯虚函数，创建连接对象，子类实现 |
| `FindTcpConn(SOCKET)` | 查找连接对象 |
| `ConnectTo()` | 作为客户端连接其他服务器 |

#### 关键数据成员
- `m_ConnMap`: socket到连接对象的映射
- `m_ClientMap`: 出站客户端连接映射
- `m_ListenSocket`: 监听socket

### 5.3 CShmMessQueue 共享内存消息队列

**文件**: [framework/shm/shm_queue.h](file:///e:/workspace/github/myserver/framework/shm/shm_queue.h)

#### 核心函数

| 函数 | 说明 |
|-----|------|
| `Init(shmKey, size)` | 初始化共享内存队列 |
| `SendMessage(BYTE*, length)` | 发送消息（写线程调用） |
| `GetMessage(BYTE*)` | 获取消息（读线程调用） |
| `ReadHeadMessage(BYTE*)` | 偷看队头消息（不移除） |
| `DeleteHeadMessage()` | 删除队头消息 |
| `GetFreeSize()` | 获取空闲空间大小 |
| `GetDataSize()` | 获取已用数据大小 |
| `IsEmpty()` | 判断是否为空 |

#### 设计原理
- 环形缓冲区，大小为2的幂
- 单生产者单消费者无锁设计
- 利用CPU缓存行对齐避免false sharing
- m_iBegin和m_iEnd分别由读、写线程独立修改

### 5.4 CThreadScheduler 线程调度器

**文件**: [framework/thread/thread_scheduler.h](file:///e:/workspace/github/myserver/framework/thread/thread_scheduler.h)

#### 核心函数

| 函数 | 说明 |
|-----|------|
| `Init(threads, initFunc, tickFunc, ...)` | 初始化线程池 |
| `StopScheduler()` | 停止调度器 |
| `Join()` | 等待所有线程结束 |
| `ThreadCount()` | 获取线程数 |

#### 使用模式
```cpp
// 创建调度器
CSafePtr<CThreadScheduler> pScheduler = new CThreadScheduler("logic");
// 初始化：指定线程数、初始化函数、tick函数
pScheduler->Init(4, InitFunc, TickFunc, initArgs, tickArgs);
```

### 5.5 CMessageFactory 消息工厂

**文件**: [framework/base/message_factory.h](file:///e:/workspace/github/myserver/framework/base/message_factory.h)

```cpp
class CMessageFactory
{
public:
    virtual shared_ptr<ProtoMess> CreateMessage() = 0;
    virtual int Execute(CSafePtr<CTCPSocket> pSocket) = 0;
    virtual int MessId() = 0;
};
```

**设计模式**: 工厂方法模式
- 每个消息ID对应一个工厂类
- CreateMessage: 创建对应的protobuf消息对象
- Execute: 执行消息处理逻辑
- MessId: 返回消息ID

### 5.6 LuaBridge Lua桥接

**文件**: [framework/lua/lua_bridge.h](file:///e:/workspace/github/myserver/framework/lua/lua_bridge.h)

#### 核心函数

| 函数 | 说明 |
|-----|------|
| `LoadFile(filePath)` | 加载Lua脚本文件 |
| `CallLuaFunc<R>(func, args...)` | 调用Lua函数（模板方式） |
| `Call(func, sig, ...)` | 调用Lua函数（签名方式） |
| `BeginNameSpace(name)` | 开始命名空间 |
| `GetGlobalNamespace()` | 获取全局命名空间 |
| `PushSharedObjToLua(L, ptr)` | 将shared_ptr对象推入Lua |

#### 注册宏
```cpp
// 注册C函数
BEGIN_REGISTER_CFUNC(luaBridge)
    REGISTER_CFUNC("Add", Add)
END_REGISTER_CFUNC

// 注册类
BEGIN_CLASS(luaBridge, MyClass)
    CLASS_ADD_CONSTRUCTOR(void(*)(int))
    CLASS_ADD_FUNC("Say", &MyClass::Say)
END_CLASS

// 注册命名空间
BEGIN_NAMESPACE(luaBridge, "space")
    ...
END_NAMESPACE
```

### 5.7 CLog 日志系统

**文件**: [framework/base/log.h](file:///e:/workspace/github/myserver/framework/base/log.h)

#### 使用宏
```cpp
// 初始化日志
INIT_LOG("gameserver");

// 写日志
DISK_LOG(ERROR_DISK, "error info, code = {}", code);
CACHE_LOG(INFO_DISK, "player login, id = {}", playerId);

// 关闭日志
SHUTDOWN_ALL_LOG();
```

#### 特点
- 基于spdlog，高性能
- 支持多种日志类型（磁盘日志、控制台日志）
- 支持按小时、按天、滚动日志
- 异步日志支持

### 5.8 CServerConfig 服务器配置

**文件**: [common/server_config.h](file:///e:/workspace/github/myserver/common/server_config.h)

#### 主要配置项

| 配置项 | 说明 |
|-------|------|
| 服务器信息 | 各类型服务器的IP、端口、ID |
| MySQL信息 | 数据库连接串 |
| TCP参数 | 心跳间隔、超时时间 |
| 服务器tick | 主循环间隔 |

---

## 6. 依赖关系

### 6.1 模块依赖图

```
                    +-------------------+
                    |   thirdparty/     |
                    |  (protobuf,mysql, |
                    |   rapidjson,ssl)  |
                    +---------+---------+
                              |
                              v
                    +---------+---------+
                    |   framework/      |
                    |  (基础框架库)      |
                    +---------+---------+
                              |
              +---------------+---------------+
              |               |               |
              v               v               v
     +--------+------+ +------+-------+ +-----+--------+
     |  common/      | | gameserver/  | |  dbserver/   |
     |  (公共代码)    | |  (游戏逻辑)  | |  (数据库服务) |
     +--------+------+ +------+-------+ +-----+--------+
              |               |               |
              +---------------+---------------+
                              |
                              v
                    +---------+---------+
                    |   proxyserver/    |
                    |   (代理服务器)     |
                    +-------------------+
```

### 6.2 服务器进程依赖

| 进程 | 依赖的库 | 依赖的其他进程 |
|-----|---------|--------------|
| proxyserver | framework, common, protobuf, spdlog | 无（最先启动） |
| gameserver | framework, common, protobuf, spdlog, lua, mysql | proxyserver, dbserver |
| dbserver | framework, common, protobuf, spdlog, mysql | proxyserver |

### 6.3 第三方依赖

| 库名 | 版本 | 用途 | 位置 |
|-----|------|------|------|
| protobuf | 3.20.0 | 消息序列化 | thirdparty/google/protobuf/ |
| mysql-connector-c++ | 8.4.0 | MySQL数据库连接 | thirdparty/mysql/ |
| rapidjson | master | JSON解析 | thirdparty/rapidjson/ |
| spdlog | 1.x | 日志库 | lib/win64/spdlogd.lib |
| openssl | - | 加密 | thirdparty/openssl/ |
| lua | 5.3.6 | 脚本语言 | lua-5.3.6/ |
| libevent | 2.0.22 | 网络事件 | lib/win64/libevent.lib |

---

## 7. 消息协议

### 7.1 消息定义文件

**位置**: [common/message/proto/](file:///e:/workspace/github/myserver/common/message/proto/)

| 文件 | 说明 |
|-----|------|
| [message.proto](file:///e:/workspace/github/myserver/common/message/proto/message.proto) | 服务器间消息、代理消息 |
| [common.proto](file:///e:/workspace/github/myserver/common/message/proto/common.proto) | 公共消息定义 |
| [player.proto](file:///e:/workspace/github/myserver/common/message/proto/player.proto) | 玩家相关消息 |
| [dbmessage.proto](file:///e:/workspace/github/myserver/common/message/proto/dbmessage.proto) | 数据库消息 |

### 7.2 服务器类型枚举
```cpp
enum enServerType {
    FE_INVALID       = 0;  // 无效
    FE_GAMESERVER    = 1;  // Game服务器
    FE_DBSERVER      = 2;  // DB服务器
    FE_PROXYSERVER   = 3;  // 代理服务器
    FE_WEBSERVER     = 4;  // http服务器
};
```

### 7.3 核心消息结构

#### 7.3.1 ProxyMessage 代理消息
用于服务器间通过ProxyServer转发的消息包装。

```protobuf
message ProxyMessage {
    required uint32         srcFE       = 1;  // 源服务器类型
    required uint32         srcID       = 2;  // 源服务器ID
    required uint32         dstFE       = 3;  // 目标服务器类型
    required uint32         dstID       = 4;  // 目标服务器ID
    required uint32         packetid    = 5;  // 转发的消息号
    required uint64         timeStamp   = 6;  // 时间戳(毫秒)
    required enMessageCmd   cmd         = 7;  // 指令类型
    optional bytes          buff        = 8;  // 字节流
};
```

#### 7.3.2 CGonnReq 连接请求
客户端连接请求。

```protobuf
message CGonnReq {
    required string Account   = 1;  // 账号(uid)
    optional string Password  = 2;  // 密码
    optional int32  PFrom     = 3;  // 平台ID
    required string Session   = 4;  // 平台session
};
```

#### 7.3.3 HeartBeatMsg 心跳消息
```protobuf
message HeartBeatMsg {
    required enHeartBeatType beatType = 1;  // 心跳类型
};

enum enHeartBeatType {
    MESS_HEARTBEAT          = 0;  // 心跳请求
    MESS_HEARTBEAT_CALLBACK = 1;  // 心跳回复
};
```

### 7.4 消息指令类型
```cpp
enum enMessageCmd {
    MESS_LOGIC      = 0;  // 逻辑消息
    MESS_REGISTER   = 1;  // 服务器注册
    MESS_KEEPALIVE  = 2;  // 心跳
};
```

---

## 8. 配置说明

### 8.1 主配置文件

**文件**: [config/serverinfo.json](file:///e:/workspace/github/myserver/config/serverinfo.json)

```json
{
  "gateinfo": {
    "host": "127.0.0.1",
    "port": 9101,
    "serverid": 20001
  },
  "gameinfo": {
    "host": "127.0.0.1",
    "port": 9102,
    "serverid": 20002
  },
  "dbinfo": {
    "host": "127.0.0.1",
    "port": 9103,
    "serverid": 20003
  },
  "proxyinfo": {
    "host": "127.0.0.1",
    "port": 9104,
    "serverid": 20004
  },
  "keepalive": 60,
  "servertick": 100,
  "checktimeOutGap": 5,
  "socketTimeout": 30,
  "proxySize": 1,
  "mysqlinfo": "127.0.0.1;3306;root;000000;my_game",
  "sleeptime": 0,
  "loop": 1
}
```

### 8.2 配置项说明

| 配置项 | 类型 | 默认值 | 说明 |
|-------|------|--------|------|
| gateinfo.host | string | 127.0.0.1 | 网关服务器IP |
| gateinfo.port | int | 9101 | 网关服务器端口 |
| gateinfo.serverid | int | 20001 | 网关服务器ID |
| gameinfo.* | - | - | 游戏服务器配置 |
| dbinfo.* | - | - | DB服务器配置 |
| proxyinfo.* | - | - | 代理服务器配置 |
| keepalive | int | 60 | 心跳间隔(秒) |
| servertick | int | 100 | 服务器tick间隔(毫秒) |
| checktimeOutGap | int | 5 | 超时检测间隔(秒) |
| socketTimeout | int | 30 | Socket超时时间(秒) |
| proxySize | int | 1 | 代理服务器数量 |
| mysqlinfo | string | - | MySQL连接串: IP;端口;用户;密码;数据库名 |
| sleeptime | int | 0 | 休眠时间 |
| loop | int | 1 | 循环次数 |

### 8.3 游戏配置数据

**位置**: [data/](file:///e:/workspace/github/myserver/data/)

大量JSON格式的游戏配置表，涵盖游戏的各个系统：

| 配置文件 | 系统 |
|---------|------|
| player_exp_config.json | 玩家经验 |
| item_config.json | 道具系统 |
| equipment_config.json | 装备系统 |
| skill_config.json | 技能系统 |
| building_config.json | 建筑系统 |
| army_config.json | 军队系统 |
| monster_config.json | 怪物系统 |
| mission_config.json | 任务系统 |
| achievement_config.json | 成就系统 |
| vip_config.json | VIP系统 |
| shop_config.json | 商城系统 |
| ... 等60+个配置文件 | |

---

## 9. 项目构建与运行

### 9.1 环境要求

#### Linux
- GCC 4.8+ (支持C++11)
- CMake 3.6+
- MySQL 开发库
- OpenSSL 开发库

#### Windows
- Visual Studio 2015+
- CMake 3.6+
- 预编译库已提供在 lib/win64/

### 9.2 编译构建

#### 使用 CMake (推荐)

```bash
# 创建构建目录
mkdir build
cd build

# 生成构建文件
cmake ..

# 编译
make -j4
```

#### Windows 下使用 Visual Studio

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
# 然后在Visual Studio中打开.sln文件编译
```

#### 输出位置
- 可执行文件: `run/` 目录
- 库文件: `lib/` 目录

### 9.3 运行方式

#### 启动顺序
1. **proxyserver** - 代理服务器（最先启动）
2. **dbserver** - 数据库服务器
3. **gameserver** - 游戏服务器

#### 启动脚本
**文件**: [run/start.sh](file:///e:/workspace/github/myserver/run/start.sh)

```bash
#!/bin/sh
# 1. 停止已有服务
sh stop.sh

# 2. 设置环境变量
export TRITON_HOME=/home/dguco/data/cpppro/myserver

# 3. 清理日志
rm ../log/* -f

# 4. 按顺序启动各服务
cd $TRITON_HOME/bin
./proxyserver
sleep 1

./gateserver  # 注意: 实际应为gameserver
sleep 1

./dbserver
sleep 1

# 5. 设置共享内存ID环境变量
export PIPE_ID=1
export SC_PIPE_ID=2
export CS_PIPE_ID=3

./gameserver
sleep 1

# 6. 查看状态
ipcs -m | grep $user
ps aux | grep $user | grep server | grep -v grep
```

#### 停止脚本
```bash
# 停止所有服务器
sh run/stop.sh

# 强制停止
sh run/kstop.sh

# 关闭并清理共享内存
sh run/shutdown.sh
sh run/rmshm.sh
```

### 9.4 其他运维脚本

| 脚本 | 功能 |
|-----|------|
| build.sh | 编译构建 |
| start.sh | 启动所有服务 |
| stop.sh | 停止所有服务 |
| kstop.sh | 强制停止 |
| shutdown.sh | 关闭服务 |
| rmshm.sh | 移除共享内存 |
| reload.sh | 重新加载配置 |
| monitoring.sh | 监控脚本 |
| pstack.sh | 栈跟踪 |

---

## 10. 设计特点与技术亮点

### 10.1 高性能设计

#### 10.1.1 无锁共享内存队列
- GameServer与DBServer之间使用无锁共享内存队列通信
- 单读单写模式，利用CPU缓存行对齐避免伪共享
- 环形缓冲区设计，size为2的幂实现快速取模

#### 10.1.2 事件驱动网络模型
- 基于IO多路复用（select/epoll）
- 非阻塞socket
- 减少线程切换开销

#### 10.1.3 多线程架构
- 网络IO与业务逻辑分离
- 数据库操作多线程化
- 消息转发多线程并行处理

### 10.2 可扩展性设计

#### 10.2.1 模块化设计
- 框架层与业务层分离
- 消息工厂模式，便于新增消息处理
- 游戏功能模块化，每个模块独立

#### 10.2.2 Lua脚本支持
- 游戏逻辑可使用Lua脚本实现
- 热更新支持（理论上）
- 灵活的C++/Lua互操作

#### 10.2.3 抽象接口
- 数据库抽象层，可切换数据库
- 网络层抽象，支持多种IO模式

### 10.3 可靠性设计

#### 10.3.1 多进程架构
- 各服务器独立进程
- 单一进程崩溃不影响整体
- 可独立重启故障进程

#### 10.3.2 心跳检测
- 服务器间心跳机制
- 连接超时自动断开
- 异常连接自动清理

#### 10.3.3 日志系统
- 基于spdlog的高性能日志
- 多种日志级别和类型
- 便于问题排查

### 10.4 跨平台设计
- Linux 和 Windows 双平台支持
- 平台差异通过宏定义隔离
- 统一的上层接口

### 10.5 内存管理
- 智能指针（CSafePtr）管理对象生命周期
- 共享内存对象池
- 内存池减少碎片

---

## 附录

### A. 共享内存Key定义
- DB2S_SHM_KEY: 20000000 (DB到GameServer)
- S2DB_SHM_KEY: 20000001 (GameServer到DB)

### B. 缓冲区大小定义
- MAX_PACKAGE_LEN: 64KB (最大包长度)
- PIPE_SIZE: 16MB (共享内存管道大小)
- GAMEPLAYER_RECV/SEND_BUFF_LEN: 1MB
- SERVER_CLIENT_RECV/SEND_BUFF: 8MB (最大32MB)

### C. 相关文档
- [doc/install.txt](file:///e:/workspace/github/myserver/doc/install.txt) - 安装说明
- [doc/ddl.sql](file:///e:/workspace/github/myserver/doc/ddl.sql) - 数据库表结构
- [doc/gameserver框架.png](file:///e:/workspace/github/myserver/doc/gameserver框架.png) - 框架图

---

**文档生成时间**: 2026-06-30
**项目版本**: 基于当前仓库版本
**维护者**: DGuco (1139140929@qq.com)
