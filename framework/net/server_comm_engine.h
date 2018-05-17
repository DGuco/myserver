#ifndef SERVER_COMM_ENGINE_H_
#define SERVER_COMM_ENGINE_H_


#include "message.pb.h"
#include "message_interface.h"
#include "byte_buff.h"

void pbmsg_setproxy(CProxyHead *pHead,
					int iSrcFE,
					int iSrcID,
					int iDstFE,
					int iDstID,
					time_t tTimestamp,
					enMessageCmd eCmd);

void pbmsg_setmessagehead(CProxyHead *pHead, int iMsgID);

class CServerCommEngine
{
public:
	static int ConvertStreamToMsg(CByteBuff *pBuff,
								  CProxyMessage *pMsg,
								  CFactory *pMsgFactory = NULL);
	// --------------------------------------------------------------------------------
	// Function:	ConvertMsgToStream
	// Description:	序列化CProxyHead + CMessageHead + CMessage.msgpara
	// --------------------------------------------------------------------------------
	static int ConvertMsgToStream(CProxyMessage *pMsg,
								  void *pBuff,
								  unsigned short &unBuffLen);
	// --------------------------------------------------------------------------------
	// Function:	ConvertStreamToProxy
	// Description:	把服务器的二进制流转换成CProxyHead 
	// --------------------------------------------------------------------------------
	static int ConvertStreamToProxy(CByteBuff *pBuff,
									CProxyHead *pProxyHead);

};

#endif // SERVER_COMM_ENGINE_H_
