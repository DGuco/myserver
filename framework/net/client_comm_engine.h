#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

class CMessage;
class CTcpHead;

#include "../message/message_interface.h"
#include "../message/message.pb.h"
#include "ccrypto.h"

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
     * 反序列化客户端上行消息(gateserver使用)
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
     * 反序列化gateserver上行的客户端消息(gameserver使用)
     * @param pBuff         消息指针
     * @param unBuffLen     消息长度
     * @param pHead         反序列化后的消息头
     * @param message       反序列化后的消息
     * @return              0 成功 其他:失败错误码
     */
	static int ConvertStreamToClientMsg(char* pBuff,
                                        unsigned short unBuffLen,
                                        C2SHead* pHead,
                                        Message* pMessage,
                                        CFactory* pMsgFactory = NULL);                        
    /**
     * 序列化消息CClientMessage 发送到gameserver(gateservet==>gameserver)
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pMsg          反序列化客户端消息组成的CClientMessage
     * @param bEncrypt      是否加密
     * @param pEncrypt      密钥
     * @return               0 成功 其他 失败
     */
	static int ConvertClientMessagedToStream(unsigned char* pBuff,
                                    unsigned short& unBuffLen,
                                    CClientMessage* pMsg = NULL,
                                    bool bEncrypt = false,
                                    const unsigned char* pEncrypt = ClientCommEngine::tpKey);


    /**
     * 序列化消息CMessage 发送到gateserver（gameserver==>gateserver）
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pMsg          反序列化客户端消息组成的CClientMessage
     * @param bEncrypt      是否加密
     * @param pEncrypt      密钥
     * @return               0 成功 其他 失败
     */
	static int ConvertGameServerMessageToStream(unsigned char* pBuff,
                                    unsigned short& unBuffLen,
                                    CGameServerMessage* pMsg = NULL);
};

#endif /* CLIENT_COMM_ENGINE_H_ */
