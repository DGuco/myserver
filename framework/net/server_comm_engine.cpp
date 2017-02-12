#include "server_comm_engine.h"
#include "../base/my_assert.h"
#include "message_interface.h"


void pbmsg_setproxy(CProxyHead* pHead, int iSrcFE, int iSrcID, int iDstFE, int iDstID, time_t tTimestamp, EProxyCmd eCmd)
{
	pHead->set_srcfe(iSrcFE);
	pHead->set_srcid(iSrcID);
	pHead->set_dstfe(iDstFE);
	pHead->set_dstid(iDstID);
	pHead->set_opflag(eCmd);
	pHead->set_timestamp(tTimestamp);
}

void pbmsg_setmessagehead(CMessage* pMsg, int iMsgID)
{
	CMessageHead* tpHead = pMsg->mutable_msghead();

	tpHead->set_messageid(iMsgID);
}

namespace ServerCommEngine
{
int ConvertStreamToMsg(const void* pBuff, unsigned short unBuffLen, CProxyHead* pProxyHead, CMessage* pMsg, CFactory* pMsgFactory)
{
	if ((pBuff == NULL) || (unBuffLen < (sizeof(unsigned short) * 3)) || (pProxyHead == NULL) || (pMsg == NULL) || ( pMsgFactory == NULL ) )
	{
		MY_ASSERT_STR(0, return -1, "ServerCommEngine::ConvertStreamToMsg Input param failed.");
	}
	char* tpBuff = (char*) pBuff;
	unsigned short tTmpLen = 0;

	// 总长度
	unsigned short tTotalLen = *(unsigned short*)tpBuff;

	// 验证包长与实际长度是否匹配
	if (tTotalLen != unBuffLen)
	{
		MY_ASSERT_STR(0, return -2, "ServerCommEngine::ConvertStreamToMsg tTotalLen = %d unequal to unBuffLen = %d.", tTotalLen, unBuffLen);
	}

	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short tAddLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tAddLen >= 8)
	{
		MY_ASSERT_STR(0, return -3, "ServerCommEngine::ConvertStreamToMsg tAddLen = %d impoosibility.", tAddLen);
	}
	// 直接抛掉补充长度
	tTotalLen -= tAddLen;

	// CProxyHead长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// CProxyHead
	if (pProxyHead->ParseFromArray(tpBuff, tTmpLen) != true)
	{
		MY_ASSERT_STR(0, return -4, "ServerCommEngine::ConvertStreamToMsg CProxyHead ParseFromArray failed.");
	}
	tpBuff += tTmpLen;
	tTotalLen -= tTmpLen;

	if (tTotalLen < 0)
	{
		MY_ASSERT_STR(0, return -5, "ServerCommEngine::ConvertStreamToMsg tTotalLen = %d impossibility.", tTotalLen);
	}

	if (tTotalLen == 0)
	{
		return 0;
	}

	// MessageHead长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// MessageHead
	if (pMsg->mutable_msghead()->ParseFromArray(tpBuff, tTmpLen) != true)
	{
		MY_ASSERT_STR(0, return -6, "ServerCommEngine::ConvertStreamToMsg CMessageHead ParseFromArray failed.");
	}
	tpBuff += tTmpLen;
	tTotalLen -= tTmpLen;

	// MessagePara长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	if (tTotalLen != tTmpLen)
	{
		// msgpara长度不匹配
		MY_ASSERT_STR(0, return -7, "ServerCommEngine::ConvertStreamToMsg failed, tTotalLen(%d) != tTmpLen.(%d)", tTotalLen, tTmpLen);
	}

	// MessagePara
	// 使用消息工厂
	Message* tpMsgPara = NULL;

	tpMsgPara = pMsgFactory->CreateMessage(pMsg->msghead().messageid());

	if (tpMsgPara == NULL)
	{
		MY_ASSERT_STR(0, return -8, "ServerCommEngine::ConvertStreamToMsg CMessageFactory can't create msg id = %d.", pMsg->msghead().messageid());
	}

	if (tpMsgPara->ParseFromArray(tpBuff, tTmpLen) != true)
	{
		// 使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
		tpMsgPara->~Message();
		MY_ASSERT_STR(0, return -9, "ServerCommEngine::ConvertStreamToMsg CMessage.msgpara ParseFromArray failed.");
	}

	pMsg->set_msgpara((unsigned long)tpMsgPara);

	return 0;
}

