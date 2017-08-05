#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

class CMessage;
class CTcpHead;

#include "../message/message_interface.h"
#include "../message/message.pb.h"
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
//////////////////////////////////////正常的消息序列化操作////////////////////////////////////
    /**
     *
     * @param pMsgSet
     * @param pMsg
     * @return
     */
	static int AddMsgToMsgSet(CMessageSet* pMsgSet,
							CMessage* pMsg);

	/**
	 * @param pBuff     client上行数据
	 * @param unBuffLen 数据长度
	 * @param pMsg      转发给gameserver的消息格式
	 * @param pMsgFactory
	 * @param bEncrypt
	 * @param pEncrypt
	 * @return  ： 0：成功 1：数据不完整继续接收  其他：错误
	 */
	static int ConvertStreamToClientMsg(char* pBuff,
                                     unsigned short nRecvAllLen,
                                     CClientMessage* pMsg,
                                     CFactory* pMsgFactory = NULL,
                                     bool bEncrypt = false,
                                     const unsigned char* pEncrypt = ClientCommEngine::tpKey);
    /**
     *
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pMsg          反序列化客户端消息组成的CClientMessage
     * @param bEncrypt      是否加密
     * @param pEncrypt      密钥
     * @return               0 成功 其他 失败
     */
	// 序列化消息CClientMessaged 为发送到gameserver的格式)
	static int ConvertClientMessagedToStream(unsigned char* pBuff,
                                    unsigned short& unBuffLen,
                                    CClientMessage* pMsg = NULL,
                                    bool bEncrypt = false,
                                    const unsigned char* pEncrypt = ClientCommEngine::tpKey);


    static int ParaseGameServerMessage(unsigned char* pBuff)

};


#endif /* CLIENT_COMM_ENGINE_H_ */
