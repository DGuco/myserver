//
// Created by DGuco on 17-7-13.
//

#include "../inc/message_factory.h"
#include "../../framework/message/dbmessage.pb.h"

CMessageFactory::CMessageFactory()
{

	memset(macMessageBuff, 0, sizeof(macMessageBuff));
}

CMessageFactory::~CMessageFactory()
{
}
CGooMess *CMessageFactory::CreateMessage(unsigned int uiMessageID)
{
	CGooMess *tpMessage = CreateServerMessage(uiMessageID);

	if (tpMessage == NULL) {
		LOG_ERROR("default", "invalid message id %u.", uiMessageID);
	}
	else {
		LOG_DEBUG("default", "create message id %u succeed.", uiMessageID);
	}

	return tpMessage;
}

CGooMess *CMessageFactory::CreateServerMessage(unsigned int uiMessageID)
{
	CGooMess *pTmpMessage = NULL;

	switch (uiMessageID) {
	CASE_NEW_MSG(CMsgExecuteSqlRequest, CMsgExecuteSqlRequest::MsgID);
	default: {
		break;
	}
	}

	return pTmpMessage;
}
