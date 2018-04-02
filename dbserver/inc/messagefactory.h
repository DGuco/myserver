//
// messagefactory.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_MESSAGEFACTORY_H
#define SERVER_MESSAGEFACTORY_H

#include "messagefactory.h"
#include "message_interface.h"

class CMessageFactory: public CFactory
{
public:
	CMessageFactory();
	~CMessageFactory();

public:
	Message *CreateMessage(unsigned int uiMessageID);

private:
	Message *CreateServerMessage(unsigned int uiMessageID);
	unsigned char macMessageBuff[MAX_PACKAGE_LEN];
};

#endif //SERVER_MESSAGEFACTORY_H
