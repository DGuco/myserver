/*
 * client_comm_engine.cpp
 *
 *  Created on: 2016年11月26日
 *      Author: DGuco
 */

#include "client_comm_engine.h"
#include "../base/base.h"
#include "../base/my_assert.h"
#include "../net/oi_tea.h"

unsigned char ClientCommEngine::tKey[16] = {1,2,3,4,5,6,7,8,9,0,2,2,4,4,5,6};
unsigned char* ClientCommEngine::tpKey = &tKey[0];

int ClientCommEngine::ParseClientStream(const void* pBuff,
                                        unsigned short nRecvAllLen,
                                        MesHead* pHead,
                                        unsigned int& unTmpOffset,
                                        unsigned int& unTmpDataLen)
{
    if (
            (pBuff == NULL) ||
            (pHead == NULL) ||
            (pEncrypt == NULL)
            )
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToMsg Input param failed.");
    }

    //小于最小长度继续接收
    if ( nRecvAllLen < MSG_HEAD_LEN)
    {
        LOG_ERROR("default","the package len is less than base len ,receive len %d",nRecvAllLen);
        return 1;
    }

    char* pTemp1 = (char*)pBuff;
    unsigned int unRecvLen = 0;
    //取出包的总长度
    memcpy(&unRecvLen,(void*)pTemp1,sizeof(unsigned int));
    unRecvLen = ntohl(len); 
    //客户端上行包的总长度小于基本长度大于最大长度，不合法
    if (unRecvLen < MSG_HEAD_LEN || unRecvLen > MSG_MAX_LEN)
    {
        LOG_ERROR("default","the package len is illegal",nRecvAllLen);
        return -1;
    }
    //接收数据的总长度小于包的总长度继续接收
    if(nRecvAllLen - unRecvLen  < 0)
    {
        LOG_DEBUG("default", "Receive client part data left len = %d",nRecvAllLen,unRecvLen);
        return 1;
    }

    unTmpOffset = 0;;
    //数据指针向后移动指向未读取位置
    pTemp1 += sizeof(unsigned int);
    unTmpOffset += sizeof(unsigned int);

    // 序列号长度
    unsigned short tTmpSeq = 0;
    memcpy(&tTmpSeq,(void*)pTemp1,sizeof(unsigned short));
    pTemp1 += sizeof(unsigned short);
    unTmpOffset += sizeof(unsigned short);

    // protobuf版本
    unsigned short tTmpProbufVersion = 0;
    memcpy(&tTmpProbufVersion,(void*)pTemp1,sizeof(unsigned short));
    pTemp1 += sizeof(unsigned short);
    unTmpOffset += sizeof(unsigned short);

    // 是否加密
    unsigned char tTmpIsEncry = 0;
    memcpy(&tTmpIsEncry,(void*)pTemp1,sizeof(unsigned char));
    pTemp1 += sizeof(unsigned char);
    unTmpOffset += sizeof(unsigned char);

    //消息指令编号
    unsigned short tTmpCmd = 0;
    memcpy(&tTmpCmd,(void*)pTemp1,sizeof(unsigned short));
    tTmpCmd = ntohs(tTmpCmd);
    pTemp1 += sizeof(unsigned short);
    unTmpOffset += sizeof(unsigned short);

    pHead->Clear();
    pHead->set_cmd(tTmpCmd);
    pHead->set_seq(tTmpSeq);
    pHead->set_isencry(tTmpIsEncry);
    //计算出数据总长度
    unTmpDataLen = unRecvLen - unTmpOffset;
    
    return 0;
}

