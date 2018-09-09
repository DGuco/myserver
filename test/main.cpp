#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <zconf.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <unordered_map>
#include <iostream>
#include "test.h"
#include "redis_client.h"
#include <string>
#include "base.h"
#include <strings.h>
#include <future>
#include "redi_lock.h"

//
// Created by dguco on 18-6-18.
//

string key = "test";
string lockKey = "Lock.test";
string key1 = "test1";
CRedisLock redisLock;

int main()
{
//	testDlopen();
//	testSoHotLoad();
//	listenFileChange();
    if (!g_RedisCli.Initialize("127.0.0.1", 6379, 2, 1)) {
        std::cout << "connect to redis failed" << std::endl;
        return -1;
    }
    redisLock.DoWithLock(lockKey, [key]
    {
        string res;
        g_RedisCli.Get(key,&res);
        int a  = atoi(res.c_str());
        a++;
        g_RedisCli.Set(key, to_string(a));
    });
    std::cout << "Over" << std::endl;
};