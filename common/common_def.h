/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define PIPE_SIZE					(0x1000000)    // 共享内存管道大小 16M
#define C2S_SHM_KEY					(20000000)
#define S2C_SHM_KEY					(20000001)
#define  GAMEPLAYER_RECV_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#define  GAMEPLAYER_SEND_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#endif //__COMMON_DEF_H__
