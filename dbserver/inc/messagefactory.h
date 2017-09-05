//
// Created by DGuco on 17-7-13.
//

#ifndef SERVER_MESSAGEFACTORY_H
#define SERVER_MESSAGEFACTORY_H

#include "../inc/messagefactory.h"
#include "../../framework/message/message_interface.h"

class CMessageFactory : public CFactory
{
public:
    CMessageFactory();
    ~CMessageFactory();

public:
    Message* CreateMessage(unsigned int uiMessageID);

private:
    Message* CreateServerMessage(unsigned int uiMessageID);
    unsigned char macMessageBuff[MAX_PACKAGE_LEN];
};

#endif //SERVER_MESSAGEFACTORY_H
