#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>
#include "base.h"
#include "log.h"

#define DO_NOTHING {}

#define ASSERT(a)                                        \
    if ((a) == false)                                                    \
    {                                                                    \
        DISK_LOG(ASSERT_DISK,"assert","[{} : {} : {}] ASSERT: ({}) == flase.",    \
                __FILE__,__LINE__,__FUNCTION__,#a);                        \
    }

#define ASSERT_EX(a, str, ...)                            \
    if((a) == false)                                                        \
    {                                                                        \
        char log_str[ 1024 + 1 ];                                                \
        sprintf_s(log_str,1024, str, ##__VA_ARGS__);                                \
        DISK_LOG(ASSERT_DISK, "[{} : {} : {}] ASSERT: ({}) == false, {}.",    \
                __FILE__, __LINE__, __FUNCTION__, #a, log_str);            \
        throw std::logic_error(log_str);\
    }

#endif // __MY_ASSERT_H__

   


      
  

