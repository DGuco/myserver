/*
 * client_comm_engine.cpp
 *
 *  Created on: 2016年11月26日
 *      Author: DGuco
 */

#include "client_comm_engine.h"

unsigned char ClientCommEngine::tKey[16] = {1,2,3,4,5,6,7,8,9,0,2,2,4,4,5,6};
unsigned char* ClientCommEngine::tpKey = &tKey[0];

void ClientCommEngine::CopyMesHead(MesHead* from,MesHead* to)
{
    if (from != NULL && to != NULL)
    {
        for (int i = 0;i < from->socketinfos_size();++i)
        {
            CSocketInfo* pSocketInfo = to->mutable_socketinfos()->Add();
            CSocketInfo socketInfo = from->socketinfos(i);
            pSocketInfo->set_socketid(socketInfo.socketid());
            pSocketInfo->set_createtime(socketInfo.createtime());
            pSocketInfo->set_state(socketInfo.state());
        }
        to->set_cmd(from->cmd());
        to->set_seq(from->seq());
        to->set_serial(from->serial());
    }

}

int ClientCommEngine::ParseClientStream(const void** pBuff,
                                        int nRecvAllLen,
                                        MesHead* pHead,
                                        MSG_LEN_TYPE& unTmpDataLen)
{
    if ( (pBuff == NULL) || (pHead == NULL))
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ParseClientStream Input param failed.");
    }

    //小于最小长度继续接收
    if ( nRecvAllLen < MSG_HEAD_LEN)
    {
        MY_ASSERT_STR(0,return 1,"the package len is less than base len ,receive len %d",nRecvAllLen);
    }

    char* pTemp = (char*)(*pBuff);
    MSG_LEN_TYPE unRecvLen = 0;
    MSG_LEN_TYPE unTmpUseLen = 0;
    //取出包的总长度
    memcpy(&unRecvLen,(void*)pTemp,sizeof(MSG_LEN_TYPE));
    unRecvLen = ntohs(unRecvLen);
    //客户端上行包的总长度小于基本长度大于最大长度，不合法
    if (unRecvLen < MSG_HEAD_LEN || unRecvLen > MSG_MAX_LEN)
    {
        MY_ASSERT_STR(0,return -1,"the package len is illegal,the len %d",unRecvLen);
    }
    //接收数据的总长度小于包的总长度继续接收
    if(nRecvAllLen - unRecvLen  < 0)
    {
        MY_ASSERT_STR(0,return 1, "Receive client part data left len = %d,real len = %d",nRecvAllLen,unRecvLen);
    }

    //数据指针向后移动指向未读取位置
    pTemp += sizeof(MSG_LEN_TYPE);
    unTmpUseLen += sizeof(MSG_LEN_TYPE);

    //序列码
    unsigned short tTmpSerial = 0;
    memcpy(&tTmpSerial,(void*)pTemp,sizeof(unsigned short));
    pTemp += sizeof(unsigned short);
    unTmpUseLen += sizeof(unsigned short);

    //应答码
    unsigned short tTmpSeq = 0;
    memcpy(&tTmpSeq,(void*)pTemp,sizeof(unsigned short));
    pTemp += sizeof(unsigned short);
    unTmpUseLen += sizeof(unsigned short);

    //消息指令编号
    MSG_CMD_TYPE tTmpCmd = 0;
    memcpy(&tTmpCmd,(void*)pTemp,sizeof(MSG_CMD_TYPE));
    tTmpCmd = ntohs(tTmpCmd);
    pTemp += sizeof(MSG_CMD_TYPE);
    unTmpUseLen += sizeof(MSG_CMD_TYPE);  

    pHead->set_serial(tTmpSerial);
    pHead->set_seq(tTmpSeq);
    pHead->set_cmd(tTmpCmd);

    //数据总长度
    unTmpDataLen = unRecvLen - unTmpUseLen;
    (*pBuff) = (void*) pTemp;
    return 0;
}

