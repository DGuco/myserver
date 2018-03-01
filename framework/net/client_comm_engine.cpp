/*
 * client_comm_engine.cpp
 *
 *  Created on: 2016年11月26日
 *      Author: DGuco
 */

#include "client_comm_engine.h"
#include "my_assert.h"

unsigned char CClientCommEngine::tKey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 2, 2, 4, 4, 5, 6};

unsigned char *CClientCommEngine::tpKey = &tKey[0];

void CClientCommEngine::CopyMesHead(MesHead *from, MesHead *to)
{
	if (from && to) {
		for (int i = 0; i < from->socketinfos_size(); ++i) {
			CSocketInfo *pSocketInfo = to->mutable_socketinfos()->Add();
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

int CClientCommEngine::ParseClientStream(const void **pBuff,
										 PACK_LEN &nLen,
										 MesHead *pHead)
{
	if ((pBuff == NULL) || (pHead == NULL)) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ParseClientStream Input param failed.");
	}

	//小于最小长度(包头长度 - 包总长度所占字节长度)
	if (nLen < MSG_HEAD_LEN - sizeof(PACK_LEN)) {
		MY_ASSERT_STR(0, return -1, "the package len is less than base len ,receive len {}", nLen);
	}

	char *pTemp = (char *) (*pBuff);

	//序列码
	unsigned short tTmpSerial = 0;
	memcpy(&tTmpSerial, (void *) pTemp, sizeof(unsigned short));
	pTemp += sizeof(unsigned short);
	nLen -= sizeof(unsigned short);

	//应答码
	unsigned short tTmpSeq = 0;
	memcpy(&tTmpSeq, (void *) pTemp, sizeof(unsigned short));
	pTemp += sizeof(unsigned short);
	nLen -= sizeof(unsigned short);

	//消息指令编号
	MSG_CMD tTmpCmd = 0;
	memcpy(&tTmpCmd, (void *) pTemp, sizeof(MSG_CMD));
	tTmpCmd = ntohs(tTmpCmd);
	pTemp += sizeof(MSG_CMD);
	nLen -= sizeof(MSG_CMD);

	pHead->set_serial(tTmpSerial);
	pHead->set_seq(tTmpSeq);
	pHead->set_cmd(tTmpCmd);

	(*pBuff) = (void *) pTemp;
	return 0;
}

int CClientCommEngine::ConverToGameStream(const void *pBuff,
										  PACK_LEN &unBuffLen,
										  const void *pDataBuff,
										  PACK_LEN &unDataLen,
										  MesHead *pHead)
{
	if ((pBuff == NULL) || (pDataBuff == NULL) || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConverGateToGame Input param failed.");
	}

	char *pTemp = (char *) pBuff;

	//预留总长度
	PACK_LEN unLength = 0;
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);
	//预留8字节对齐长度
	pTemp += sizeof(unsigned short);
	unLength += sizeof(unsigned short);

	// MesHead长度
	*(PACK_LEN *) pTemp = (PACK_LEN) pHead->ByteSize();
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// MesHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false) {
		MY_ASSERT_STR(0, return -2, "CClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
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
	*(PACK_LEN *) pTemp = (PACK_LEN) unDataLen;
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// 拷贝消息到发送缓冲区
	memcpy(pTemp, (char *) pDataBuff, unDataLen);
	pTemp += unDataLen;
	unLength += unDataLen;

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
		//将字节对齐部分置为0
		memset(pTemp, 0, iTmpAddlen);
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	pTemp = (char *) pBuff;
	//序列话消息总长度
	*(unsigned int *) pTemp = unLength;
	pTemp += sizeof(unsigned short);
	//序列话8字节对齐长度
	*(unsigned short *) pTemp = iTmpAddlen;

	unBuffLen = unLength;
	return 0;
}

int CClientCommEngine::ConvertToGameStream(const void *pBuff,
										   PACK_LEN &unBuffLen,
										   MesHead *pHead,
										   Message *pMsg)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char *pTemp = (char *) pBuff;

	PACK_LEN unLength = 0;
	//预留总长度
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);
	//预留8字节对齐长度
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// MesHead长度
	*(PACK_LEN *) pTemp = (PACK_LEN) pHead->ByteSize();
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// MesHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pHead->GetCachedSize();
	unLength += pHead->GetCachedSize();

	if (pMsg) {
		char tEncryBuff[MAX_PACKAGE_LEN] = {0};
		char *tpEncryBuff = &tEncryBuff[0];
		PACK_LEN iMsgParaLen = MAX_PACKAGE_LEN;
		if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false) {
			MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
		}
		// 消息长度
		*(PACK_LEN *) pTemp = (PACK_LEN) iMsgParaLen;
		pTemp += sizeof(PACK_LEN);
		unLength += sizeof(PACK_LEN);

		//拷贝消息到缓冲区
		memcpy(pTemp, tpEncryBuff, iMsgParaLen);
		pTemp += iMsgParaLen;
		unLength += iMsgParaLen;
	}

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
		//将字节对齐部分置为0
		memset(pTemp, 0, iTmpAddlen);
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	pTemp = (char *) pBuff;
	//序列话消息总长度
	*(short *) pTemp = unLength;
	pTemp += sizeof(short);
	//序列话8字节对齐长度
	*(short *) pTemp = iTmpAddlen;

	unBuffLen = unLength;
	return 0;
}

