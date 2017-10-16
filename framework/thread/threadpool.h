//
// Created by DGuco on 17-10-12.
// 基于c++11简单的线程池
//

#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H

#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <bits/unordered_map.h>

using  namespace std;

class CThreadPool{
public:
    CThreadPool();
    CThreadPool(size_t);
    ~CThreadPool();
    //向后添加任务
    template<class F, class... Args>
    auto PushTaskBack(F&& f, Args&&... args);
    //向前添加任务
    template<class F, class... Args>
    auto PushTaskFront(F&& f, Args&&... args);
    bool IsThisThreadIn();
    bool IsThisThreadIn(thread* thrd);

private:
    void ThreadFunc();
private:
    std::unordered_map<std::thread::id,std::thread*> m_mWorkers;
    std::deque< std::function<void()> > m_qTasks;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

#endif //SERVER_THREADPOOL_H
