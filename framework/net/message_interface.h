#ifndef MESSAGE_INTERFACE_H_
#define MESSAGE_INTERFACE_H_

#include <google/protobuf/message.h>
#include "server_tool.h"
#include "base.h"

typedef ::google::protobuf::Message CGooMess;

class CFactory
{
public:
	CFactory() {}
	virtual ~CFactory() {}

	virtual CGooMess* CreateMessage(unsigned int uiMessageID) ;
};


#endif /* MESSAGE_INTERFACE_H_ */
