/*
 * messagefactory.h
 *
 *  Created on  :  2017年8月8日
 *  Author      : DGuco
 */

#ifndef SERVER_MESSAGE_FACTORY_H_
#define SERVER_MESSAGE_FACTORY_H_

#include "message_interface.h"

//实际可用空间MSG_POOL_SIZE - 1
#define MSG_POOL_SIZE 1024 + 1

#define CASE_NEW_MSG(msgname, msgid, addr)  \
    case msgid:                       \
    {                                 \
        pTmpMessage = new (addr)msgname;    \
        break;                        \
    }


class CMessageFactory: public CFactory, public CSingleton<CMessageFactory>
{
public:
	CMessageFactory();
	//创建消息
	CGooMess *CreateMessage(unsigned int uiMessageID) override;
	//释放消息
	void FreeMessage(CGooMess *gooMess) override;
private:
	CGooMess *CreateClientMessage(unsigned int uiMessageID);
	CGooMess *CreateServerMessage(unsigned int uiMessageID);
	int GetFreeSize();
public:
	char macMessageBuff[MSG_POOL_SIZE][MAX_PACKAGE_LEN];
private:
	int m_iFront;
	int m_iBack;
	std::mutex m_oMutex;
};


#endif /* MESSAGE_FACTORY_H_ */
