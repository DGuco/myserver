/*****************************************************************
* FileName:platform_def.h
* Summary :
* Date	  :2023-7-28
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#ifdef __LINUX__
    //дз╢Ффауо
    #define __MEM_BARRIER \
        __asm__ __volatile__("mfence":::"memory")
    //дз╢Ф╤афауо
    #define __READ_BARRIER__ \
        __asm__ __volatile__("lfence":::"memory")
    //дз╢Фп╢фауо
    #define __WRITE_BARRIER__ \
        __asm__ __volatile__("sfence":::"memory")
#else
#include <windows.h>
    //дз╢Ффауо
    #define __MEM_BARRIER MemoryFence 
    //дз╢Ф╤афауо
    #define __READ_BARRIER__ LoadFence
    //дз╢Фп╢фауо
    #define __WRITE_BARRIER__ StoreFence
#endif

#endif //__PLATFORM_DEF_H__
