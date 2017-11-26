//
// Created by DGuco on 17-7-27.
//

#ifndef SERVER_MESSAGEDISPATCHER_H
#define SERVER_MESSAGEDISPATCHER_H

#include "servertool.h"
#include "message.pb.h"

class CMessageDispatcher : public CSingleton<CMessageDispatcher>
{
public:
    CMessageDispatcher();
    ~CMessageDispatcher();

    // 客户端上传的消息派发
    int ProcessClientMessage(CMessage* pMsg);
    // 服务器消息派发
    int ProcessServerMessage(CProxyMessage* pMessage);
};

#endif //SERVER_MESSAGEDISPATCHER_H
