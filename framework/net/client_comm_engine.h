#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

#include "message_interface.h"
#include "message.pb.h"
#include "ccrypto.h"
#include "byte_buff.h"

class CClientCommEngine
{
public:
	static unsigned char tKey[16];
	static unsigned char *tpKey;
	static void CopyMesHead(MesHead *from, MesHead *to);

	/**
	 *
	 * @param byteBuff
	 * @param pHead
	 * @return
	 */
	static int ParseClientStream(CByteBuff *byteBuff,
								 MesHead *pHead);

	/**
	 * 序列化消息Message 发送到gameserver（gateserver==>gameserver）
	 * @param convertBuff
	 * @param unBuffLen
	 * @param pDataBuff
	 * @param unDataLen
	 * @param pHead
	 * @return
	 */
	static int ConverToGameStream(CByteBuff *convertBuff,
								  const void *pDataBuff,
								  unsigned short &unDataLen,
								  MesHead *pHead);

	/**
	 * 序列化消息Message 发送到gameserver（gateserver==>gameserver）
	 * @param pBuff
	 * @param unBuffLen
	 * @param pHead
	 * @param pMsg
	 * @return
	 */
	static int ConvertToGameStream(CByteBuff *convertBuff,
								   MesHead *pHead,
								   Message *pMsg = NULL);

	/**
	 * 序列化消息CMessage 发送到gateserver（gameserver==>gateserver）
	 * @param pBuff         存放序列化后消息的地址
	 * @param unBuffLen     长度
	 * @param pHead         消息头
	 * @param pMsg          反序列化客户端消息Message
	 * @return              0 成功 其他 失败
	 */
	static int ConvertToGateStream(const void *pBuff,
								   unsigned short &unBuffLen,
								   MesHead *pHead,
								   Message *pMsg);

	/**
	 * @param pBuff         存放转换后地址指针
	 * @param unBuffLen     消息长度
	 * @param pHead         反序列化后的消息头
	 * @param message       反序列化后的消息
	 * @return              0 成功 其他:失败错误码
	 */
	static int ConvertStreamToMessage(const void *pBuff,
									  unsigned short unBuffLen,
									  CMessage *pMessage = NULL,
									  CFactory *pMsgFactory = NULL,
									  int *unOffset = 0x0);
};

#endif /* CLIENT_COMM_ENGINE_H_ */
