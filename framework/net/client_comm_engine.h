#ifndef CLIENT_COMM_ENGINE_H_
#define CLIENT_COMM_ENGINE_H_

#include <message.pb.h>
#include "message_interface.h"
#include "message.pb.h"
#include "ccrypto.h"
#include "byte_buff.h"
using namespace std;

class CClientCommEngine
{
public:
	static unsigned char tKey[16];
	static unsigned char *tpKey;
	static void CopyMesHead(CMesHead *from, CMesHead *to);

	/**
	 *
	 * @param byteBuff
	 * @param pHead
	 * @return
	 */
	static int ParseClientStream(CByteBuff *parseByteBuff,
								 CMesHead *pHead);

	/**
	 * 序列化消息Message 发送到gameserver（gateserver==>gameserver）
	 * @param convertBuff
	 * @param pDataBuff
	 * @param unDataLen
	 * @param pHead
	 * @return
	 */
	static int ConvertToGameStream(CByteBuff *convertBuff,
								   const void *pDataBuff,
								   unsigned short &unDataLen,
								   CMesHead *pHead);

	/**
	 * 序列化消息Message 发送到gameserver（gateserver==>gameserver）
	 * @param pBuff
	 * @param unBuffLen
	 * @param pHead
	 * @param pMsg
	 * @return
	 */
	static int ConvertToGameStream(CByteBuff* convertBuff,
								   CMessage *pMessage);

	/**
	 * 序列化消息CMessage 发送到gateserver（gameserver==>gateserver）
	 * @param pBuff
	 * @param unBuffLen
	 * @param pHead
	 * @param pMsg
	 * @return
	 */
	static int ConvertToGateStream(CByteBuff *pBuff,
								   CMessage *pMessage,
								   unsigned short cmd,
								   unsigned short serial,
								   unsigned short seq);

	/**
	 *
	 * @param pBuff
	 * @param unBuffLen
	 * @param pMessage
	 * @param pMsgFactory
	 * @return
	 */
	static int ConvertStreamToMessage(CByteBuff *pBuff,
									  unsigned short unBuffLen,
									  CMessage *pMessage = NULL,
									  CFactory *pMsgFactory = NULL);
};

#endif /* CLIENT_COMM_ENGINE_H_ */
