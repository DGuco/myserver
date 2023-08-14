/*****************************************************************
* FileName:tcp_server.h
* Summary :
* Date	  :2023-8-14
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "tcp_socket.h"

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
class CTCPServer
{
public:
	CTCPServer();
	~CTCPServer();

	int Initialize(short nEntityType, short nEntityID, u_long ulIPAddr, u_short unPort);
	int ConnectToServer(char* szLocalAddr = NULL, eLinkMode emBlock = em_block_mode);
	int CreateServer();
	int GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr);
	u_long GetConnAddr();
	u_short GetConnPort();
	short  GetEntityType();
	short  GetEntityID();
	CTCPSocket<uiRecvBufLen, uiSendBufLen>* GetSocket();

private:
	short   m_nEntityType;
	short   m_nEntityID;
	CTCPSocket<uiRecvBufLen, uiSendBufLen> m_stSocket;
	u_long m_ulIPAddr;
	u_short m_unPort;
};
#endif //__TCP_SERVER_H__
