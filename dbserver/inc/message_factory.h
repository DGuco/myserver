//
// messagefactory.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_MESSAGEFACTORY_H
#define SERVER_MESSAGEFACTORY_H

#include "message_factory.h"
#include "message_interface.h"

#define CASE_NEW_MSG(msgname, msgid)                \
    case msgid:                        \
    {                                            \
        pTmpMessage = new(macMessageBuff) msgname;    \
        break;                                    \
    }

class CMessageFactory: public CFactory
{
public:
	CMessageFactory();
	~CMessageFactory();

public:
	CGooMess *CreateMessage(unsigned int uiMessageID);

private:
	CGooMess *CreateServerMessage(unsigned int uiMessageID);
	unsigned char macMessageBuff[MAX_PACKAGE_LEN];
};

#endif //SERVER_MESSAGEFACTORY_H
