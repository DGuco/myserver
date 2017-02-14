/*
 * client_comm_engine.cpp
 *
 *  Created on: 2013年9月5日
 *      Author: yaoqiang
 */

#include "client_comm_engine.h"
#include "base.h"
#include "yq_assert.h"
#include "oi_tea.h"

unsigned char ClientCommEngine::tKey[16] = {7,21,111,121,57,69,51,121,57,111,20,223,125,154,19,64};
unsigned char* ClientCommEngine::tpKey = &tKey[0];

void pbmsg_settcphead(CTcpHead& rHead, int iSrcFE, int iSrcID, int iDstFE, int iDstID, time_t tTimestamp, EGateCmd eCmd)
{
	rHead.set_srcfe(iSrcFE);
	rHead.set_srcid(iSrcID);
	rHead.set_dstfe(iDstFE);
	rHead.set_dstid(iDstID);
	rHead.set_opflag(eCmd);
	rHead.set_timestamp(tTimestamp);
}

// [客户端实际上下行消息结构]
// [消息说明]
// |字段名称 					|长度											|描述
// |client总长度 				|2字节											|
// |8字节对齐补充长度 	|2字节											|
// |CMessageHead长度	|2字节											|消息头长度
// |CMessageHead			|CMessageHead.ByteSize()			|消息头
// |MsgPara长度				|2字节											|实际消息内容长度
// |MsgPara						|MsgPara.ByteSize()						|实际消息内容
// |字节对齐补充内容 		|8-(client总长度 % 8)					|该内容无任何意义，仅用于消息8字节对齐


// [服务器内部总消息组成](仅在服务器内部传输时,CTcpHead后面的可以为空)
// [消息说明]
// |字段名称 					|长度												|描述
// |总长度 						|2字节												|
// |8字节对齐补充长度 	|2字节												|
// |CTcpHead长度 			|2字节												|
// |CTcpHead 					|CTcpHead.ByteSize()						|
// |client总长度 				|2字节												|
// |8字节对齐补充长度 	|2字节												|
// |CMessageHead长度	|2字节												|消息头长度
// |CMessageHead			|CMessageHead.ByteSize()				|消息头
// |MsgPara长度				|2字节												|实际消息内容长度
// |MsgPara						|MsgPara.ByteSize()							|实际消息内容
// |字节对齐补充内容 		|8-(client总长度 % 8)						|该内容无任何意义，仅用于消息8字节对齐
// |字节对齐补充内容 		|8-(总长度 % 8)								|该内容无任何意义，仅用于消息8字节对齐


