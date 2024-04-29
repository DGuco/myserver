/*****************************************************************
* FileName:message_factory.h
* Summary :
* Date	  :2024-3-25
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __MESSAGE_FACTORY_H__
#define __MESSAGE_FACTORY_H__

#include <memory>
#include <google/protobuf/message.h>
#include "safe_pointer.h"
#include "common_def.h"
#include "base.h"

class CTCPSocket;
typedef ::google::protobuf::Message ProtoMess;
using namespace std;

class CMessageFactory
{
public:
	CMessageFactory() {};
	virtual ~CMessageFactory() {};
	virtual shared_ptr<ProtoMess>	CreateMessage() = 0;
	virtual int						Execute(CSafePtr<CTCPSocket> pSocket) = 0;
	virtual int						MessId() = 0;
private:
	static BYTE	 m_MessageBuff[MAX_PACKAGE_LEN];
};

#endif /* __MESSAGE_FACTORY_H__ */
