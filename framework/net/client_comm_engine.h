#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

class CMessage;
class CTcpHead;

#include "../message/message_interface.h"
#include "../message/message.pb.h"
#include "../message/tcpmessage.pb.h"
#include "ccrypto.h"

// 加密类型定义
enum EncryptType
{
	EncryptNone = 0,
	EncryptType1 = 1,
	EncryptType2 = 2,
};

void pbmsg_settcphead(CTcpHead& rHead, int iSrcFE, int iSrcID, int iDstFE, int iDstID, time_t tTimestamp, EGateCmd eCmd = EGC_NULL);

class ClientCommEngine
{
public:
	static unsigned char tKey[16];
	static unsigned char* tpKey;

    // 反序列化客户端Message
    static int ConvertClientStreamToMsg(const void* pBuff,
                                  unsigned short unBuffLen,
                                  CMessage* pMsg,
                                  CFactory* pMsgFactory = NULL,
                                  bool bEncrypt = false,
                                  const unsigned char* pEncrypt = ClientCommEngine::tpKey);

    // 反序列化CMessage
	static int ConvertStreamToMsg(const void* pBuff, 
								 unsigned short unBuffLen,
								 CMessage* pMsg, 
								 CFactory* pMsgFactory = NULL, 
								 bool bEncrypt = false, 
								 const unsigned char* pEncrypt = ClientCommEngine::tpKey);

    // 反序列化CTcpHead, 返回剩余长度
    static int ConvertStreamToMsg(const void* pBuff,
                                  unsigned short unBuffLen,
                                  unsigned short& rOffset,
                                  CTcpHead* pTcpHead);

	// 序列化消息(CMessage为空代表服务器内部消息)
	static int ConvertMsgToStream(void* pBuff, 
								unsigned short& unBuffLen, 
								const CTcpHead* pTcpHead, 
								CMessage* pMsg = NULL, 
								bool bEncrypt = false, 
								const unsigned char* pEncrypt = ClientCommEngine::tpKey);
};


#endif /* CLIENT_COMM_ENGINE_H_ */