int ClientCommEngine::ConverToGameStream(const void * pBuff,
                                MSG_LEN_TYPE& unBuffLen,
                                const void *pDataBuff,
                                MSG_LEN_TYPE& unDataLen,
                                MesHead* pHead)
{
	if ((pBuff == NULL) || (pDataBuff == NULL) || pHead == NULL )
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConverGateToGame Input param failed.");
	}

	char* pTemp = (char*)pBuff;
    
    //预留总长度
    MSG_LEN_TYPE unLength = 0;
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);
    //预留8字节对齐长度
    pTemp += sizeof(unsigned short);
    unLength += sizeof(unsigned short);

    // MesHead长度
	*(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE) pHead->ByteSize();
	pTemp += sizeof(MSG_LEN_TYPE);
	unLength += sizeof(MSG_LEN_TYPE);

	// MesHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false)
	{
		MY_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pHead->GetCachedSize();
	unLength += pHead->GetCachedSize();

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

    // data长度
    *(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE) unDataLen;
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);

    // 拷贝消息到发送缓冲区
    memcpy(pTemp, (char*)pDataBuff, unDataLen);
    pTemp += unDataLen;
    unLength += unDataLen;

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
    *(unsigned int*) pTemp = unLength;
    pTemp += sizeof(unsigned short);
    //序列话8字节对齐长度
    *(unsigned short*) pTemp = iTmpAddlen;

    unBuffLen = unLength;
    return 0;
}


int ClientCommEngine::ConvertToGameStream(const void * pBuff,
                                            MSG_LEN_TYPE& unBuffLen,
                                            MesHead* pHead,
                                            Message* pMsg)
{
    if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL)
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
    }

    char* pTemp = (char*)pBuff;

    MSG_LEN_TYPE unLength = 0;
    //预留总长度
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);
    //预留8字节对齐长度
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);

    // MesHead长度
    *(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE) pHead->ByteSize();
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);

    // MesHead
    if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false)
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
    }
    pTemp += pHead->GetCachedSize();
    unLength += pHead->GetCachedSize();

    if (pMsg != NULL)
    {
        char tEncryBuff[MAX_PACKAGE_LEN] = {0};
        char* tpEncryBuff = &tEncryBuff[0];
        MSG_LEN_TYPE iMsgParaLen = MAX_PACKAGE_LEN;
        if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false)
        {
            MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
        }
        // 消息长度
        *(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE)iMsgParaLen;
        pTemp += sizeof(MSG_LEN_TYPE);
        unLength += sizeof(MSG_LEN_TYPE);

        //拷贝消息到缓冲区
        memcpy(pTemp,tpEncryBuff,iMsgParaLen);
        pTemp += iMsgParaLen;
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
    return 0;
}

int ClientCommEngine::ConvertToGateStream(const void * pBuff,
                                        MSG_LEN_TYPE& unBuffLen,
                                        MesHead* pHead,
                                        Message* pMsg)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL)
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char* pTemp = (char*)pBuff;

    MSG_LEN_TYPE unLength = 0;
    //预留总长度
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);
    //预留8字节对齐长度
    pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);

	// MesHead长度
	*(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE) pHead->ByteSize();
	pTemp += sizeof(MSG_LEN_TYPE);
	unLength += sizeof(MSG_LEN_TYPE);

	// MesHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false)
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pHead->GetCachedSize();
	unLength += pHead->GetCachedSize();

    char tEncryBuff[MAX_PACKAGE_LEN] = {0};
    char* tpEncryBuff = &tEncryBuff[0];
    MSG_LEN_TYPE iMsgParaLen = MAX_PACKAGE_LEN;
    if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false)
    {
        MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
    }
    
    // if (pHead->isencry())  
    // {
    //     //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
    //     CAes tmpAes;
    //     tmpAes.init(tpKey,16);
    //     tpEncryBuff = tmpAes.encrypt(tpEncryBuff,pMsg->ByteSize(), iMsgParaLen);
    // }
    // else
    // {
    //     iMsgParaLen = pMsg->ByteSize();
    // }

    // 消息长度
	*(MSG_LEN_TYPE*)pTemp = (MSG_LEN_TYPE)iMsgParaLen;
	pTemp += sizeof(MSG_LEN_TYPE);
    unLength += sizeof(MSG_LEN_TYPE);
    //应答码
	*(unsigned short*)pTemp = (unsigned short)pHead->serial();
	pTemp += sizeof(unsigned short);
    unLength += sizeof(unsigned short);
    //序列码
	*(unsigned short*)pTemp = (unsigned short)pHead->seq();
	pTemp += sizeof(unsigned short);
    unLength += sizeof(unsigned short);
    //命令号
	*(unsigned short*)pTemp = (unsigned short)pHead->cmd();
	pTemp += sizeof(unsigned short);
    unLength += sizeof(unsigned short);

    memcpy(pTemp,tpEncryBuff,iMsgParaLen);
	pTemp += iMsgParaLen;
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
    return 0;
}

