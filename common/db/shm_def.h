#ifndef _SHM_DEF_H_
#define _SHM_DEF_H_

#ifdef __LINUX__
#define MAX_PLAYER_POOL_SIZE (5000)
#define MAX_PLAYER_CACHE_SIZE (50)
#else
#define MAX_PLAYER_POOL_SIZE (500)
#define MAX_PLAYER_CACHE_SIZE (50)
#endif

enum enShmType
{
	enShmType_Player = 0,
    enShmType_Max,
};

#endif