int CClientCommEngine::ConvertToGateStream(const void *pBuff,
										   PACK_LEN &unBuffLen,
										   MesHead *pHead,
										   Message *pMsg)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char *pTemp = (char *) pBuff;

	PACK_LEN unLength = 0;
	//预留总长度
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);
	//预留8字节对齐长度
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// MesHead长度
	*(PACK_LEN *) pTemp = (PACK_LEN) pHead->ByteSize();
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);

	// MesHead
	if (pHead->SerializeToArray(pTemp, unBuffLen - unLength) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
	}
	pTemp += pHead->GetCachedSize();
	unLength += pHead->GetCachedSize();

	char tEncryBuff[MAX_PACKAGE_LEN] = {0};
	char *tpEncryBuff = &tEncryBuff[0];
	PACK_LEN iMsgParaLen = MAX_PACKAGE_LEN;
	if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
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
	*(PACK_LEN *) pTemp = (PACK_LEN) iMsgParaLen;
	pTemp += sizeof(PACK_LEN);
	unLength += sizeof(PACK_LEN);
	//应答码
	*(unsigned short *) pTemp = (unsigned short) pHead->serial();
	pTemp += sizeof(unsigned short);
	unLength += sizeof(unsigned short);
	//序列码
	*(unsigned short *) pTemp = (unsigned short) pHead->seq();
	pTemp += sizeof(unsigned short);
	unLength += sizeof(unsigned short);
	//命令号
	*(unsigned short *) pTemp = (unsigned short) pHead->cmd();
	pTemp += sizeof(unsigned short);
	unLength += sizeof(unsigned short);

	memcpy(pTemp, tpEncryBuff, iMsgParaLen);
	pTemp += iMsgParaLen;
	unLength += iMsgParaLen;

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
		//将字节对齐部分置为0
		memset(pTemp, 0, iTmpAddlen);
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	pTemp = (char *) pBuff;
	//序列话消息总长度
	*(short *) pTemp = unLength;
	pTemp += sizeof(short);
	//序列话8字节对齐长度
	*(short *) pTemp = iTmpAddlen;

	unBuffLen = unLength;
	return 0;
}

int CClientCommEngine::ConvertStreamToMessage(const void *pBuff,
											  PACK_LEN unBuffLen,
											  CMessage *pMessage,
											  CFactory *pMsgFactory,
											  int *unOffset)
{
	if ((pBuff == NULL)) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}

	char *pbyTmpBuff = (char *) pBuff;
	PACK_LEN unTmpUseLen = 0;
	//取出数据总长度
	PACK_LEN unTmpTotalLen = *(PACK_LEN *) pbyTmpBuff;
	pbyTmpBuff += sizeof(PACK_LEN);        // 指针指向数据处
	unTmpUseLen += sizeof(PACK_LEN);

	// 总长度不匹配
	if (unTmpTotalLen != unBuffLen) {
		MY_ASSERT_STR(0,
					  return -1,
					  "the package len is not equal to datalen {} ,package len {},data len",
					  unBuffLen,
					  unTmpTotalLen);
	}

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short unTmpAddlLen = *(unsigned short *) pbyTmpBuff;
	pbyTmpBuff += sizeof(unsigned short);
	unTmpUseLen += sizeof(unsigned short);

	//扔掉字节对齐长度(字节补齐部分早尾部)
	unTmpUseLen += unTmpAddlLen;

	// MesHead长度
	PACK_LEN tmpHeadLen = *(PACK_LEN *) pbyTmpBuff;
	pbyTmpBuff += sizeof(PACK_LEN);
	unTmpUseLen += sizeof(PACK_LEN);

	MesHead *pHead = pMessage->mutable_msghead();
	//反序列化失败
	if (pHead->ParseFromArray(pbyTmpBuff, tmpHeadLen) == false) {
		MY_ASSERT_STR(0, return -1, "MesHead ParseFromArray failed");
	}
	pbyTmpBuff += tmpHeadLen;
	unTmpUseLen += tmpHeadLen;

	//消息长度
	PACK_LEN tmpDataLen = *(PACK_LEN *) pbyTmpBuff;
	pbyTmpBuff += sizeof(PACK_LEN);
	unTmpUseLen += sizeof(PACK_LEN);

	if (unOffset) {
		*(unOffset) = unTmpUseLen;
	}

	//序列化消息,消息
	if ((unTmpTotalLen - unTmpUseLen) > 0 && pMsgFactory != NULL && pMessage != NULL) {
		// MessagePara
		// 使用消息工厂
		Message *tpMsgPara = pMsgFactory->CreateMessage(pHead->cmd());
		if (tpMsgPara == NULL) {
			MY_ASSERT_STR(0, return -1, "Message CreateMessage failed");
		}

		if (tpMsgPara->ParseFromArray(pbyTmpBuff, tmpDataLen) == false) {
			// 因为使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
			// 并且是非线程安全的
			pMessage->~Message();
			MY_ASSERT_STR(0, return -1, "Message ParseFromArray failed");
		}
		pMessage->set_msgpara((unsigned long) tpMsgPara);
	}
	return 0;
}  