// 反序列化客户端上下行的消息内容
int ClientCommEngine::ConvertStreamToMsg(const void* pBuff, unsigned short unBuffLen, CMessage* pMsg, CFactory* pMsgFactory, bool bEncrypt, const unsigned char* pEncrypt)
{
	if (
			(pBuff == NULL) ||
			(unBuffLen < (sizeof(unsigned short) * 4)) ||
			(pMsg == NULL) ||
			(pEncrypt == NULL)
	)
	{
		YQ_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToMsg Input param failed.");
	}

	char* tpBuff = (char*) pBuff;
	unsigned short tTmpLen = 0;

	// client总长度
	unsigned short tTotalClientLen = *(unsigned short*)tpBuff;

	// 验证包长与实际长度是否匹配
	if (tTotalClientLen != unBuffLen)
	{
		YQ_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertStreamToMsg tTotalClientLen = %d unequal to unBuffLen = %d.", tTotalClientLen, unBuffLen);
	}
	tpBuff += sizeof(unsigned short);
	tTotalClientLen -= sizeof(unsigned short);

	// 字节对齐补充长度（采用8字节对齐）
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalClientLen -= sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tTmpLen >= 8)
	{
		YQ_ASSERT_STR(0, return -3, "ClientCommEngine::ConvertStreamToMsg tAddLen = %d impoosibility.", tTmpLen);
	}
	// 直接扔掉8字节补齐的长度
	tTotalClientLen -= tTmpLen;

	// CMessageHead长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalClientLen -= sizeof(unsigned short);

	// CMessageHead
	CMessageHead* pMsgHead = pMsg->mutable_msghead();
	if (pMsgHead->ParseFromArray(tpBuff, tTmpLen) == false)
	{
		YQ_ASSERT_STR(0, return -4, "ClientCommEngine::ConvertStreamToMsg CMessageHead ParseFromArray failed.");
	}
	if (pMsgHead->ByteSize() != tTmpLen)
	{
		YQ_ASSERT_STR(0, return -5, "ClientCommEngine::ConvertStreamToMsg pMsgHead ByteSize = %d unequal to tTmpLen = %d..",
			pMsgHead->ByteSize(), tTmpLen);
	}
	tpBuff += tTmpLen;
	tTotalClientLen -= tTmpLen;

	if (pMsgFactory)
	{
		// MessagePara长度
		tTmpLen = *(unsigned short*)tpBuff;
		tpBuff += sizeof(unsigned short);
		tTotalClientLen -= sizeof(unsigned short);

		if (tTmpLen != tTotalClientLen)
		{
			YQ_ASSERT_STR(0, return -6, "ClientCommEngine::ConvertStreamToMsg tTmpLen %d unequal to tTotalClientLen %d.",
				tTmpLen, tTotalClientLen);
		}

		// 有消息工厂则向下解析
		// 开始消息解密
		unsigned char tEncryBuff[MAX_PACKAGE_LEN] = {0};
		unsigned char* tpEncryBuff = &tEncryBuff[0];
		unsigned short tOutLen = MAX_PACKAGE_LEN;

		if (bEncrypt)
		{
			// 加密的消息处理
			int tDecLen = tOutLen;
			DecryptData(EncryptType1, pEncrypt, (unsigned char*)tpBuff, (int)tTotalClientLen, tpEncryBuff, &tDecLen);
			tOutLen = tDecLen;
		}
		else
		{
			// 未加密的消息处理
			tpEncryBuff = (unsigned char*)tpBuff;
			tOutLen = tTotalClientLen;
		}

		// 解密后的长度不能超过最大MAX_PACKAGE_LEN,也不能比输入的未加密前长度长
		if (tOutLen >= MAX_PACKAGE_LEN || tOutLen > tTotalClientLen)
		{
			YQ_ASSERT_STR(0, return -7, "ClientCommEngine::ConvertStreamToMsg DecryptData failed, tOutLen = %d, Input Length = %d.", tOutLen, tTotalClientLen);
		}

		// MessagePara
		// 使用消息工厂
		Message* tpMsgPara = pMsgFactory->CreateMessage(pMsg->msghead().messageid());
		if (tpMsgPara == NULL)
		{
			YQ_ASSERT_STR(0, return -8, "ClientCommEngine::ConvertStreamToMsg CMessageFactory can't create msg id = %d.", pMsg->msghead().messageid());
		}

		if (tpMsgPara->ParseFromArray(tpEncryBuff, tOutLen) != true)
		{
			// 使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
			tpMsgPara->~Message();
			YQ_ASSERT_STR(0, return -9, "ClientCommEngine::ConvertStreamToMsg CMessage.msgpara ParseFromArray failed.");
		}

		pMsg->set_msgpara((unsigned long)tpMsgPara);
	}

	return 0;
}

int ClientCommEngine::ConvertStreamToMsg(const void* pBuff, unsigned short unBuffLen, unsigned short& rOffset, CTcpHead* pTcpHead)
{
	if (	(pBuff == NULL) ||
			(unBuffLen < (sizeof(unsigned short) * 2)) ||
			(pTcpHead == NULL)
	)
	{
		YQ_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertStreamToMsg Input param failed.");
	}

	char* tpBuff = (char*) pBuff;
	unsigned short tTmpLen = 0;

	// 总长度
	unsigned short tTotalLen = *(unsigned short*)tpBuff;
	if (tTotalLen != unBuffLen)
	{
		YQ_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertStreamToMsg tTotalLen = %d unequal to unBuffLen = %d.", tTotalLen, unBuffLen);
	}
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);
	rOffset += sizeof(unsigned short);

	// 字节对齐补充长度（采用8字节对齐）
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);
	rOffset += sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tTmpLen >= 8)
	{
		YQ_ASSERT_STR(0, return -3, "ClientCommEngine::ConvertStreamToMsg tAddLen = %d impoosibility.", tTmpLen);
	}
	// 直接扔掉8字节补齐的长度
	tTotalLen -= tTmpLen;

	// CTcpHead长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);
	rOffset += sizeof(unsigned short);

	// CTcpHead
	if (pTcpHead->ParseFromArray(tpBuff, tTmpLen) == false)
	{
		YQ_ASSERT_STR(0, return -3, "ClientCommEngine::ConvertStreamToMsg CTcpHead ParseFromArray falied.");
	}

	if (pTcpHead->ByteSize() != tTmpLen)
	{
		YQ_ASSERT_STR(0, return -4, "ClientCommEngine::ConvertStreamToMsg pTcpHead ByteSize = %d unequal to tTmpLen = %d.",
			pTcpHead->ByteSize(), tTmpLen);
	}
	tpBuff += tTmpLen;
	tTotalLen -= tTmpLen;
	rOffset += tTmpLen;

	return tTotalLen;
}

