#ifndef MESSAGE_INTERFACE_H_
#define MESSAGE_INTERFACE_H_

#include <google/protobuf/message.h>
#include "../base/servertool.h"
#include "../base/base.h"

typedef ::google::protobuf::Message Message;

#define CASE_NEW_MSG(msgname,msgid)				\
	case msgid:						\
	{											\
		pTmpMessage = new(macMessageBuff) msgname;	\
		break;									\
	}

class CFactory 
{
public:
	CFactory() {}
	virtual ~CFactory() {}

	virtual Message* CreateMessage(unsigned int uiMessageID) ;
};


#endif /* MESSAGE_INTERFACE_H_ */