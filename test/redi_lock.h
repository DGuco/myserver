//
// Created by dguco on 18-9-9.
//

#ifndef SERVER_REDI_LOCK_H
#define SERVER_REDI_LOCK_H
#include <iostream>
#include <redis_client.h>
#include <future>

using namespace std;

// 获取当前秒数
int GetSecond()
{
    struct timeval tmval = {0};
    int nRetCode = gettimeofday(&tmval, NULL);
    if (nRetCode != 0) {
        return 0;
    }
    return (int) (tmval.tv_sec);
}

#define LOCK_TIME_OUT 1 //1s

CRedisClient g_RedisCli;

class CRedisLock
{
public:
    /**
     *
     * @param lockKey
     * @param isBlock  司机阻塞
     * @return 是否锁成功
     */
    bool Lock(std::string &lockKey, bool isBlock = false);
    template<class F, class... Args>
    void DoWithLock(std::string &lockKey, F &&f, Args &&... args);
    template<class F, class... Args>
    void TryDoWithLock(std::string &lockKey, F &&f, Args &&... args);
private:
    int m_iLockTimeOut;
};

template<class F, class... Args>
void CRedisLock::TryDoWithLock(std::string &lockKey, F &&f, Args &&... args)
{
    bool isLock = Lock(lockKey, false);
    if (isLock) {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        (*task)();
        int now = GetSecond();
        if (now < m_iLockTimeOut) {
            g_RedisCli.Del(lockKey);
        }
    }
}

template<class F, class... Args>
inline void CRedisLock::DoWithLock(std::string &lockKey, F &&f, Args &&... args)
{
    bool isLock = Lock(lockKey, true);
    if (isLock) {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        (*task)();
        int now = GetSecond();
        if (now < m_iLockTimeOut) {
            g_RedisCli.Del(lockKey);
        }
    }
}

bool CRedisLock::Lock(std::string &lockKey, bool isBlock)
{
    int lock = 0;
    m_iLockTimeOut = 0;
    bool isLock = false;
    while (lock != 1) {
        int now = GetSecond();
        m_iLockTimeOut = now + LOCK_TIME_OUT + 1;
        lock = g_RedisCli.Setnx(lockKey, to_string(m_iLockTimeOut));
        //是否获取成功
        if (lock == 1) {
            isLock = true;
        }
        //判断是否超时，并设置新的超时时间
        if (!isLock) {
            string res;
            g_RedisCli.Get(lockKey, &res);
            int out1 = atoi(res.c_str());
            g_RedisCli.Getset(lockKey, &res);
            int out2 = atoi(res.c_str());
            //如果
            if (out1 == out2) {
                isLock = true;
            }
        }
        if (isLock or !isBlock) {
            break;
        }
        else {
            usleep(1000);
        }
    }
    return isLock;
}

#endif //SERVER_REDI_LOCK_H
