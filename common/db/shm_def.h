#ifndef _SHM_DEF_H_
#define _SHM_DEF_H_

#ifdef __LINUX__
#define MAX_PLAYER_POOL_SIZE (5000)
#else
#define MAX_PLAYER_POOL_SIZE (500)
#endif

enum enShmType
{
	enShmType_Player = 0,
    enShmType_Max,
};

#endif