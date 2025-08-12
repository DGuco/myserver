//
//  base.h
//  游戏工具
//  Created by DGuco on 16/12/19.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#ifndef __BASE_H__
#define __BASE_H__

#include <csignal>
#include <stdio.h>
#include <atomic>
#include "platform_def.h"
#include "my_new.h"

#define MIN(a,b)((a) <= (b) ? (a) : (b))
#define MAX(a,b)((a) >= (b) ? (a) : (b))
#define __MY_FILE__ ((strrchr(__FILE__, '/') == NULL) ? __FILE__ : strrchr(__FILE__, '/') + 1)
#define STR(x)			#x

//#define new MY_NEW
#define SAFE_DELETE(ptr) \
	do \
	{\
		if(ptr != NULL)\
		{\
			delete ptr;\
			ptr = NULL;\
		}\
	}\
	while(0);\

#define SAFE_DELETE_ARR(ptr) \
	do \
	{\
		if(ptr != NULL)\
		{\
			delete[] ptr;\
			ptr = NULL;\
		}\
	}\
	while(0);\

typedef unsigned char BYTE;
typedef unsigned char byte;
typedef int int32;
typedef short int16;
typedef char int8;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef False
#define False  0
#endif

#ifndef True
#define True   1
#endif

//对原子变量y进行acquire的load操作，确保在读取原子变量之前所有之前的写操作都已经完成,确保读取操作能够看到最新的数据
template<typename T>
inline T load_acquire(std::atomic<T>& atomic_value) 
{
	return atomic_value.load(std::memory_order_acquire);
}

//对原子变量y进行了release的store操作，确保在写操作之后，所有之后的读操作都能看到这个写操作的结果
template<typename T>
inline void store_release(std::atomic<T>& atomic_value,T value)
{
	atomic_value.store(value,std::memory_order_release);
}

#endif // __BASE_H__
