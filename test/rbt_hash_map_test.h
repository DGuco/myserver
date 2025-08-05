#ifndef __RBT_HASH_MAP_TEST_H__
#define __RBT_HASH_MAP_TEST_H__

//  Created by 杜国超 on 20/05/20.
//  Copyright ? 2019年 杜国超. All rights reserved.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <map>
#include "rbthash_map.h"
#include "time_helper.h"
 
 
#define VMRSS_LINE 22
 
// 获取当前微秒
time_t GetUSTime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
 
 
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
double GetMemoryUsage() 
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return 0.0;
    return (pmc.WorkingSetSize / (1024.0 * 1024.0)); // 字节转MB
}
#else
#include <hash_map>
    // get specific process physical memeory occupation size by pid (MB)
    inline float GetMemoryUsage()
    {
        int curpid = getpid();
        char file_name[64] = { 0 };
        FILE* fd;
        char line_buff[512] = { 0 };
        sprintf(file_name, "/proc/%d/status", pid);
    
        fd = fopen(file_name, "r");
        if (nullptr == fd)
            return 0;
    
        char name[64];
        int vmrss = 0;
        for (int i = 0; i < VMRSS_LINE - 1; i++)
            fgets(line_buff, sizeof(line_buff), fd);
    
        fgets(line_buff, sizeof(line_buff), fd);
        sscanf(line_buff, "%s %d", name, &vmrss);
        fclose(fd);
    
        // cnvert VmRSS from KB to MB
        return vmrss / 1024.0;
    }
#endif


 
#define TEST_COUNT 5000000
#define MAP_VALUE_SIZE 100      //map value 占用内存大小
#define HASH_CONFLICT_RATE 4   //hash冲突倍率
 
struct MapValue
{
    MapValue(int _a)
    {
        a = _a;
    }
 
    int a;
    char data[MAP_VALUE_SIZE];
} ;
 
int Test_RbtHashMap()
{
    time_t start = 0;
    time_t end = 0;
    unsigned long long res = 0;
 
    float memstart = GetMemoryUsage();
    float memend = 0.0f;
 
    printf("------------------------------------------------------------------------\n");
    my_std::RbtHashMap<int,MapValue,TEST_COUNT>* pMyMap = new my_std::RbtHashMap<int,MapValue,TEST_COUNT>();
    memend = GetMemoryUsage();
    start = GetUSTime();
    for(int i = 0;i < TEST_COUNT;i++)
    {
        pMyMap->insert(i * HASH_CONFLICT_RATE,MapValue(i * HASH_CONFLICT_RATE));
    }
    end = GetUSTime();
    printf("my::hash_map<int,int,TEST_COUNT> insert use  %ld ms\n",(end - start) / 1000);
 
//    my::hash_map<int,MapValue,TEST_COUNT>* pMyMap1 = new my::hash_map<int,MapValue,TEST_COUNT>();
//    memcpy(pMyMap1,pMyMap,sizeof(my::hash_map<int,int,TEST_COUNT>));
    start = GetUSTime();
    for(int i = 0;i < TEST_COUNT;i++)
    {
        res += pMyMap->find(i * HASH_CONFLICT_RATE)->second->a;
    }
    end = GetUSTime();
    printf("my::hash_map<int,int,TEST_COUNT> find use  %ld ms,res = %llu,cap = %lu,mem = %f MB\n",
            (end - start) / 1000,res,pMyMap->cap(),memend - memstart);
 
 
    printf("------------------------------------------------------------------------\n");
    memstart = GetMemoryUsage();
    std::map<int,MapValue> testMap;
    start = GetUSTime();
    for(int i = 0;i < TEST_COUNT;i++)
    {
        testMap.insert(std::make_pair(i * HASH_CONFLICT_RATE,MapValue(i * HASH_CONFLICT_RATE)));
    }
    memend = GetMemoryUsage();
    end = GetUSTime();
    printf("std::map<int,int> insert use  %ld ms\n",(end - start) / 1000);
    start = GetUSTime();
    res = 0;
    for(int i = 0;i < TEST_COUNT;i++)
    {
        res += testMap.find(i * HASH_CONFLICT_RATE)->second.a;
    }
    end = GetUSTime();
    printf("std::map<int,int> find use  %ld ms,res = %llu,cap = %lu,mem = %f MB\n",
            (end - start) / 1000,res,testMap.max_size(),memend - memstart);
 

#ifndef WIN32
    printf("------------------------------------------------------------------------\n");
    memstart = GetMemoryUsage();
    __gnu_cxx::hash_map<int,MapValue> testGnuMap;
    start = GetUSTime();
    for(int i = 0;i < TEST_COUNT;i++)
    {
        testGnuMap.insert(std::make_pair(i * HASH_CONFLICT_RATE,MapValue(i * HASH_CONFLICT_RATE)));
    }
    memend = GetMemoryUsage();
    end = GetUSTime();
    printf("__gnu_cxx::hash_map<int,int> insert use  %ld ms\n",(end - start) / 1000);
    start = GetUSTime();
    res = 0;
    for(int i = 0;i < TEST_COUNT;i++)
    {
        res += testGnuMap.find(i * HASH_CONFLICT_RATE)->second.a;
    }
    end = GetUSTime();
    printf("__gnu_cxx::hash_map<int,int> find use  %ld ms,res = %llu,cap = %lu,mem = %f MB\n",
            (end - start) / 1000,res,testGnuMap.max_size(),memend - memstart);
 #endif
 
    printf("------------------------------------------------------------------------\n");
    memstart = GetMemoryUsage();
    std::unordered_map<int,MapValue> testUnorderMap;
    start = GetUSTime();
    for(int i = 0;i < TEST_COUNT;i++)
    {
        testUnorderMap.insert(std::make_pair(i * HASH_CONFLICT_RATE,MapValue(i * HASH_CONFLICT_RATE)));
    }
    memend = GetMemoryUsage();
    end = GetUSTime();
    printf("std::unordered_map<int,int> insert use  %ld ms\n",(end - start) / 1000);
    start = GetUSTime();
    res = 0;
    for(int i = 0;i < TEST_COUNT;i++)
    {
        res += testUnorderMap.find(i * HASH_CONFLICT_RATE)->second.a;
    }
    end = GetUSTime();
    printf("std::unordered_map<int,int> find use  %ld ms,res = %llu,cap = %lu,mem = %f MB\n",
            (end - start) / 1000,res,testUnorderMap.max_size(),memend - memstart);
    printf("------------------------------------------------------------------------\n");
 
    return 0;
}

#endif