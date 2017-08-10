//
// Created by DGuco on 17-7-27.
//

#ifndef SERVER_MESSAGEDISPATCHER_H
#define SERVER_MESSAGEDISPATCHER_H

#include "../../framework/base/servertool.h"

class CMessage;
class CCSHead;
class CProxyHead;

class CMessageDispatcher : public CSingleton<CMessageDispatcher>
{
public:
    CMessageDispatcher();
    ~CMessageDispatcher();

    // 客户端上传的消息派发
    int ProcessClientMessage(CMessage* pMsg);
    // 服务器消息派发
    int ProcessServerMessage(CProxyHead* pHead, CMessage* pMsg);
};

#endif //SERVER_MESSAGEDISPATCHER_H
