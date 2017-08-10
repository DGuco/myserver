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

int ClientCommEngine::ConvertStreamToClientMsg(char* pBuff,
                                             unsigned short nRecvAllLen,
                                             CClientMessage* pMsg,
                                             CFactory* pMsgFactory,
                                             bool bEncrypt,
                                             const unsigned char* pEncrypt)
{
    if (
            (pBuff == NULL) ||
            (pMsg == NULL) ||
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

    char* pTemp1 = pBuff;
    unsigned int unRecvLen = 0;
    //取出包的总长度
    memcpy(&unRecvLen,(void*)pTemp1,sizeof(unsigned int));
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


    unsigned int tmpRecvLen = unRecvLen;
    //数据指针向后移动指向未读取位置
    pTemp1 += sizeof(unsigned int);
    tmpRecvLen -= sizeof(unsigned int);

    // 序列号长度
    unsigned short tTmpSeq = 0;
    memcpy(&tTmpSeq,(void*)pTemp1,sizeof(unsigned short));
    pTemp1 += sizeof(unsigned short);
    tmpRecvLen -= sizeof(unsigned short);

    // protobuf版本
    unsigned char tTmpProbufVersion = 0;
    memcpy(&tTmpProbufVersion,(void*)pTemp1,sizeof(unsigned char));
    pTemp1 += sizeof(unsigned char);
    tmpRecvLen -= sizeof(unsigned char);

    // 是否加密
    unsigned char tTmpIsEncry = 0;
    memcpy(&tTmpIsEncry,(void*)pTemp1,sizeof(unsigned char));
    pTemp1 += sizeof(unsigned char);
    tmpRecvLen -= sizeof(unsigned char);

    //消息指令编号
    unsigned short tTmpCmd = 0;
    memcpy(&tTmpCmd,(void*)pTemp1,sizeof(unsigned short));
    tTmpCmd = ntohs(tTmpCmd);
    pTemp1 += sizeof(unsigned short);
    tmpRecvLen -= sizeof(unsigned short);

    //组织消息发送给gameserver
    C2SHead *tmpHead = pMsg->mutable_msghead();
    tmpHead->Clear();
    tmpHead->set_cmd(tTmpCmd);
    tmpHead->set_seq(tTmpSeq);
    tmpHead->set_isencry(tTmpIsEncry);

    // 开始消息解密
    unsigned char tEncryBuff[MAX_PACKAGE_LEN] = {0};
    unsigned char* tpEncryBuff = &tEncryBuff[0];
    unsigned short tOutLen = MAX_PACKAGE_LEN;
    if (tTmpIsEncry)
    {
        // 加密的消息处理
        int tDecLen = tOutLen;
        //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
        CAes tmpAes;
        tmpAes.init(tpKey,16);
        int outlen;
        tpEncryBuff = (unsigned char*)tmpAes.decrypt((const char*)pTemp1,tmpRecvLen,tDecLen);
        tOutLen = tDecLen;
    }
    else
    {
        // 未加密的消息处理
        tpEncryBuff = (unsigned char*)pTemp1;
        tOutLen = tmpRecvLen;
    }
    pMsg->set_msgparas(tpEncryBuff,tmpRecvLen);
    return 0;
}

int ClientCommEngine::ConvertStreamToClientMsg(char* pBuff,
                                    unsigned short unBuffLen,
                                    C2SHead* pHead,
                                    Message* pMessage,
                                    CFactory* pMsgFactory = NULL)
{
    if ((pBuff == NULL) || (pHead == NULL)|| (pMessage == NULL))
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}
    
    char* pbyTmpBuff = pBuff;
    int iTmpLen = unBuffLen;
    //取出数据总长度
    unsigned short unTmpTotalLen = *(unsigned short*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned short);		// 指针指向数据处
    iTmpLen -= sizeof(unsigned short);			// 从长度减少一个len的长度

    // 总长度不匹配
    if (unTmpTotalLen != unBuffLen)
    {
        return -1;
    }

	// 字节对齐补充长度（采用8字节对齐）
    unsigned short unTmpAddlLen = *(unsigned short*) pbyTmpBuff;
    pbyTmpBuff += sizeof(unsigned short);
    iTmpLen -= sizeof(unsigned short);	

    //扔掉字节对齐长度
    iTmpLen -= unTmpAddlLen;

    CClientMessage tmpClientMessage;
    //反序列化失败
    if (tmpClientMessage.ParseFromArray(pbyTmpBuff,iTmpLen) == false)
    {
        return -1;
    }

    C2SHead tmpHead = tmpClientMessage.msghead();
    pHead->set_cmd(tmpHead.cmd());
    pHead->set_seq(tmpHead.seq());
    pHead->set_isencry(tmpHead.isencry());
    CSocketInfo tmpSocketInfo = tmpHead.socketinfos();
    pHead->mutable_socketinfos()->set_socketid(tmpSocketInfo.socketid());
    pHead->mutable_socketinfos()->set_createtime(tmpSocketInfo.createtime());
    pHead->mutable_socketinfos()->set_state(tmpSocketInfo.state());

    //如果有消息
    if (iTmpLen > 0 && pMsgFactory != NULL)
    {
        pMessage = pMsgFactory->CreateMessage(tmpHead.cmd());
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

int ClientCommEngine::ConvertClientMessagedToStream(unsigned char * pBuff,
							  unsigned short& unBuffLen,
							  CClientMessage* pMsg,
							  bool bEncrypt = false,
							  const unsigned char* pEncrypt = ClientCommEngine::tpKey)
{
	if ((pBuff == NULL) || (pMsg == NULL) )
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	unsigned char* pTemp = pBuff;

    unsigned int unLength = 0;
    //预留总长度
    pTemp += sizeof(short);
    unLength += sizeof(short);
    //预留8字节对齐长度
    pTemp += sizeof(short);
    unLength += sizeof(short);
    //序列化CClientMessage
    if (pMsg->SerializeToArray(pTemp,pMsg->ByteSize()) != true)
    {
        LOG_ERROR("default", "CTCPCtrl::RecvClientData error,pbTmpTcpHead SerializeToArray error");
        return -1;
    }

    pTemp += pMsg->GetCachedSize();
    unLength += pMsg->GetCachedSize();

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
    pTemp = pBuff;
    //序列话消息总长度
    *(short*) pTemp = unLength;
    pTemp += sizeof(short);
    //序列话8字节对齐长度
    *(short*) pTemp = iTmpAddlen;

    unBuffLen = unLength;
}

int ClientCommEngine::ConvertGameServerMessageToStream(unsigned char * pBuff,
							  unsigned short& unBuffLen,
							  CGameServerMessage* pMsg)
{
	if ((pBuff == NULL) || (pMsg == NULL) )
	{
		MY_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	unsigned char* pTemp = pBuff;

    unsigned int unLength = 0;
    //预留总长度
    pTemp += sizeof(short);
    unLength += sizeof(short);
    //预留8字节对齐长度
    pTemp += sizeof(short);
    unLength += sizeof(short);
    //序列化CClientMessage
    if (pMsg->SerializeToArray(pTemp,pMsg->ByteSize()) != true)
    {
        LOG_ERROR("default", "CTCPCtrl::RecvClientData error,pbTmpTcpHead SerializeToArray error");
        return -1;
    }

    pTemp += pMsg->GetCachedSize();
    unLength += pMsg->GetCachedSize();

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
    pTemp = pBuff;
    //序列话消息总长度
    *(short*) pTemp = unLength;
    pTemp += sizeof(short);
    //序列话8字节对齐长度
    *(short*) pTemp = iTmpAddlen;

    unBuffLen = unLength;
}
