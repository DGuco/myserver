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

int ClientCommEngine::ParseMessageHead(const void* pBuff,
                                    MSG_LEN_TYPE& unLen,
                                    unsigned int& unSerial,
                                    unsigned int& unSeqId,
                                    int& unCmd)
{
    if ( (pBuff == NULL))
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ParseMessageHead Input param failed.");
    }
    MSG_LEN_TYPE unTmpLen = 0;
}

int ClientCommEngine::ParseClientStream(const void** pBuff,
                                        MSG_LEN_TYPE& nRecvAllLen,
                                        C2SHead* pHead,
                                        MSG_LEN_TYPE& unTmpDataLen,
                                        MSG_CMD_TYPE& unCmd)
{
    if ( (pBuff == NULL) || (pHead == NULL))
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToMsg Input param failed.");
    }

    //小于最小长度继续接收
    if ( nRecvAllLen < MSG_HEAD_LEN)
    {
        MY_ASSERT_STR(0,return 1,"the package len is less than base len ,receive len %d",nRecvAllLen);
    }

    char* pTemp = (char*)*pBuff;
    MSG_LEN_TYPE unRecvLen = 0;
    MSG_LEN_TYPE unTmpUseLen = 0;
    //取出包的总长度
    memcpy(&unRecvLen,(void*)pTemp,sizeof(MSG_LEN_TYPE));
    unRecvLen = ntohs(len); 
    //客户端上行包的总长度小于基本长度大于最大长度，不合法
    if (unRecvLen < MSG_HEAD_LEN || unRecvLen > MSG_MAX_LEN)
    {
        MY_ASSERT_STR(0,return -1,"default","the package len is illegal,receive len %d",nRecvAllLen);
    }
    //接收数据的总长度小于包的总长度继续接收
    if(nRecvAllLen - unRecvLen  < 0)
    {
        MY_ASSERT_STR(0,return 1, "Receive client part data left len = %d",nRecvAllLen,unRecvLen);
    }

    //数据总长度
    unTmpDataLen = unRecvLen;
    //数据指针向后移动指向未读取位置
    pTemp += sizeof(MSG_LEN_TYPE);
    unTmpUseLen += sizeof(MSG_LEN_TYPE);

    // 序列号
    unsigned short tTmpSeq = 0;
    memcpy(&tTmpSeq,(void*)pTemp,sizeof(unsigned short));
    pTemp += sizeof(unsigned short);
    unTmpUseLen += sizeof(unsigned short);

    // protobuf版本
    unsigned short tTmpProbufVersion = 0;
    memcpy(&tTmpProbufVersion,(void*)pTemp,sizeof(unsigned short));
    pTemp += sizeof(unsigned short);
    unTmpUseLen += sizeof(unsigned short);

    // 是否加密
    unsigned char tTmpIsEncry = 0;
    memcpy(&tTmpIsEncry,(void*)pTemp,sizeof(unsigned char));
    pTemp += sizeof(unsigned char);
    unTmpUseLen += sizeof(unsigned char);

    //消息指令编号
    MSG_CMD_TYPE tTmpCmd = 0;
    memcpy(&tTmpCmd,(void*)pTemp,sizeof(MSG_CMD_TYPE));
    tTmpCmd = ntohs(tTmpCmd);
    pTemp += sizeof(MSG_CMD_TYPE);
    unTmpUseLen += sizeof(MSG_CMD_TYPE);  

    //指针指向当前数据包的末尾
    pTemp += unRecvLen - unTmpUseLen;
    unCmd = tTmpCmd;

    return 0;
}

