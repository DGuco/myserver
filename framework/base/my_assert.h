#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>
#include "base.h"

#include "log.h"

#define DO_NOTHING {}

#define ASSERT(a)                                        \
    if ((a) == false)                                                    \
    {                                                                    \
        LOG_ERROR("default","[{} : {} : {}] ASSERT: ({}) == flase.",    \
                __FILE__,__LINE__,__FUNCTION__,#a);                        \
    }

#define ASSERT_EX(a, str, ...)                            \
    if((a) == false)                                                        \
    {                                                                        \
        char log_str[ 1024 ];                                                \
        sprintf(log_str, str, ##__VA_ARGS__);                                \
        LOG_ERROR( "default", "[{} : {} : {}] ASSERT: ({}) == false, {}.",    \
                __FILE__, __LINE__, __FUNCTION__, #a, log_str);            \
        throw std::logic_error(log_str);\
    }

#endif // __MY_ASSERT_H__

   


      
  