int ClientCommEngine::ConvertStreamToMessage(const void* pBuff,
                                                MSG_LEN_TYPE unBuffLen,
                                                MesHead* pHead,
                                                CMessage* pMessage,
                                                CFactory* pMsgFactory,
                                                int* unOffset)
{
    if ((pBuff == NULL) || (pHead == NULL))
	{
		MY_ASSERT_STR(0,return -1, "ClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}

    char* pbyTmpBuff = (char*)pBuff;
    unOffset = 0;
    //取出数据总长度
    MSG_LEN_TYPE unTmpTotalLen = *(MSG_LEN_TYPE*) pbyTmpBuff;
    pbyTmpBuff += sizeof(MSG_LEN_TYPE);		// 指针指向数据处
    unOffset += sizeof(MSG_LEN_TYPE);

    // 总长度不匹配
    if (unTmpTotalLen != unBuffLen)
    {
        MY_ASSERT_STR(0,return -1,"the package len is not equal to datalen ,package len %d,data len",unBuffLen,unTmpTotalLen);
    }

    MSG_LEN_TYPE unTmpUseLen = 0;
	// 字节对齐补充长度（采用8字节对齐）
    unsigned short unTmpAddlLen = *(unsigned short*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned short);
    unTmpUseLen += sizeof(unsigned short);

    //扔掉字节对齐长度(字节补齐部分早尾部)
    unTmpUseLen += unTmpAddlLen;

	// MesHead长度
    MSG_LEN_TYPE tmpHeadLen = *(MSG_LEN_TYPE*) pbyTmpBuff;
    pbyTmpBuff += sizeof(MSG_LEN_TYPE);
    unTmpUseLen += sizeof(MSG_LEN_TYPE);

    //反序列化失败
    if (pHead->ParseFromArray(pbyTmpBuff,tmpHeadLen) == false)
    {
        MY_ASSERT_STR(0,return -1,"MesHead ParseFromArray failed");
    }
    pbyTmpBuff += tmpHeadLen;
    unTmpUseLen += tmpHeadLen;

    //消息长度
    MSG_LEN_TYPE tmpDataLen = *(MSG_LEN_TYPE*) pbyTmpBuff;
    pbyTmpBuff += sizeof(MSG_LEN_TYPE);
    unTmpUseLen += sizeof(MSG_LEN_TYPE);

    if (!unOffset)
    {
        *(unOffset) = unTmpUseLen;
    }

    //序列化消息
    if ((unTmpTotalLen - unTmpUseLen) > 0 && pMsgFactory != NULL && pMessage != NULL)
    {
        // MessagePara
        // 使用消息工厂
        Message* tpMsgPara = pMsgFactory->CreateMessage(pHead->cmd());
        if (tpMsgPara == NULL)
        {
            MY_ASSERT_STR(0,return -1,"Message CreateMessage failed");
        }

        if (tpMsgPara->ParseFromArray(pbyTmpBuff,tmpDataLen) == false)
        {
            pMessage->~Message();
            MY_ASSERT_STR(0,return -1,"Message ParseFromArray failed");
        }
        pMessage->set_msgpara((unsigned long)tpMsgPara);
    }
    return 0;
}  
