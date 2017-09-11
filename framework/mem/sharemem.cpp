#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "sharemem.h"
#include "../log/log.h"

/**
  *函数名          : MakeKey
  *功能描述        : 生成共享内存块唯一标识key
  *参数			  : pFile文件路径 vId：
  *返回值         ： 共享内存块唯一标识key
**/
key_t MakeKey( const char* pFile, int vId )
{
	return ftok( pFile, vId );
}

/**
  *函数名          : CreateShareMem
  *功能描述        : 创建共享内存块
  *参数			 :  iKey：共享内存块唯一标识key vSize：大小
  *返回值         ： 共享内存块地址
**/
BYTE* CreateShareMem( key_t iKey, long vSize )
{
	size_t iTempShmSize;
	int iShmID;

	if( iKey < 0 )
	{
		LOG_ERROR("default", "CreateShareMem failed. errno:%s.", strerror(errno));
		exit(-1);
	}

	iTempShmSize = ( size_t ) vSize;
	//iTempShmSize += sizeof(CSharedMem);

	LOG_NOTICE( "default", "Try to malloc share memory of %ld bytes...", iTempShmSize);

	iShmID = shmget( iKey, iTempShmSize, IPC_CREAT|IPC_EXCL|0666 );

	if( iShmID < 0 )
	{
		if( errno != EEXIST )
		{
			LOG_ERROR( "default", "Alloc share memory failed, iKey:%d, size:%lld, error:%s",
					iKey, iTempShmSize, strerror(errno));
			exit(-1);
		}

//		LOG_NOTICE( "default", "Same shm seg (key=%08X) exist, now try to destroy it...", iKey);
//        DestroyShareMem(iKey);
//        iShmID = shmget( iKey, iTempShmSize, IPC_CREAT|IPC_EXCL|0666 );
//        if (iShmID < 0)
//        {
//            LOG_ERROR( "default", "Fatal error, alloc share memory failed, %s", strerror(errno));
//            exit(-1);
//        }
		LOG_NOTICE( "default", "Same shm seg (key=%08X) exist, now try to attach it...", iKey);

		iShmID = shmget( iKey, iTempShmSize, 0666 );
		if( iShmID < 0 )
		{
			LOG_NOTICE( "default", "Attach to share memory %d failed, %s. Now try to touch it", iShmID, strerror(errno));
			iShmID = shmget( iKey, 0, 0666 );
			if( iShmID < 0 )
			{
				LOG_ERROR( "default", "Fatel error, touch to shm failed, %s.", strerror(errno));
				exit(-1);
			}
			else
			{
				LOG_NOTICE( "default", "First remove the exist share memory %d", iShmID);
				if( shmctl(iShmID, IPC_RMID, NULL) )
				{
					LOG_ERROR( "default", "Remove share memory failed, %s", strerror(errno));
					exit(-1);
				}
				iShmID = shmget( iKey, iTempShmSize, IPC_CREAT|IPC_EXCL|0666 );
				if( iShmID < 0 )
				{
					LOG_ERROR( "default", "Fatal error, alloc share memory failed, %s", strerror(errno));
					exit(-1);
				}
			}
		}
		else
		{
			LOG_NOTICE( "default", "Attach to share memory succeed.");
		}
	}

	LOG_NOTICE( "default", "Successfully alloced share memory block, key = %08X, id = %d, size = %ld", iKey, iShmID, iTempShmSize);
	BYTE* tpShm = (BYTE *)shmat(iShmID, NULL, 0);

	if((void*)-1 == tpShm)
	{
		LOG_ERROR("default", "create share memory failed, shmat failed, iShmID = %d, error = %s.",
				iShmID, strerror(errno));
		exit(0);
	}

	return tpShm;
}

/************************************************
  函数名          : DestroyShareMem
  功能描述        : 销毁共享内存块
  参数			 :  iKey：共享内存块唯一标识key
  返回值         ： 成功0 错误：错误码
************************************************/
int DestroyShareMem( key_t iKey )
{
	int iShmID;

	if( iKey < 0 )
	{
		LOG_ERROR( "default", "Error in ftok, %s.", strerror(errno));
		return -1;
	}

	LOG_NOTICE( "default", "Touch to share memory key = 0x%08X...", iKey);

	iShmID = shmget( iKey, 0, 0666 );
	if( iShmID < 0 )
	{
		LOG_ERROR( "default", "Error, touch to shm failed, %s", strerror(errno));
		return -1;
	}
	else
	{
		LOG_NOTICE( "default", "Now remove the exist share memory %d", iShmID);
		if( shmctl(iShmID, IPC_RMID, NULL) )
		{
			LOG_ERROR( "default", "Remove share memory failed, %s", strerror(errno));
			return -1;
		}
		LOG_NOTICE( "default", "Remove shared memory(id = %d, key = 0X%08X) succeed.", iShmID, iKey);
	}

	return 0;
}
