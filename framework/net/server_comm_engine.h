#ifndef SERVER_COMM_ENGINE_H_
#define SERVER_COMM_ENGINE_H_


#include "../message/message.pb.h"
#include "../message/message_interface.h"


void pbmsg_setproxy(CProxyHead* pHead, int iSrcFE, int iSrcID, int iDstFE, int iDstID, time_t tTimestamp, enMessageCmd eCmd);
void pbmsg_setmessagehead(CProxyHead* pHead, int iMsgID);


namespace ServerCommEngine
{
	// --------------------------------------------------------------------------------
	// Function:	ConvertStreamToMsg
	// Description:	把服务器的二进制流转换成CProxyHead + CMessageHead + CMessage.msgpara
	// 				消息体指针已经在内部赋值给了CMessage.msgpara
	// 				注意: 需要在外部delete msgpara
	// --------------------------------------------------------------------------------
	int ConvertStreamToMsg(const void* pBuff, unsigned short unBuffLen, CProxyHead* pProxyHead, CMessage* pMsg, CFactory* pMsgFactory);
	// --------------------------------------------------------------------------------
	// Function:	ConvertMsgToStream
	// Description:	序列化CProxyHead + CMessageHead + CMessage.msgpara
	// --------------------------------------------------------------------------------
	int ConvertMsgToStream(CProxyHead* pProxyHead, CMessage* pMsg, void* pBuff, unsigned short& unBuffLen);
	// --------------------------------------------------------------------------------
	// Function:	ConvertStreamToProxy
	// Description:	把服务器的二进制流转换成CProxyHead 
	// --------------------------------------------------------------------------------
	int ConvertStreamToProxy(const void* pBuff, unsigned short unBuffLen, CProxyHead* pProxyHead);

};

#endif // SERVER_COMM_ENGINE_H_
