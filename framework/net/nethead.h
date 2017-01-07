#ifndef _NETHEAD_H_
#define _NETHEAD_H_

#include <string.h>

#include "../tools/base.h"

#define MAX_NETHEAD_LEN (sizeof(short) * 2 + 		\
		sizeof(int) * (MAX_BROADCAST_NUM + 1) + 	\
		sizeof(time_t) * (MAX_BROADCAST_NUM + 1) + 	\
		sizeof(char))
#define MIN_NETHEAD_LEN (sizeof(short) * 2 + 	\
		sizeof(int) * 2 + 						\
		sizeof(time_t) * 2 + 					\
		sizeof(char))

/*该结构是tcpserver和mainserver通讯用的*/ 
typedef struct CNetHead { 
	unsigned int	m_iSrcIP;							/*由tcpsvrd发给mainserver，记录了该socket的IP*/
	unsigned short	m_nSrcPort;							/*由tcpsvrd发给mainserver，记录了该socket在PORT*/
	unsigned short	m_nNum;								/*由mainserver发给tcpsvrd时，记录需要转发的Socket数量*/
														/*由tcpsvrd发给mainserver时，默认为1*/
	int				m_iSockets[MAX_BROADCAST_NUM];		/*由mainserver发给tcpsvrd时，记录需要转发的Socket在数组中的位置*/
														/*由tcpsvrd发给mainserver时，记录该socket在数组中的位置*/
	time_t			m_tCreateTimes[MAX_BROADCAST_NUM];	/*由mainserver发给tcpsvrd时，记录需要转发的Socket的创建时间*/
														/*由tcpsvrd发给mainserver时，记录该socket创建的时间戳*/
	time_t			m_tStamp;							/*接收和发送数据包的时间戳*/
	char			m_cState;							/*状态， < 0 说明关闭了socket*/

	unsigned short 	m_nIndex; 							/*用于取转发socket的游标，取一次+1，等于m_nNum时表示已经取完*/

	CNetHead() {
		Clear();
	}

	~CNetHead() {}

	void Clear() {
		m_iSrcIP = 0;
		m_nSrcPort = 0;
		m_nNum = 0;
		m_iSockets[0] = 0;
		m_tCreateTimes[0] = 0;
		m_tStamp = 0;
		m_cState = 0;
		m_nIndex = 0;
	}

	void Initialize(time_t tStamp, char cState, unsigned int iSrcIP = 0, unsigned short nSrcPort = 0) {
		Clear();
		m_iSrcIP = iSrcIP;
		m_nSrcPort = nSrcPort;
		m_tStamp = tStamp;
		m_cState = cState;
	}

	int AddEntity(int iSocket, time_t tTime) {
		if (m_nNum >= MAX_BROADCAST_NUM) return -1;
		m_iSockets[m_nNum] = iSocket;
		m_tCreateTimes[m_nNum] = tTime;
		m_nNum++;
		return m_nNum;
	}

	bool HasEntity() {
		return (m_nIndex < m_nNum);
	}

	int PopEntity(int* pSocket, time_t* pTime) {
		if (pSocket == NULL || pTime == NULL) return -1;
		if (!HasEntity()) return -2;

		*(int*)pSocket = m_iSockets[m_nIndex];
		*(time_t*)pTime = m_tCreateTimes[m_nIndex];
		
		return ++m_nIndex;
	}

	static unsigned short MaxSize() {
		return MAX_NETHEAD_LEN;
	}

	static unsigned short MinSize() {
		return MIN_NETHEAD_LEN;
	}

	unsigned short Size() {
		return sizeof(short) * 2 +
			sizeof(int) * (m_nNum + 1) +
			sizeof(time_t) * (m_nNum + 1) + 
			sizeof(char);
	}

	unsigned short SerializeToArray(void* data, int size) {
		if (data == NULL) return -1;
		if (Size() > size) return -2;
		char* pData = (char*) data;
		unsigned short tLen = 0;
		*((unsigned int*)(pData + tLen)) = m_iSrcIP;						tLen += sizeof(unsigned int);
		*((unsigned short*)(pData + tLen)) = m_nSrcPort;					tLen += sizeof(unsigned short);
		*((unsigned short*)(pData + tLen)) = m_nNum;						tLen += sizeof(unsigned short);
		memcpy((pData + tLen), m_iSockets, (sizeof(int) * m_nNum));			tLen += (sizeof(int) * m_nNum);
		memcpy((pData + tLen), m_tCreateTimes, (sizeof(time_t) * m_nNum));	tLen += (sizeof(time_t) * m_nNum);
		*((time_t*)(pData + tLen)) = m_tStamp;								tLen += sizeof(time_t);
		*((char*)(pData + tLen)) = m_cState;								tLen += sizeof(char);
		return tLen;
	}

	unsigned short ParseFromArray(const void* data, int size) {
		if (data == NULL) return -1;
		if (MaxSize() < size || MinSize() > size) return -2;
		const char* pData = (const char*)data;
		unsigned short tLen = 0;
		m_iSrcIP = *((unsigned int*)(pData + tLen));						tLen += sizeof(unsigned int);
		m_nSrcPort = *((unsigned short*)(pData + tLen));					tLen += sizeof(unsigned short);
		m_nNum = *((unsigned short*)(pData + tLen));						tLen += sizeof(unsigned short);
		memcpy(m_iSockets, (pData + tLen), (sizeof(int) * m_nNum));			tLen += (sizeof(int) * m_nNum);
		memcpy(m_tCreateTimes, (pData + tLen), (sizeof(time_t) * m_nNum));	tLen += (sizeof(time_t) * m_nNum);
		m_tStamp = *((time_t*)(pData + tLen));								tLen += sizeof(time_t);
		m_cState = *((char*)(pData + tLen));								tLen += sizeof(char);
		// 成功反序列化时重置游标
		m_nIndex = 0;
		return tLen;
	}

} TNetHead;

#endif // _NETHEAD_H_
