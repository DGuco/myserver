/******************************************************************************
   文件名          : yq_assert.h
  版本号          : 1.0
  作者            : 姚强
  生成日期        : 2013-08-06
  最近修改        : 
  功能描述        : 封装assert, relase模式下也可用
  函数列表        : 
  修改历史        : 
  1 日期          : 2013-08-06
    作者          : 姚强
    修改内容      : 生成
*******************************************************************************/
#ifndef __YQ_ASSERT_H__
#define __YQ_ASSERT_H__

#include <assert.h>
#include "base.h"

#ifdef USE_YQ_ASSERT
#include "log.h"
#define YQ_ASSERT(a, fail_handle) 										\
	if ((a) == false) 													\
	{ 																	\
		LOG_ERROR("default","[%s : %d : %s] ASSERT: (%s) == flase.",	\
				__FILE__,__LINE__,__FUNCTION__,#a);						\
		fail_handle; 													\
	}
	
#define YQ_ASSERT_STR(a, fail_handle, str, ...) 							\
	if((a) == false) 														\
	{ 																		\
		char log_str[ 1024 ]; 												\
		sprintf(log_str, str, ##__VA_ARGS__); 								\
		LOG_ERROR( "default", "[%s : %d : %s] ASSERT: (%s) == false, %s.", 	\
				__FILE__, __LINE__, __FUNCTION__, #a, log_str); 			\
		fail_handle; 														\
	}

#define YQ_ASSERT_LOG(log, a, fail_handle) 						\
	if((a) == false) 											\
	{ 															\
		LOG_ERROR(log, "[%s : %d : %s] ASSERT: (%s) == false.",	\
			__YQ_FILE__, __LINE__,__FUNCTION__, #a);			\
		fail_handle; 											\
	}

#define YQ_ASSERT_LOG_STR(log, a, fail_handle, str, ...) 			\
	if((a) == false) 												\
	{ 																\
		char log_str[1024]; 										\
		sprintf(log_str, str, ##__VA_ARGS__); 						\
		LOG_ERROR(log, "[%s : %d : %s] ASSERT: (%s) == false, %s.",	\
			__YQ_FILE__, __LINE__, __FUNCTION__, #a, log_str); 		\
		fail_handle; 												\
	}
#else
#define YQ_ASSERT(a, fail_handle) 	\
	do 								\
	{ 								\
		assert(a); 					\
	} while(0);

#define YQ_ASSERT_STR(a, fail_handle, str, ...) 			YQ_ASSERT(a, fail_handle)
#define YQ_ASSERT_LOG(log, a, fail_handle) 					YQ_ASSERT(a, fail_handle)
#define YQ_ASSERT_LOG_STR(log, a, fail_handle, str, ...) 	YQ_ASSERT( a, fail_handle )

#endif // USE_YQ_ASSERT

#endif // __YQ_ASSERT_H__

   


      
  

