//
// Created by DGuco on 17-7-26.
// 内存池安全操作接口,线程安全
//

#ifndef SERVER_MEMPOOLSAFTY_H
#define SERVER_MEMPOOLSAFTY_H

#include <mutex>
#include "mempool.h"

template<class Type>
class CMemoryPoolSafty : public CMemoryPool<Type>
{
private:
    std::mutex	m_cs;
public:
    virtual bool Create(uint nNumOfElements )
    {
        return CMemoryPool<Type>::Create( nNumOfElements );
    }

    virtual void Destroy()
    {
        CMemoryPool<Type>::Destroy();
    }

    virtual Type* Alloc()
    {
        std::lock_guard<std::mutex> guard(m_cs);
        Type* pType;
        pType = CMemoryPool<Type>::Alloc();
        return pType;
    }

    virtual void Free( Type* pElement )
    {
        std::lock_guard<std::mutex> guard(m_cs);
        CMemoryPool<Type>::Free( pElement );
    }

};
#endif //SERVER_MEMPOOLSAFTY_H
