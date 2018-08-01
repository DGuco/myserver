/*
 *  message_factory.cpp
 *
 *  Created on  : 2017年8月8日
 *  Author      : DGuco
 */

#include "player.pb.h"
#include "../inc/message_factory.h"

// 单件定义
template<> shared_ptr<CMessageFactory> CSingleton<CMessageFactory>::spSingleton = NULL;

CMessageFactory::CMessageFactory()
	:
	m_iFront(-1),
	m_iBack(0)
{
	for (int i = 0; i < MSG_POOL_SIZE; i++) {
		memset(macMessageBuff[i], 0, MAX_PACKAGE_LEN);
	}
}

// 根据消息ID，创建消息
CGooMess *CMessageFactory::CreateMessage(unsigned int uiMessageID)
{
	if (m_iBack == m_iFront) {
		LOG_ERROR("default", "The msg pool has no free spacd,front:{},back:{}", m_iFront, m_iBack);
		return NULL;
	}

	CGooMess *pTmpMessage = CreateClientMessage(uiMessageID);
	if (pTmpMessage == NULL) {
		pTmpMessage = CreateServerMessage(uiMessageID);
	}

	if (pTmpMessage == NULL) {
		LOG_ERROR("default", "[{} : {} : {}] invalid message id {}.", __MY_FILE__, __LINE__, __FUNCTION__, uiMessageID);
	}
	else {
		LOG_DEBUG("default", "create message id {} succeed.", uiMessageID);
	}

	return pTmpMessage;
}

void CMessageFactory::FreeMessage(CGooMess *gooMess)
{
	gooMess->~Message();
	int index = (m_iFront + 1) % MSG_POOL_SIZE;
}

CGooMess *CMessageFactory::CreateClientMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	switch (uiMessageID) {
		// 只有客户端上行的消息需要在这里生成，下行的消息都是临时变量，不需要在MessageFactory中创建
	CASE_NEW_MSG(UserAccountLoginRequest, 100, macMessageBuff[m_iBack]);
	CASE_NEW_MSG(PlayerLoginRequest, 101, macMessageBuff[m_iBack]);
	default: {
		break;
	}
	}
	if (pTmpMessage != NULL) {
		m_iBack++;
	}
	return pTmpMessage;
}

CGooMess *CMessageFactory::CreateServerMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	int index = m_iBack % MSG_POOL_SIZE;
	switch (uiMessageID) {
		// 服务器内部的消息都需要在这里创建
		{
			break;
		}
	}
	if (pTmpMessage != NULL) {
		m_iBack++;
	}
	return pTmpMessage;
}

