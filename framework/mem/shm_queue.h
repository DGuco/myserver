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
     * 0) ���̶߳����߳�дģ�͡�https://blog.csdn.net/D_Guco/article/details/100066985
     * 1) �����д������int����,cpu���Ա�֤��float,double��long����Ļ������͵Ķ�д��ԭ�ӵ�,��֤һ���̲߳����������һ���߳�д��һ���ֵ
     * 2) �ڱ���֮�����һ��64�ֽ�(cache line�ĳ���)�ı���(û��ʵ�ʵļ�������),���ǿ��Ա�֤������������ͬʱ��һ��cache line��,��ֹ��ͬ��
     *    ���̻����߳�ͬʱ������һ��cache line�ﲻͬ�ı�������false sharing,
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
     * �����Ϣ ����mes queue��˵��д��������Ϊ�ڶ����������һ����Ϣ��,���޸�m_iEnd
     * дȡ�����ڴ�ܵ����ı��д������,���������ڴ���ı�m_iEnd����֤�������̶���д���̲��ᷢ��������д����������ݲ�һ��
     * @param message
     * @param length
     * @return
     */
    int SendMessage(BYTE* message, msize_t length);
    /**
     * ��ȡ��Ϣ ����mes queue��˵�Ƕ���������Ϊ�Ӷ�����������һ����Ϣ�� ���޸�m_iBegin
     * ��ȡ�����ڴ�ܵ����ı��д������,���������ڴ���ı�m_iBegin����֤�������̶���д���̲��ᷢ��������д����������ݲ�һ��
     * @param pOutCode
     * @return message  > 0 data len ,or < 0 error code
     * */
    int GetMessage(BYTE* pOutCode);
    /**
     * ��mess queue ͷ����ȡһ����Ϣ���Ӷ�����copy����һ����Ϣ��û�иı�mess queue
     * @param pOutCode
     * @param pOutLength
     * @return message  > 0 data len ,or < 0 error code
     * */
    int ReadHeadMessage(BYTE* pOutCode);
    /**
     * ��mess queueɾ��ͷ��ɾ��һ����Ϣ�����޸�m_iBegin
     * @param iCodeOffset
     * @param pOutCode
     * @return
     * */
    int DeleteHeadMessage();
    /**
     * ��ӡ������Ϣ
     **/
    void DebugTrunk();
    //��ȡ��������С
    unsigned int GetFreeSize();
    //��ȡ���ݳ���
    unsigned int GetDataSize();
    //��ȡ�洢���ݵ��ڴ�ȡ���ȣ����еĺ�ռ�õģ�
    unsigned int GetQueueLength();
    //�Ƿ�����Ϊ��
    bool IsEmpty();
public:
    //�Ƿ���2�Ĵη�
    static bool IsPowerOfTwo(size_t size);
    //����ӽ������2��ָ������
    static int Fls(size_t size);
    static size_t RoundupPowofTwo(size_t size);
    //����CMssageQueue����
    /**
     *
     * @param shmkey
     * @param queuesize ��������size != 2^n,size ���ʼ��Ϊ>size����С��2^n����,����2^n-1 < size < 2^n,
     *                  ��MessageQueue����ʼ��Ϊ2^n
     * @return
     */
    static CShmMessQueue* CreateInstance(int shmkey,size_t queuesize);
private:
    stMemTrunk* m_stMemTrunk;
    BYTE*       m_pQueueAddr;
    CSharedMem  m_ShareMem;
};
#endif //__SHM_QUEUE_H__

