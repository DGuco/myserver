#ifndef MESSAGE_INTERFACE_H_
#define MESSAGE_INTERFACE_H_

#include <type_traits>
#include <google/protobuf/message.h>
#include "safe_pointer.h"
#include "tcp_socket.h"

using namespace std;

typedef ::google::protobuf::Message CGoogleMess;

template<typename M_>
class CMessageFactory
{
	static_assert(std::is_base_of<CGoogleMess, M_>, "is not a google protobuf message");
public:
	CMessageFactory() {}
	~CMessageFactory() {}
	virtual Execute(CSafePointer<M_> pMessage, CSafePointer<CTCPSocket> pSocket) = 0;
private:
	CSafePointer<M_> m_pMessage;
};


#endif /* MESSAGE_INTERFACE_H_ */