int ClientCommEngine::ConverToGameStream(const void * pBuff,
                                unsigned int& unBuffLen,
                                const void *pDataBuff,
                                unsigned int& unDataLen,
                                MesHead* pHead,
                                const unsigned char* pEncrypt = ClientCommEngine::tpKey)
{
	if ((pBuff == NULL) || (pMsg == NULL) )
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	unsigned char* pTemp = (char*)pBuff;
    
    //预留总长度
    unsigned int unLength = 0;
    pTemp += sizeof(unsigned int);
    unLength += sizeof(unsigned int);
    //预留8字节对齐长度
    pTemp += sizeof(unsigned short);
    unLength += sizeof(unsigned short);

    // C2SHead
	*(unsigned int*)pTemp = (unsigned int) pHead->ByteSize();
	pTemp += sizeof(unsigned int);
	unLength += sizeof(unsigned int);

	// C2SHead
	if (pHead->SerializeToArray(tpBuff, unBuffLen - unLength) == false)
	{
		MY_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pTcpHead->GetCachedSize();
	unLength += pTcpHead->GetCachedSize();


    // 开始消息解密
    char tEncryBuff[MAX_PACKAGE_LEN] = {0};
    char* tpEncryBuff = &tEncryBuff[0];
    unsigned int tOutLen = MAX_PACKAGE_LEN;
    if (tTmpIsEncry)
    {
        // 加密的消息处理
        int tDecLen = tOutLen;
        //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
        CAes tmpAes;
        tmpAes.init(tpKey,16);
        int outlen;
        tpEncryBuff = (unsigned char*)tmpAes.decrypt((const char*)pDataBuff,unDataLen,tDecLen);
        tOutLen = tDecLen;
    }
    else
    {
        // 未加密的消息处理
        tpEncryBuff = (unsigned char*)pTemp1;
        tOutLen = unDataLen;
    }

    // 拷贝消息到发送缓冲区
    memcpy(pTemp, (char*)tpEncryBuff, tOutLen);
    pTemp += tOutLen;
    unLength += tOutLen;

    //8字节对齐
    unsigned short iTmpAddlen = (unLength % 8);
    if (iTmpAddlen > 0)
    {
        iTmpAddlen = 8 - iTmpAddlen;
        //将字节对齐部分置为0
        memset(pTemp,0,iTmpAddlen);
    }
    unLength += iTmpAddlen;

    //回到消息起始地值补充数据从长度和字节补齐长度
    pTemp = ((char*))pBuff;
    //序列话消息总长度
    *(unsigned int*) pTemp = unLength;
    pTemp += sizeof(unsigned short);
    //序列话8字节对齐长度
    *(unsigned short*) pTemp = iTmpAddlen;

    unBuffLen = unLength;
}

int ClientCommEngine::ConvertMessageToStream(const void * pBuff,
							  unsigned int& unBuffLen,
                              MesHead* pHead,
							  Message* pMsg)
{
	if ((pBuff == NULL) || pHead == NULL)
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char* pTemp = (char*)pBuff;

    unsigned int unLength = 0;
    //预留总长度
    pTemp += sizeof(unsigned int);
    unLength += sizeof(unsigned int);
    //预留8字节对齐长度
    pTemp += sizeof(short);
    unLength += sizeof(short);

	// S2CHead长度
	*(unsigned int*)pTemp = (unsigned int) pTcpHead->ByteSize();
	pTemp += sizeof(unsigned int);
	unLength += sizeof(unsigned int);

	// S2CHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false)
	{
		MY_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pHead->GetCachedSize();
	unLength += pHead->GetCachedSize();

    if(pMsg != NULL)
    {
        char tEncryBuff[MAX_PACKAGE_LEN] = {0};
        char* tpEncryBuff = &tEncryBuff[0];
        unsigned int iMsgParaLen = MAX_PACKAGE_LEN;
        if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false)
        {
            MY_ASSERT_STR(0, return -5, "ClientCommEngine::ConvertMsgToStream MsgPara SerializeToArray failed.");
        }

        
        if (pHead->isencry())  
        {
            //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
            CAes tmpAes;
            tmpAes.init(tpKey,16);
            tpEncryBuff = tmpAes.encrypt(tpEncryBuff,pMsg->ByteSize(), iMsgParaLen);
        }
        else
        {
            iMsgParaLen = pMsg->ByteSize();
        }
        memccpy(pTemp,tpEncryBuff,iMsgParaLen);
        unLength += iMsgParaLen;
    }

    //8字节对齐
    unsigned short iTmpAddlen = (unLength % 8);
    if (iTmpAddlen > 0)
    {
        iTmpAddlen = 8 - iTmpAddlen;
        //将字节对齐部分置为0
        memset(pTemp,0,iTmpAddlen);
    }
    unLength += iTmpAddlen;

    //回到消息起始地值补充数据从长度和字节补齐长度
    pTemp = (char*)pBuff;
    //序列话消息总长度
    *(short*) pTemp = unLength;
    pTemp += sizeof(short);
    //序列话8字节对齐长度
    *(short*) pTemp = iTmpAddlen;

    unBuffLen = unLength;
}

int ClientCommEngine::ConvertStreamToMessage(const void* pBuff,
                                            unsigned short unBuffLen,
                                            MesHead* pHead,
                                            unsigned int& unTmpOffset,
                                            Message* pMessage,
                                            CFactory* pMsgFactory)
{
    if ((pBuff == NULL) || (pHead == NULL))
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}
    
    char* pbyTmpBuff = (char*)pBuff;
    //取出数据总长度
    unsigned int unTmpTotalLen = *(unsigned int*) pbyTmpBuff;
    int iTmpLen = unTmpTotalLen;
    unTmpOffset = 0;
    pbyTmpBuff += sizeof(unsigned int);		// 指针指向数据处
    iTmpLen -= sizeof(unsigned int);		// 从长度减少一个len的长度
    unTmpOffset += sizeof(unsigned int);

    // 总长度不匹配
    if (unTmpTotalLen != unBuffLen)
    {
        MY_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertStreamToMsg tTotalLen = %d unequal to unBuffLen = %d.", unTmpTotalLen, unBuffLen);
        return -1;
    }

	// 字节对齐补充长度（采用8字节对齐）
    unsigned short unTmpAddlLen = *(unsigned short*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned short);
    iTmpLen -= sizeof(unsigned short);	
    unTmpOffset += sizeof(unsigned short);

    //扔掉字节对齐长度
    iTmpLen -= unTmpAddlLen;
    unTmpOffset += unTmpAddlLen;

	//C2SHead 长度
    unsigned int tmpHeadLen = *(unsigned int*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned int);
    iTmpLen -= sizeof(unsigned int);	
    unTmpOffset += sizeof(unsigned int);

    //反序列化失败
    if (pHead->ParseFromArray(pbyTmpBuff,tmpHeadLen) == false)
    {
        return -1;
    }
    pbyTmpBuff += tmpHeadLen;
    iTmpLen -= tmpHeadLen;
    unTmpOffset += tmpHeadLen;

    //如果有消息
    if (iTmpLen > 0 && pMsgFactory != NULL)
    {
        pMessage = pMsgFactory->CreateMessage(pHead->cmd());
        if (pMessage == NULL) 
        {
            return -1;
        }

        if (pMessage->ParseFromString(tmpClientMessage.msgparas()) == false)
        {
            pMessage->~Message();
            return -1;        
        }
    }else {
        return iTmpLen;
    }
}  
