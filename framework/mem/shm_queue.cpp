//
//  messagequeue_h
//
//  Created by 杜国超 on 17/6/22.
//  Copyright © 2017年 杜国超. All rights reserved.
//
#include <algorithm> 
#include "shm_queue.h"

using namespace std;

// CMessageQueue::CMessageQueue(BYTE* pCurrAddr, eQueueModel module, key_t shmKey, int shmId, size_t size)
// {
//     m_pShm = (void*)pCurrAddr;
//     m_pQueueAddr = pCurrAddr;
//     m_stMemTrunk = new (m_pQueueAddr) stMemTrunk();
//     m_pQueueAddr += sizeof(stMemTrunk);
//     m_stMemTrunk->m_iBegin = 0;
//     m_stMemTrunk->m_iEnd = 0;
//     m_stMemTrunk->m_iShmKey = shmKey;
//     m_stMemTrunk->m_iShmId = shmId;
//     m_stMemTrunk->m_iSize = (unsigned int)size;
//     m_stMemTrunk->m_eQueueModule = module;
//     InitLock();
// }

CMessageQueue::~CMessageQueue()
{
 
}

int CMessageQueue::SendMessage(BYTE* message, msize_t length)
{
    if (!message || length <= 0) {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    // 首先判断是否队列已满
    unsigned int freeSize = GetFreeSize();
    if (freeSize <= 0)
    {
        return (int)eQueueErrorCode::QUEUE_NO_SPACE;
    }

    //空间不足
    if (length > freeSize)
    {
        return (int)eQueueErrorCode::QUEUE_NO_SPACE;
    }

    msize_t usInLength = length;
	msize_t usInLength = length;
    BYTE* pTempDst = m_pQueueAddr;

    unsigned int tmpLen = std::min(usInLength, m_stMemTrunk->m_iSize - tmpEnd);
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

int CMessageQueue::GetMessage(BYTE* pOutCode,msize_t& outlen)
{
    if (!pOutCode) {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    // 如果数据的最大长度不到sizeof(msize_t)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(msize_t)) {
        //PrintTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }


    msize_t usOutLength;
	//数据目的地址
    msize_t tmpDataLen = 0;
	unsigned int tmpBegin = m_stMemTrunk->m_iBegin;
    //数据源地址
	BYTE* pTempSrc = &m_pQueueAddr[0];
	memcpy((void*)(&tmpDataLen), pTempSrc, sizeof(msize_t));
    usOutLength = ntohl(tmpDataLen);
    outlen = usOutLength;

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(msize_t)) || usOutLength < 0) {
//         printf("[%s:%d] ReadHeadMessage usOutLength illegal,usOutLength: %d,nTempMaxLength %d \n",
//             __FILE__, __LINE__, usOutLength, nTempMaxLength);
//         PrintTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    BYTE* pTempDst = &pOutCode[0];  // 设置接收 Code 的地址
    unsigned int tmpLen = std::min(usOutLength, m_stMemTrunk->m_iSize - tmpBegin);
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
int CMessageQueue::ReadHeadMessage(BYTE* pOutCode, msize_t& outlen)
{
    if (!pOutCode) {
        return (int)eQueueErrorCode::QUEUE_PARAM_ERROR;
    }

    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    // 如果数据的最大长度不到sizeof(MESS_SIZE_TYPE)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(msize_t)) {
//         printf("[%s:%d] ReadHeadMessage data len illegal,nTempMaxLength %d \n", __FILE__, __LINE__, nTempMaxLength);
//         PrintTrunk();
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

	msize_t usOutLength;
	//数据目的地址
	msize_t tmpDataLen = 0;
	//数据源地址
	BYTE* pTempSrc = &m_pQueueAddr[0];
	memcpy((void*)(&tmpDataLen), pTempSrc, sizeof(msize_t));
	usOutLength = ntohl(tmpDataLen);
	outlen = usOutLength;

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(msize_t)) || usOutLength < 0) {
//         printf("[%s:%d] ReadHeadMessage usOutLength illegal,usOutLength: %d,nTempMaxLength %d \n",
//             __FILE__, __LINE__, usOutLength, nTempMaxLength);
//         PrintTrunk();
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    BYTE* pTempDst = &pOutCode[0];  // 设置接收 Code 的地址

    unsigned int tmpIndex = tmpBegin & (m_stMemTrunk->m_iSize - 1);
    unsigned int tmpLen = std::min(usOutLength, m_stMemTrunk->m_iSize - tmpIndex);
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
int CMessageQueue::DeleteHeadMessage()
{
    int nTempMaxLength = GetDataSize();
    if (nTempMaxLength <= 0) {
        return (int)eQueueErrorCode::QUEUE_NO_MESSAGE;
    }

    BYTE* pTempSrc = m_pQueueAddr;
    // 如果数据的最大长度不到sizeof(MESS_SIZE_TYPE)（存入数据时在数据头插入了数据的长度,长度本身）
    if (nTempMaxLength <= (int)sizeof(msize_t)) {
//         printf("[%s:%d] ReadHeadMessage data len illegal,nTempMaxLength %d \n", __FILE__, __LINE__, nTempMaxLength);
//         PrintTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    msize_t usOutLength;
    BYTE* pTempDst = (BYTE*)&usOutLength;   // 数据拷贝的目的地址
    unsigned int tmpBegin = m_stMemTrunk->m_iBegin;
    //取出数据的长度
    for (msize_t i = 0; i < sizeof(msize_t); i++) {
        pTempDst[i] = pTempSrc[tmpBegin];
        tmpBegin = (tmpBegin + 1) & (m_stMemTrunk->m_iSize - 1);
    }

    // 将数据长度回传
    //取出的数据的长度实际有的数据长度，非法
    if (usOutLength > (int) (nTempMaxLength - sizeof(msize_t)) || usOutLength < 0) {
//         printf("[%s:%d] ReadHeadMessage usOutLength illegal,usOutLength: %d,nTempMaxLength %d \n",
//             __FILE__, __LINE__, usOutLength, nTempMaxLength);
//         PrintTrunk();
        m_stMemTrunk->m_iBegin = m_stMemTrunk->m_iEnd;
        return (int)eQueueErrorCode::QUEUE_DATA_SEQUENCE_ERROR;
    }

    m_stMemTrunk->m_iBegin = (tmpBegin + usOutLength) & (m_stMemTrunk->m_iSize - 1);
    return usOutLength;
}

void CMessageQueue::PrintTrunk()
{
    printf("Mem trunk address 0x%p,shmkey %d ,shmid %d, size %d, begin %d, end %d, queue module %d \n",
        m_stMemTrunk,
        m_stMemTrunk->m_iShmKey,
        m_stMemTrunk->m_iShmId,
        m_stMemTrunk->m_iSize,
        m_stMemTrunk->m_iBegin,
        m_stMemTrunk->m_iEnd,
        m_stMemTrunk->m_eQueueModule);
}

//获取空闲区大小
unsigned int CMessageQueue::GetFreeSize()
{
    //长度应该减去预留部分长度8，保证首尾不会相接
    return GetQueueLength() - GetDataSize() - EXTRA_BYTE;
}

//获取数据长度
unsigned int CMessageQueue::GetDataSize()
{
    //第一次写数据前
    if (m_stMemTrunk->m_iBegin == m_stMemTrunk->m_iEnd) {
        return 0;
    }
    //数据在两头
    else if (m_stMemTrunk->m_iBegin > m_stMemTrunk->m_iEnd) {

        return  (unsigned int)(m_stMemTrunk->m_iEnd + m_stMemTrunk->m_iSize - m_stMemTrunk->m_iBegin);
    }
    else   //数据在中间
    {
        return m_stMemTrunk->m_iEnd - m_stMemTrunk->m_iBegin;
    }
}

unsigned int CMessageQueue::GetQueueLength()
{
    return (unsigned int)m_stMemTrunk->m_iSize;
}

bool CMessageQueue::IsPowerOfTwo(size_t size) {
    if (size < 1)
    {
        return false;//2的次幂一定大于0
    }
    return ((size & (size - 1)) == 0);
}


int CMessageQueue::Fls(size_t size) {
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

size_t CMessageQueue::RoundupPowofTwo(size_t size) {
    return 1UL << Fls(size - 1);
}

CMessageQueue* CMessageQueue::CreateInstance(key_t shmkey,
    size_t queuesize,
    eQueueModel queueModule)
{
    if (queuesize <= 0)
    {
        return NULL;
    }

    queuesize = IsPowerOfTwo(queuesize) ? queuesize : RoundupPowofTwo(queuesize);
    if (queuesize <= 0) {
        return NULL;
    }
    enShmModule shmModule;
    int shmId = 0;
    BYTE* tmpMem = CMessageQueue::CreateShareMem(shmkey, queuesize + sizeof(stMemTrunk), shmModule, shmId);
    CMessageQueue* messageQueue = new CMessageQueue(tmpMem, queueModule, shmkey, shmId, queuesize);
    messageQueue->PrintTrunk();
    return messageQueue;
}
