/*
 *  message_factory.cpp
 *
 *  Created on  : 2017年8月8日
 *  Author      : DGuco
 */

#include <player.pb.h>
#include "../inc/message_factory.h"


// 单件定义
template<> shared_ptr<CMessageFactory> CSingleton<CMessageFactory>::spSingleton = NULL;

CMessageFactory::CMessageFactory()
	:
	m_iFront(0),
	m_iBack(0)
{
}

// 根据消息ID，创建消息
CGooMess *CMessageFactory::CreateMessage(unsigned int uiMessageID)
{
	std::lock_guard<mutex> lock(m_oMutex);

	if (GetFreeSize() <= 0) {
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
		m_iBack = (m_iBack + 1) % MSG_POOL_SIZE;
		LOG_DEBUG("default", "create message id {} succeed.", uiMessageID);
	}

	return pTmpMessage;
}

void CMessageFactory::FreeMessage(CGooMess *gooMess)
{
	std::lock_guard<mutex> lock(m_oMutex);
	//因为使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
	gooMess->~Message();
	m_iFront = (m_iFront + 1) % MSG_POOL_SIZE;
}
CGooMess *CMessageFactory::CreateClientMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	switch(uiMessageID){
		// 只有客户端上行的消息需要在这里生成，下行的消息都是临时变量，不需要在MessageFactory中创建
		CASE_NEW_MSG(UserAccountLoginRequest, 100, macMessageBuff[m_iBack]);
		CASE_NEW_MSG(PlayerLoginRequest, 101, macMessageBuff[m_iBack]);
		default:{
			break;
		}
	}
	return pTmpMessage;
}

CGooMess *CMessageFactory::CreateServerMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;
	switch(uiMessageID){
		// 服务器内部的消息都需要在这里创建
		{
			break;
		}
	}
	return pTmpMessage;
}

int CMessageFactory::GetFreeSize()
{
	int nTempMaxLength = 0;
	//获得剩余空间大小
	if (m_iFront == m_iBack) {
		nTempMaxLength = MSG_POOL_SIZE;  // nTempMaxLength 为剩余空间
	}
	else if (m_iFront > m_iBack) {
		nTempMaxLength = m_iFront - m_iBack;
	}
	else {
		nTempMaxLength = (MSG_POOL_SIZE - m_iBack) + m_iFront;
	}

	/**
	 * 最大长度应该减去预留部分长度，保证首尾不会相接,
	 * 以此区分数据头不在内存区头部写满数据，和没有数据的情况
	 */
	nTempMaxLength -= 1;
	return nTempMaxLength;
}