#ifndef MY_NEW_H
#define MY_NEW_H

#include <stdlib.h>

void operator_new_log(size_t size, const char *file, unsigned int line);
void operator_new_arr_log(size_t size, const char *file, unsigned int line);
void operator_delete_log(void *ptr); 
void operator_delete_arr_log(void *ptr); 

inline void* operator new(size_t size, const char *file, unsigned int line)
{
    void *ptr = malloc(size);
    operator_new_log(size, file, line);
    return ptr;
}

#pragma warning(push)
#pragma warning(disable:4595)

inline void operator delete(void *ptr) 
{
    operator_delete_log(ptr);
    free(ptr);
}

inline void operator delete[](void *ptr) 
{
    operator_delete_arr_log(ptr);
    free(ptr);
}
// 代码
#pragma warning(pop)

// 新增匹配的 delete 运算符
inline void operator delete(void* ptr, const char*, unsigned int) noexcept {
    operator_delete_log(ptr);
    free(ptr);
}

inline void operator delete[](void* ptr, const char*, unsigned int) noexcept {
    operator_delete_arr_log(ptr);
    free(ptr);
}

// 为 C++14 新增 size_t 版本
inline void operator delete(void* ptr, size_t, const char* file, unsigned int line) noexcept {
    operator_delete_log(ptr);
    free(ptr);
}

inline void operator delete[](void* ptr, size_t, const char* file, unsigned int line) noexcept {
    operator_delete_arr_log(ptr);
    free(ptr);
}

#define MY_NEW new(__FILE__, __LINE__)

#endif