int ClientCommEngine::ConverToGameStream(const void * pBuff,
                                MSG_LEN_TYPE& unBuffLen,
                                const void *pDataBuff,
                                MSG_LEN_TYPE& unDataLen,
                                C2SHead* pHead)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL )
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char* pTemp = (char*)pBuff;
    
    //预留总长度
    MSG_LEN_TYPE unLength = 0;
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);
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

    // // 开始消息解密
    // char tEncryBuff[MAX_PACKAGE_LEN] = {0};
    // char* tpEncryBuff = &tEncryBuff[0];
    // unsigned int tOutLen = MAX_PACKAGE_LEN;
    // if (tTmpIsEncry)
    // {
    //     // 加密的消息处理
    //     int tDecLen = tOutLen;
    //     //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
    //     CAes tmpAes;
    //     tmpAes.init(tpKey,16);
    //     int outlen;
    //     tpEncryBuff = (unsigned char*)tmpAes.decrypt((const char*)pDataBuff,unDataLen,tDecLen);
    //     tOutLen = tDecLen;
    // }
    // else
    // {
    //     // 未加密的消息处理
    //     tpEncryBuff = (unsigned char*)pTemp1;
    //     tOutLen = unDataLen;
    // }

    // 拷贝消息到发送缓冲区
    memcpy(pTemp, (char*)pDataBuff, unDataLen);
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
                              S2CHead* pHead,
							  Message* pMsg)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL)
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
                                            C2SHead* pHead,
                                            Message* pMessage,
                                            CFactory* pMsgFactory = NULL)
{
    if ((pBuff == NULL) || (pHead == NULL)|| (pMessage == NULL))
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}
    
    char* pbyTmpBuff = (char*)pBuff;
    //取出数据总长度
    unsigned int unTmpTotalLen = *(unsigned int*) pbyTmpBuff;
    int iTmpLen = unTmpTotalLen;
    pbyTmpBuff += sizeof(unsigned int);		// 指针指向数据处
    iTmpLen -= sizeof(unsigned int);		// 从长度减少一个len的长度

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

    //扔掉字节对齐长度
    iTmpLen -= unTmpAddlLen;

	//C2SHead 长度
    unsigned int tmpHeadLen = *(unsigned int*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned int);
    iTmpLen -= sizeof(unsigned int);	

    //反序列化失败
    if (pHead->ParseFromArray(pbyTmpBuff,tmpHeadLen) == false)
    {
        return -1;
    }
    pbyTmpBuff += tmpHeadLen;
    iTmpLen -= tmpHeadLen;

    //如果有消息
    if (iTmpLen > 0 && pMsgFactory != NULL)
    {
        pMessage = pMsgFactory->CreateMessage(pHead->cmd());
    }

    if (pMessage == NULL) 
    {
        return -1;
    }

    if (pMessage->ParseFromString(tmpClientMessage.msgparas()) == false)
    {
        pMessage->~Message();
        return -1;        
    }
}  

int ClientCommEngine::ConvertStreamToMessage(const void* pBuff,
                                            unsigned short unBuffLen,
                                            S2CHead* pHead,
                                            unsigned int& unTmpOffset)
{
    if ((pBuff == NULL) || (pHead == NULL))
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}
    
    char* pbyTmpBuff = (char*)pBuff;
    //取出数据总长度
    unsigned int unTmpTotalLen = *(unsigned int*) pbyTmpBuff;
    unTmpOffset = 0;
    pbyTmpBuff += sizeof(unsigned int);		// 指针指向数据处
    unTmpOffset += sizeof(unsigned int);		// 从长度减少一个len的长度

    // 总长度不匹配
    if (unTmpTotalLen != unBuffLen)
    {
        MY_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertStreamToMsg tTotalLen = %d unequal to unBuffLen = %d.", unTmpTotalLen, unBuffLen);
    }

	// 字节对齐补充长度（采用8字节对齐）
    unsigned short unTmpAddlLen = *(unsigned short*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned short);
    unTmpOffset += sizeof(unsigned short);	

    //扔掉字节对齐长度
    iTmpLen -= unTmpAddlLen;

	//S2CHead 长度
    unsigned int tmpHeadLen = *(unsigned int*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned int);
    unTmpOffset += sizeof(unsigned int);	

    //反序列化失败
    if (pHead->ParseFromArray(pbyTmpBuff,tmpHeadLen) == false)
    {
        return -1;
    }
    pbyTmpBuff += tmpHeadLen;
    unTmpOffset += tmpHeadLen;
} 
