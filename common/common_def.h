/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define PIPE_SIZE            (0x1000000)    // 共享内存管道大小 16M
#define C2S_SHM_KEY			 (20000000)
#define S2C_SHM_KEY			 (20000001)

// 管道标识符
enum enLockIdx
{
	IDX_PIPELOCK_C2S = 0,
	IDX_PIPELOCK_S2C = 1,
	IDX_PIPELOCK_A2C = 2,
	IDX_PIPELOCK_S2L = 3,
};

#endif //__COMMON_DEF_H__
