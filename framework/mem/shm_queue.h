/*****************************************************************
* FileName:shm_queue.h
* Summary :
* Date	  :2024-2-26
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __SHM_QUEUE_H__
#define __SHM_QUEUE_H__

#include <iostream>
#include "shm.h"
#include "platform_def.h"

#define EXTRA_BYTE (8)

#pragma pack(CACHE_LINE_SIZE)                  
struct stMemTrunk
{
    /**
     * 0) 单线程读单线程写模型　https://blog.csdn.net/D_Guco/article/details/100066985
     * 1) 这里读写索引用int类型,cpu可以保证对float,double和long除外的基本类型的读写是原子的,保证一个线程不会读到另外一个线程写到一半的值
     * 2) 在变量之间插入一个64字节(cache line的长度)的变量(没有实际的计算意义),但是可以保证两个变量不会同时在一个cache line里,防止不同的
     *    进程或者线程同时访问在一个cache line里不同的变量产生false sharing,
     */
    volatile unsigned int m_iBegin;
    char __cache_padding1__[CACHE_LINE_SIZE];
    volatile unsigned int m_iEnd;
    char __cache_padding2__[CACHE_LINE_SIZE];
    int m_iShmKey;
    char __cache_padding3__[CACHE_LINE_SIZE];
    unsigned int m_iSize;
    char __cache_padding4__[CACHE_LINE_SIZE];
    int m_iShmId;
    char __cache_padding5__[CACHE_LINE_SIZE];
};
#pragma pack()                         

class CShmMessQueue
{
public:
    CShmMessQueue();
    ~CShmMessQueue();
    CShmMessQueue(const CShmMessQueue&) = delete;
    CShmMessQueue(CShmMessQueue&&) = delete;
    CShmMessQueue& operator=(const CShmMessQueue&) = delete;
public:
    bool Init(sm_key shmKey,size_t size);
    /**
     * 添加消息 对于mes queue来说是写操作，因为在队列中添加了一个消息包,仅修改m_iEnd
     * 写取共享内存管道（改变读写索引）,，读共享内存仅改变m_iEnd，保证读单进程读和写进程不会发生竞争，写不会造成数据不一致
     * @param message
     * @param length
     * @return
     */
    int SendMessage(BYTE* message, msize_t length);
    /**
     * 获取消息 对于mes queue来说是读操作，因为从队列中拿走了一个消息包 仅修改m_iBegin
     * 读取共享内存管道（改变读写索引）,，读共享内存仅改变m_iBegin，保证读单进程读和写进程不会发生竞争，写不会造成数据不一致
     * @param pOutCode
     * @return message  > 0 data len ,or < 0 error code
     * */
    int GetMessage(BYTE* pOutCode);
    /**
     * 从mess queue 头部读取一个消息，从队列中copy走了一个消息包没有改变mess queue
     * @param pOutCode
     * @param pOutLength
     * @return message  > 0 data len ,or < 0 error code
     * */
    int ReadHeadMessage(BYTE* pOutCode);
    /**
     * 从mess queue删除头部删除一个消息，仅修改m_iBegin
     * @param iCodeOffset
     * @param pOutCode
     * @return
     * */
    int DeleteHeadMessage();
    /**
     * 打印队列信息
     **/
    void DebugTrunk();
    //获取空闲区大小
    unsigned int GetFreeSize();
    //获取数据长度
    unsigned int GetDataSize();
    //获取存储数据的内存取长度（空闲的和占用的）
    unsigned int GetQueueLength();
    //是否数据为空
    bool IsEmpty();
public:
    //是否是2的次方
    static bool IsPowerOfTwo(size_t size);
    //求最接近的最大2的指数次幂
    static int Fls(size_t size);
    static size_t RoundupPowofTwo(size_t size);
    //创建CMssageQueue对象
    /**
     *
     * @param shmkey
     * @param queuesize 如果传入的size != 2^n,size 会初始化为>size的最小的2^n的数,例如2^n-1 < size < 2^n,
     *                  则MessageQueue被初始化为2^n
     * @return
     */
    static CShmMessQueue* CreateInstance(int shmkey,size_t queuesize);
private:
    stMemTrunk* m_stMemTrunk;
    BYTE*       m_pQueueAddr;
    CSharedMem  m_ShareMem;
};
#endif //__SHM_QUEUE_H__