int ConvertMsgToStream(CProxyHead* pProxyHead, CMessage* pMsg, void* pBuff, unsigned short& unBuffLen)
{
	if ((pBuff == NULL) || (unBuffLen < 8) || (pProxyHead == NULL))
	{
		MY_ASSERT_STR(0, return -1, "ServerCommEngine::ConvertMsgToStream Input impossibility.");
	}

	char* tpBuff = (char*) pBuff;
	unsigned short tLen = 0;
	int tAddLen = 0;

	// 序列化总长度
	// 暂时跳过，后面补充
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化8字节对齐补充长度
	// 暂时跳过后面补充
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化CProxyHead长度
	*(unsigned short*) tpBuff = pProxyHead->ByteSize();
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化CProxyHead
	if (pProxyHead->SerializeToArray(tpBuff, unBuffLen - tLen) == false)
	{
		MY_ASSERT_STR(0, return -2, "ServerCommEngine::ConvertMsgToStream CProxyHead SerializeToArray failed.");
	}
	tpBuff += pProxyHead->GetCachedSize();
	tLen += pProxyHead->GetCachedSize();

	if (pMsg == NULL)
	{
		// 发送给proxy或者proxy发出的消息，只有CProxyHead，所以到这里就OK了
		// 主要是注册和心跳
		tAddLen = (tLen % 8);
		if (tAddLen > 0)
		{
			tAddLen = (8 - tAddLen);
		}

		tpBuff = (char*) pBuff;
		// 序列化总长度
		*(unsigned short*) tpBuff = (tLen + tAddLen);
		tpBuff += sizeof(unsigned short);

		// 序列化8字节对齐补充长度
		*(unsigned short*) tpBuff = tAddLen;

		unBuffLen = (tLen + tAddLen);
		return 0;
	}

	// 序列化CMessageHead长度
	// 取长度，第一次用ByteSize()，如果消息不改变，以后就可以GetCachedSize()提高效率
	*(unsigned short*)tpBuff = pMsg->msghead().ByteSize();
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化CMessageHead
	if (pMsg->mutable_msghead()->SerializeToArray(tpBuff, unBuffLen - tLen) == false)
	{
		MY_ASSERT_STR(0, return -3, "ServerCommEngine::ConvertMsgToStream CMessageHead SerializeToArray failed.");
	}
	tpBuff += pMsg->mutable_msghead()->GetCachedSize();
	tLen += pMsg->mutable_msghead()->GetCachedSize();

	// 获取msgpara
	Message* tpMsgPara = (Message*)pMsg->msgpara();
	if (tpMsgPara == NULL)
	{
		MY_ASSERT_STR(0, return -4, "ServerCommEngine::ConvertMsgToStream CMessagePara is NULL.");
	}

	// msgpara长度
	*(unsigned short*)tpBuff = tpMsgPara->ByteSize();
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化msgpara
	if (tpMsgPara->SerializeToArray(tpBuff, unBuffLen - tLen) == false)
	{
		MY_ASSERT_STR(0, return -5, "ServerCommEngine::ConvertMsgToStream msgpara SerializeToArray failed.");
	}
	tpBuff += tpMsgPara->GetCachedSize();
	tLen += tpMsgPara->GetCachedSize();

	// 计算补充长度
	tAddLen = (tLen % 8);
	if (tAddLen > 0)
	{
		tAddLen = (8 - tAddLen);
	}

	tpBuff = (char*) pBuff;
	// 序列化总长度
	*(unsigned short*) tpBuff = (tLen + tAddLen);
	tpBuff += sizeof(unsigned short);

	// 序列化8字节对齐补充长度
	*(unsigned short*) tpBuff = tAddLen;

	unBuffLen = (tLen + tAddLen);

	return 0;
}

int ConvertStreamToProxy(const void* pBuff, unsigned short unBuffLen, CProxyHead* pProxyHead)
{
	if ((pBuff == NULL) || (unBuffLen < (sizeof(unsigned short) * 3)) || (pProxyHead == NULL) )
	{
		MY_ASSERT_STR(0, return -1, "ServerCommEngine::ConvertStreamToMsg Input param failed.");
	}
	char* tpBuff = (char*) pBuff;
	unsigned short tTmpLen = 0;

	// 总长度
	unsigned short tTotalLen = *(unsigned short*)tpBuff;

	// 验证包长与实际长度是否匹配
	if (tTotalLen != unBuffLen)
	{
		MY_ASSERT_STR(0, return -2, "ServerCommEngine::ConvertStreamToMsg tTotalLen = %d unequal to unBuffLen = %d.", tTotalLen, unBuffLen);
	}

	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short tAddLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tAddLen >= 8)
	{
		MY_ASSERT_STR(0, return -3, "ServerCommEngine::ConvertStreamToMsg tAddLen = %d impoosibility.", tAddLen);
	}
	// 直接抛掉补充长度
	tTotalLen -= tAddLen;

	// CProxyHead长度
	tTmpLen = *(unsigned short*)tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// CProxyHead
	if (pProxyHead->ParseFromArray(tpBuff, tTmpLen) != true)
	{
		MY_ASSERT_STR(0, return -4, "ServerCommEngine::ConvertStreamToMsg CProxyHead ParseFromArray failed.");
	}
	tpBuff += tTmpLen;
	tTotalLen -= tTmpLen;

	if (tTotalLen < 0)
	{
		MY_ASSERT_STR(0, return -5, "ServerCommEngine::ConvertStreamToMsg tTotalLen = %d impossibility.", tTotalLen);
	}

	return 0;
}
};
