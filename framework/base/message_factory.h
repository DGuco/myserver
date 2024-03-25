/*****************************************************************
* FileName:message_factory.h
* Summary :
* Date	  :2024-3-25
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __MESSAGE_FACTORY_H__
#define __MESSAGE_FACTORY_H__

#include "safe_pointer.h"

class CTCPSocket;
class CGameMess
{
public:
	CGameMess();
	virtual ~CGameMess();
	virtual int Execute(CSafePtr<CTCPSocket> pSocket) = 0;
};

class CMessageFactory
{
public:
	CMessageFactory() {};
	virtual ~CMessageFactory() {};
	virtual CSafePtr<CGameMess> CreateMessage() = 0;
};


#endif /* __MESSAGE_FACTORY_H__ */
