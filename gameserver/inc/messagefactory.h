/*
 * messagefactory.h
 *
 *  Created on  :  2017年8月8日
 *  Author      : DGuco
 *  c++ placement new 操作符在指定空间macMessageBuff生成消息
 */

#ifndef SERVER_MESSAGE_FACTORY_H_
#define SERVER_MESSAGE_FACTORY_H_

#include "../../framework/message/message_interface.h"

class CMessageFactory : public CFactory, public CSingleton<CMessageFactory>
{
public:
	Message* CreateMessage(unsigned int uiMessageID);

private:
	Message* CreateClientMessage(unsigned int uiMessageID);
	Message* CreateServerMessage(unsigned int uiMessageID);

	static unsigned char macMessageBuff[MAX_PACKAGE_LEN];
};


#endif /* MESSAGE_FACTORY_H_ */
