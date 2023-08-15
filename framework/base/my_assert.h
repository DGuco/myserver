#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>
#include "base.h"

#ifdef USE_MY_ASSERT
#include "log.h"

#define DO_NOTHING {}

#define ASSERT(a, fail_handle)                                        \
    if ((a) == false)                                                    \
    {                                                                    \
        LOG_ERROR("default","[{} : {} : {}] ASSERT: ({}) == flase.",    \
                __FILE__,__LINE__,__FUNCTION__,#a);                        \
        fail_handle;                                                    \
    }

#define ASSERT_EX(a, fail_handle, str, ...)                            \
    if((a) == false)                                                        \
    {                                                                        \
        char log_str[ 1024 ];                                                \
        sprintf(log_str, str, ##__VA_ARGS__);                                \
        LOG_ERROR( "default", "[{} : {} : {}] ASSERT: ({}) == false, {}.",    \
                __FILE__, __LINE__, __FUNCTION__, #a, log_str);            \
        fail_handle;                                                        \
    }}
#else
#define ASSERT(a, fail_handle) 	\
	do 								\
	{ 								\
		assert(a); 					\
	} while(0);

#define ASSERT_EX(a, fail_handle, str, ...) 			ASSERT(a, fail_handle)

#endif // USE_MY_ASSERT

#endif // __MY_ASSERT_H__

   


      
  

