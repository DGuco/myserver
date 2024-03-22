/*
 *  message_factory.cpp
 *
 *  Created on  : 2017年8月8日
 *  Author      : DGuco
 */

#include <player.pb.h>
#include "../inc/message_factory.h"


CMessageFactory::CMessageFactory()
{}

// 根据消息ID，创建消息
CGooMess *CMessageFactory::CreateMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = CreateClientMessage(uiMessageID);
	if (pTmpMessage == NULL) 
	{
		pTmpMessage = CreateServerMessage(uiMessageID);
	}

	if (pTmpMessage == NULL) 
	{
		CACHE_LOG(ERROR_CACHE, "CreateMessage failed invalid message id {}.", uiMessageID);
	}
	return pTmpMessage;
}

void CMessageFactory::FreeMessage(CGooMess *gooMess)
{
	//因为使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
	gooMess->~Message();
}
CGooMess *CMessageFactory::CreateClientMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	switch(uiMessageID)
	{
		// 只有客户端上行的消息需要在这里生成，下行的消息都是临时变量，不需要在MessageFactory中创建
		CASE_NEW_MSG(UserAccountLoginRequest, 100, m_aMessageBuff);
		CASE_NEW_MSG(PlayerLoginRequest, 101, m_aMessageBuff);
		default:
		{
			break;
		}
	}
	return pTmpMessage;
}

CGooMess *CMessageFactory::CreateServerMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	switch(uiMessageID)
	{
		// 服务器内部的消息都需要在这里创建
		{
			break;
		}
	}
	return pTmpMessage;
}