// 序列化客户端上下行的消息内容
int ClientCommEngine::ConvertMsgToStream(void* pBuff, unsigned short& unBuffLen, const CTcpHead* pTcpHead, CMessage* pMsg, bool bEncrypt, const unsigned char* pEncrypt)
{
	if (
			(pBuff == NULL) ||
			(pTcpHead == NULL) ||
			(unBuffLen < (pTcpHead->ByteSize() + sizeof(unsigned short) * 2))
	)
	{
		YQ_ASSERT_STR(0, return -1, "ClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	char* tpBuff = (char*) pBuff;

	// 预留总长度
	unsigned short tTotalLen = 0;
	tpBuff += sizeof(unsigned short);
	tTotalLen += sizeof(unsigned short);

	// 8字节对齐的长度
	tpBuff += sizeof(unsigned short);
	tTotalLen += sizeof(unsigned short);

	// CTcpHead长度
	*(unsigned short*)tpBuff = (unsigned short) pTcpHead->ByteSize();
	tpBuff += sizeof(unsigned short);
	tTotalLen += sizeof(unsigned short);

	// CTcpHead
	if (pTcpHead->SerializeToArray(tpBuff, unBuffLen - tTotalLen) == false)
	{
		YQ_ASSERT_STR(0, return -2, "ClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
	}
	tpBuff += pTcpHead->GetCachedSize();
	tTotalLen += pTcpHead->GetCachedSize();

	// 客户端消息总长度
	unsigned short tTotalClientLen = 0;
	char* tpClientBuff = tpBuff;
	// 有CMessage表示不是服务器内部消息
	if (pMsg)
	{
		// 预留客户端消息总长度
		char* pTotalClientLen = tpClientBuff;
		tpClientBuff += sizeof(unsigned short);
		tTotalClientLen += sizeof(unsigned short);

		// 预留8字节补齐长度
		char* pAddLen = tpClientBuff;
		tpClientBuff += sizeof(unsigned short);
		tTotalClientLen += sizeof(unsigned short);

		// CMessageHead长度
		*(unsigned short*)tpClientBuff = (unsigned short) pMsg->msghead().ByteSize();
		tpClientBuff += sizeof(unsigned short);
		tTotalClientLen += sizeof(unsigned short);

		// CMessageHead
		if (pMsg->msghead().SerializeToArray(tpClientBuff, unBuffLen - tTotalLen - tTotalClientLen) == false)
		{
			YQ_ASSERT_STR(0, return -3, "ClientCommEngine::ConvertMsgToStream CMessageHead SerializeToArray failed.");
		}
		tpClientBuff += pMsg->msghead().GetCachedSize();
		tTotalClientLen += pMsg->msghead().GetCachedSize();

		// 预留MsgPara长度
		char* pMsgParaLen = tpClientBuff;
		tpClientBuff += sizeof(unsigned short);
		tTotalClientLen += sizeof(unsigned short);

		// MsgPara
		unsigned char tEncryBuff[MAX_PACKAGE_LEN] = {0};
		unsigned char* tpEncryBuff = &tEncryBuff[0];
		unsigned short iMsgParaLen = MAX_PACKAGE_LEN;

		::google::protobuf::Message* pMsgPara = (::google::protobuf::Message*) pMsg->msgpara();
		if (pMsgPara == NULL)
		{
			YQ_ASSERT_STR(0, return -4, "ClientCommEngine::ConvertMsgToStream MsgPara is NULL.");
		}
		if (pMsgPara->SerializeToArray(tpEncryBuff, iMsgParaLen) == false)
		{
			YQ_ASSERT_STR(0, return -5, "ClientCommEngine::ConvertMsgToStream MsgPara SerializeToArray failed.");
		}
		// 消息加密
		if (bEncrypt)
		{
			// 加密的消息处理
			int tEncLen = unBuffLen - tTotalLen - tTotalClientLen;
			EncryptData(EncryptType1, pEncrypt, tpEncryBuff, pMsgPara->ByteSize(), (unsigned char*)tpClientBuff, &tEncLen);
			iMsgParaLen = tEncLen;
		}
		else
		{
			// 未加密的消息处理
			iMsgParaLen = pMsgPara->ByteSize();
			memcpy(tpClientBuff, tpEncryBuff, iMsgParaLen);
		}
		tpClientBuff += iMsgParaLen;
		tTotalClientLen += iMsgParaLen;

		// 8字节补齐
		unsigned short iAddLen = (tTotalClientLen % 8);
		if (iAddLen > 0)
		{
			iAddLen = (8 - iAddLen);
			memset(tpClientBuff, 0, iAddLen);
		}
		tpClientBuff += iAddLen;
		tTotalClientLen += iAddLen;

		// 各个长度
		*(unsigned short*)pTotalClientLen = tTotalClientLen;
		*(unsigned short*)pAddLen = iAddLen;
		*(unsigned short*)pMsgParaLen = iMsgParaLen;
	}

	// 总长度
	tTotalLen += tTotalClientLen;

	// 8字节对齐
	unsigned short iAddLen = (tTotalLen % 8);
	if (iAddLen > 0)
	{
		iAddLen = (8 - iAddLen);
		memset(tpClientBuff, 0, iAddLen);
	}
	tpClientBuff += iAddLen;
	tTotalLen += iAddLen;

	// 设置总长度
	tpBuff = (char*) pBuff;
	*(unsigned short*)tpBuff = tTotalLen;
	tpBuff += sizeof(unsigned short);

	// 设置8字节对齐长度
	*(unsigned short*) tpBuff = iAddLen;

	unBuffLen = tTotalLen;

	return 0;
}

void ClientCommEngine::EncryptData(short nAlgorithm,
				 const unsigned char* pbyKey,
				 const unsigned char* pbyIn,
				 int nInLength,
				 unsigned char* pbyOut,
				 int* pnOutLength)
{
	if ((pbyKey == NULL) || (pbyIn == NULL) || (pbyOut == NULL) || (pnOutLength == NULL))
	{
		return;
	}

	switch(nAlgorithm)
	{
		case 1:
		{
			oi_symmetry_encrypt(pbyIn, nInLength, pbyKey, pbyOut, pnOutLength);
			break;
		}
		case 2:
		{
			oi_symmetry_encrypt2(pbyIn, nInLength, pbyKey, pbyOut, pnOutLength);
			break;
		}
		default:
		{
			memcpy(pbyOut, pbyIn, nInLength);
			*pnOutLength = nInLength;
			break;
		}
	}
}


int ClientCommEngine::DecryptData(short nAlgorithm,
				const unsigned char* pbyKey,
				const unsigned char* pbyIn,
				int nInLength,
				unsigned char* pbyOut,
				int* pnOutLength)
{
	if ((pbyKey == NULL) || (pbyIn == NULL) || (pbyOut == NULL) || (pnOutLength == NULL))
	{
		return 0;
	}

	int nDecryResult = 0;

	switch(nAlgorithm)
	{
		case 1:
		{
			nDecryResult = oi_symmetry_decrypt(pbyIn, nInLength, pbyKey, pbyOut, pnOutLength);
			break;
		}
		case 2:
		{
			nDecryResult = oi_symmetry_decrypt2(pbyIn, nInLength, pbyKey, pbyOut, pnOutLength);
			break;
		}
		default:
		{
			memcpy(pbyOut, pbyIn, nInLength);
			*pnOutLength = nInLength;
			nDecryResult = 1;
			break;
		}
	}

	return nDecryResult;
}

//}	// namespace ClientCommEngine
