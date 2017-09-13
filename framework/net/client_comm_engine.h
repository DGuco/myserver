#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

#include "message_interface.h"
#include "message.pb.h"
#include "ccrypto.h"


class ClientCommEngine
{
public:
	static unsigned char tKey[16];
	static unsigned char* tpKey;
    static void CopyMesHead(MesHead* from,MesHead* to);

    /**
	 * @param pBuff         client上行数据
	 * @param unBuffLen     上行数据长度
	 * @param pHead         转发给gameserver的消息C2SHead
     * @param unTmpDataLen  当前数据包的长度
	 * @return          0：成功 1：数据不完整继续接收  其他：错误
	 */
	static int ParseClientStream(const void** pBuff,
                                    int nRecvAllLen,
                                    MesHead* pHead,
                                    MSG_LEN_TYPE & unTmpDataLen);
                      
    /**
     * 序列化消息发送到gameserver
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     消息长度
     * @param pDataBuff     客户端上行消息数据指针
     * @param unDataLen     客户端上行消息数据长度
     * @param pHead         反序列化客户端消息组成的消息头
     * @return               0 成功 其他 失败
     */
	static int ConverToGameStream(const void* pBuff,
                                    MSG_LEN_TYPE& unBuffLen,
                                    const void *pDataBuff,
                                    MSG_LEN_TYPE& unDataLen,
                                    MesHead* pHead);
                                    
    /**
     * 序列化消息Message 发送到gameserver（gateserver==>gameserver）
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     长度
     * @param pHead         消息头
     * @param pMsg          反序列化客户端消Message
     * @return              0 成功 其他 失败
     */
	static int ConvertToGameStream(const void* pBuff,
        MSG_LEN_TYPE& unBuffLen,
        MesHead* pHead,
        Message* pMsg = NULL);


    /**
     * 序列化消息CMessage 发送到gateserver（gameserver==>gateserver）
     * @param pBuff         存放序列化后消息的地址
     * @param unBuffLen     长度
     * @param pHead         消息头
     * @param pMsg          反序列化客户端消息Message
     * @return              0 成功 其他 失败
     */
	static int ConvertToGateStream(const void* pBuff,
                                    MSG_LEN_TYPE& unBuffLen,
                                    MesHead* pHead,
                                    Message* pMsg);

    /**
     * @param pBuff         存放转换后地址指针
     * @param unBuffLen     消息长度
     * @param pHead         反序列化后的消息头
     * @param message       反序列化后的消息
     * @return              0 成功 其他:失败错误码
     */
	static int ConvertStreamToMessage(const void* pBuff,
                                        MSG_LEN_TYPE unBuffLen,
                                        MesHead* pHead,
                                        Message* pMessage = NULL,
                                        CFactory* pMsgFactory = NULL,
                                        int* unOffset = NULL);
};

#endif /* CLIENT_COMM_ENGINE_H_ */
