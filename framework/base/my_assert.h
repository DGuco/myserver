#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>
#include "base.h"

#ifdef USE_MY_ASSERT
#include "../log/log.h"
#define MY_ASSERT(a, fail_handle) 										\
	if ((a) == false) 													\
	{ 																	\
		LOG_ERROR("default","[%s : %d : %s] ASSERT: (%s) == flase.",	\
				__FILE__,__LINE__,__FUNCTION__,#a);						\
		fail_handle; 													\
	}
	
#define MY_ASSERT_STR(a, fail_handle, str, ...) 							\
	if((a) == false) 														\
	{ 																		\
		char log_str[ 1024 ]; 												\
		sprintf(log_str, str, ##__VA_ARGS__); 								\
		LOG_ERROR( "default", "[%s : %d : %s] ASSERT: (%s) == false, %s.", 	\
				__FILE__, __LINE__, __FUNCTION__, #a, log_str); 			\
		fail_handle; 														\
	}

#define MY_ASSERT_LOG(log, a, fail_handle) 						\
	if((a) == false) 											\
	{ 															\
		LOG_ERROR(log, "[%s : %d : %s] ASSERT: (%s) == false.",	\
			__YQ_FILE__, __LINE__,__FUNCTION__, #a);			\
		fail_handle; 											\
	}

#define MY_ASSERT_LOG_STR(log, a, fail_handle, str, ...) 			\
	if((a) == false) 												\
	{ 																\
		char log_str[1024]; 										\
		sprintf(log_str, str, ##__VA_ARGS__); 						\
		LOG_ERROR(log, "[%s : %d : %s] ASSERT: (%s) == false, %s.",	\
			__YQ_FILE__, __LINE__, __FUNCTION__, #a, log_str); 		\
		fail_handle; 												\
	}
#else
#define MY_ASSERT(a, fail_handle) 	\
	do 								\
	{ 								\
		assert(a); 					\
	} while(0);

#define MY_ASSERT_STR(a, fail_handle, str, ...) 			YQ_ASSERT(a, fail_handle)
#define MY_ASSERT_LOG(log, a, fail_handle) 					YQ_ASSERT(a, fail_handle)
#define MY_ASSERT_LOG_STR(log, a, fail_handle, str, ...) 	YQ_ASSERT( a, fail_handle )

#endif // USE_MY_ASSERT

#endif // __MY_ASSERT_H__

   


      
  

