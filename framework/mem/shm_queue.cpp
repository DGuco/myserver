#include "shm_queue.h"
#include "log.h"

CShmMessQueue::CShmMessQueue()
{
	m_pQueueAddr = NULL;
	m_stMemTrunk = NULL;
}

CShmMessQueue::~CShmMessQueue()
{
    if (m_stMemTrunk) 
    {
        m_stMemTrunk->~stMemTrunk();
    }

    m_pQueueAddr = NULL;
    //m_ShareMem.DetachSegment();
}

bool CShmMessQueue::Init(sm_key shmKey,size_t size)
{
    bool bRet = m_ShareMem.CreateSegment(shmKey, size + sizeof(SSmHead) + sizeof(stMemTrunk));
    if (!bRet)
    {
        return false;
    }
	m_pQueueAddr = m_ShareMem.GetSegment();
	m_stMemTrunk = new (m_pQueueAddr) stMemTrunk();
	m_pQueueAddr += sizeof(stMemTrunk);
	m_stMemTrunk->m_iBegin = 0;
	m_stMemTrunk->m_iEnd = 0;
	m_stMemTrunk->m_iShmKey = shmKey;
	m_stMemTrunk->m_iSize = (unsigned int)size;
    return true;
}

int CShmMessQueue::SendMessage(BYTE* message, int length)
{
    if (!message || length <= 0) {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    // 首先判断是否队列已满
    int size = GetFreeSize();
    if (size <= 0) 
    {
        return (int)eQueueErrorCode::QUEUE_NO_SPACE;
    }

    //空间不足
    if ((length + sizeof(int)) > size)
    {
        return (int)eQueueErrorCode::QUEUE_NO_SPACE;
    }

    int usInLength = length;
    BYTE* pTempDst = m_pQueueAddr;
    BYTE* pTempSrc = (BYTE*)(&usInLength);

    //写入的时候我们在数据头插上数据的长度，方便准确取数据,每次写入一个字节可能会分散在队列的头和尾
    unsigned int tmpEnd = m_stMemTrunk->m_iEnd;
    for (int i = 0; i < sizeof(usInLength); i++)
    {
        pTempDst[tmpEnd] = pTempSrc[i];  // 拷贝 Code 的长度
        tmpEnd = (tmpEnd + 1) & (m_stMemTrunk->m_iSize - 1);  // % 用于防止 Code 结尾的 idx 超出 codequeue
    }

    unsigned int tmpLen = MIN(usInLength, m_stMemTrunk->m_iSize - tmpEnd);
    memcpy((void*)(&pTempDst[tmpEnd]), (const void*)message, (size_t)tmpLen);
    size_t tmpLastLen = length - tmpLen;
    if (tmpLastLen > 0)
    {
        /* then put the rest (if any) at the beginning of the buffer */
        memcpy(&pTempDst[0], message + tmpLen, tmpLastLen);
    }

    /*
    * Ensure that we add the bytes to the kfifo -before-
    * we update the fifo->in index.
    * 数据写入完成修改m_iEnd，保证读端不会读到写入一半的数据
    */
    __WRITE_BARRIER__;
    m_stMemTrunk->m_iEnd = (tmpEnd + usInLength) & (m_stMemTrunk->m_iSize - 1);
    return (int)eQueueErrorCode::QUEUE_OK;
}

int CShmMessQueue::GetMessage(BYTE* pOutCode)
{
    if (!pOutCode) 
    {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    BYTE* pTempSrc = m_pQueueAddr;
    // 如果数据的最大长度不到sizeof(MESS_SIZE_TYPE)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(int))
    {
        DISK_LOG(SHM_ERROR,"ReadHeadMessage data len illegal,nTempMaxLength {}", nTempMaxLength);
        DebugTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    int usOutLength;
    BYTE* pTempDst = (BYTE*)&usOutLength;   // 数据拷贝的目的地址
    unsigned int tmpBegin = m_stMemTrunk->m_iBegin;
    //取出数据的长度
    for (int i = 0; i < sizeof(int); i++)
    {
        pTempDst[i] = pTempSrc[tmpBegin];
        tmpBegin = (tmpBegin + 1) & (m_stMemTrunk->m_iSize - 1);
    }

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(int)) || usOutLength < 0)
    {
        DISK_LOG(SHM_ERROR,"ReadHeadMessage usOutLength illegal,usOutLength: %d,nTempMaxLength {}\n",usOutLength, nTempMaxLength);
        DebugTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    pTempDst = &pOutCode[0];  // 设置接收 Code 的地址
    unsigned int tmpLen = MIN(usOutLength, m_stMemTrunk->m_iSize - tmpBegin);
    memcpy(&pTempDst[0], &pTempSrc[tmpBegin], tmpLen);
    unsigned int tmpLast = usOutLength - tmpLen;
    if (tmpLast > 0)
    {
        memcpy(&pTempDst[tmpLen], pTempSrc, tmpLast);
    }

    __WRITE_BARRIER__;
    m_stMemTrunk->m_iBegin = (tmpBegin + usOutLength) & (m_stMemTrunk->m_iSize - 1);
    return usOutLength;
}

/**
    *函数名          : PeekHeadCode
    *功能描述        : 查看共享内存管道（不改变读写索引）
    * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CShmMessQueue::ReadHeadMessage(BYTE* pOutCode)
{
    if (!pOutCode) 
    {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) 
    {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    BYTE* pTempSrc = m_pQueueAddr;
    // 如果数据的最大长度不到sizeof(MESS_SIZE_TYPE)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(int))
    {
        DISK_LOG(SHM_ERROR,"ReadHeadMessage data len illegal, nTempMaxLength{}",nTempMaxLength);
        DebugTrunk();
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    int usOutLength;
    BYTE* pTempDst = (BYTE*)&usOutLength;   // 数据拷贝的目的地址
    unsigned int tmpBegin = m_stMemTrunk->m_iBegin;
    //取出数据的长度
    for (int i = 0; i < sizeof(int); i++)
    {
        pTempDst[i] = pTempSrc[tmpBegin];
        tmpBegin = (tmpBegin + 1) & (m_stMemTrunk->m_iSize - 1);
    }

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(int)) || usOutLength < 0)
    {
        DISK_LOG(SHM_ERROR, "ReadHeadMessage usOutLength illegal, usOutLength: {}, nTempMaxLength{} ",usOutLength, nTempMaxLength);
        DebugTrunk();
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    pTempDst = &pOutCode[0];  // 设置接收 Code 的地址
    unsigned int tmpIndex = tmpBegin & (m_stMemTrunk->m_iSize - 1);
    unsigned int tmpLen = MIN(usOutLength, m_stMemTrunk->m_iSize - tmpIndex);
    memcpy(pTempDst, pTempSrc + tmpBegin, tmpLen);
    unsigned int tmpLast = usOutLength - tmpLen;
    if (tmpLast > 0)
    {
        memcpy(pTempDst + tmpLen, pTempSrc, tmpLast);
    }
    return usOutLength;
}

/**
    *函数名          : GetOneCode
    *功能描述        : 从指定位置iCodeOffset获取指定长度nCodeLength数据
    * */
int CShmMessQueue::DeleteHeadMessage()
{
    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) 
    {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    BYTE* pTempSrc = m_pQueueAddr;
    // 如果数据的最大长度不到sizeof(MESS_SIZE_TYPE)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(int))
    {
		DISK_LOG(SHM_ERROR, "ReadHeadMessage data len illegal,nTempMaxLength {}", nTempMaxLength);
        DebugTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    int usOutLength;
    BYTE* pTempDst = (BYTE*)&usOutLength;   // 数据拷贝的目的地址
    unsigned int tmpBegin = m_stMemTrunk->m_iBegin;
    //取出数据的长度
    for (int i = 0; i < sizeof(int); i++)
    {
        pTempDst[i] = pTempSrc[tmpBegin];
        tmpBegin = (tmpBegin + 1) & (m_stMemTrunk->m_iSize - 1);
    }

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(int)) || usOutLength < 0)
    {
        DISK_LOG(SHM_ERROR,"ReadHeadMessage usOutLength illegal,usOutLength: {},nTempMaxLength {}",usOutLength, nTempMaxLength);
        DebugTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    m_stMemTrunk->m_iBegin = (tmpBegin + usOutLength) & (m_stMemTrunk->m_iSize - 1);
    return usOutLength;
}

void CShmMessQueue::DebugTrunk()
{
    DISK_LOG(SHM_DEBUG,"Mem trunk address 0x%p,shmkey {} ,shmid {}, size {}, begin {}, end {}\n",
        (void*)m_stMemTrunk,
        m_stMemTrunk->m_iShmKey,
        m_stMemTrunk->m_iShmId,
        m_stMemTrunk->m_iSize,
        m_stMemTrunk->m_iBegin,
        m_stMemTrunk->m_iEnd);
}

//获取空闲区大小
unsigned int CShmMessQueue::GetFreeSize()
{
    //长度应该减去预留部分长度8，保证首尾不会相接
    return GetQueueLength() - GetDataSize() - EXTRA_BYTE;
}

//获取数据长度
unsigned int CShmMessQueue::GetDataSize()
{
    //第一次写数据前
    if (m_stMemTrunk->m_iBegin == m_stMemTrunk->m_iEnd) 
    {
        return 0;
    }
    //数据在两头
    else if (m_stMemTrunk->m_iBegin > m_stMemTrunk->m_iEnd) 
    {

        return  (unsigned int)(m_stMemTrunk->m_iEnd + m_stMemTrunk->m_iSize - m_stMemTrunk->m_iBegin);
    }
    else   //数据在中间
    {
        return m_stMemTrunk->m_iEnd - m_stMemTrunk->m_iBegin;
    }
}

unsigned int CShmMessQueue::GetQueueLength()
{
    return (unsigned int)m_stMemTrunk->m_iSize;
}


//是否数据为空
bool  CShmMessQueue::IsEmpty()
{
    return GetDataSize() <= 0;
}

bool CShmMessQueue::IsPowerOfTwo(size_t size) 
{
    if (size < 1)
    {
        return false;//2的次幂一定大于0
    }
    return ((size & (size - 1)) == 0);
}


int CShmMessQueue::Fls(size_t size) {
    int position;
    int i;
    if (0 != size)
    {
        for (i = (size >> 1), position = 0; i != 0; ++position)
            i >>= 1;
    }
    else
    {
        position = -1;
    }
    return position + 1;
}

size_t CShmMessQueue::RoundupPowofTwo(size_t size) 
{
    return 1UL << Fls(size - 1);
}

CShmMessQueue* CShmMessQueue::CreateInstance(int shmkey,size_t queuesize)
{
    if (queuesize <= 0)
    {
        return NULL;
    }

    queuesize = IsPowerOfTwo(queuesize) ? queuesize : RoundupPowofTwo(queuesize);
    if (queuesize <= 0) 
    {
        return NULL;
    }
    eShmModule shmModule;
    int shmId = 0;
    CShmMessQueue* pMessQueue = new CShmMessQueue();
    if (pMessQueue == NULL)
    {
        return NULL;
    }
    bool bRet = pMessQueue->Init(shmkey, queuesize);
    if (!bRet)
    {
        DELETE(pMessQueue);
        return NULL;
    }
    return pMessQueue;
}
