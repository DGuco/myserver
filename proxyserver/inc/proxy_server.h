/*****************************************************************
* FileName:gate_server.h
* Summary :
* Date	  :2024-2-22
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__
#include "safe_pointer.h"
#include "tcp_server.h"
#include "singleton.h"
#include "message.pb.h"
#include "proxy_player.h"
#include "common_def.h"
#include "shm_queue.h"

class CProxyServer : public CTCPServer,public CSingleton<CProxyServer>
{
public:
	//���캯��
	CProxyServer();
	//��������
	virtual ~CProxyServer();
public:
	//׼��run
	bool PrepareToRun();
	//���socket
	void ClearSocket(CSafePtr<CProxyPlayer> pGamePlayer, short iError);
	//֪ͨgameserver client �Ͽ�����
	void DisConnect(CSafePtr<CProxyPlayer> pGamePlayer, short iError);
	//���ܿͻ�������
	void RecvClientData(CSafePtr<CProxyPlayer> pGamePlayer);
	//������Ϣ��client
	void RecvGameData();
	//��game ������Ϣ
	int SendToGame(char* data, int iTmpLen);
	//��client�������ݰ�
	//int SendToClient(CMessG2G& tmpMes);
private:
	//���ض�client��������
	//void SendToClient(const CSocketInfo& socketInfo, const char* data, unsigned int len);
public:
	//�����ӻص�
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket);
private:
	//gateserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_C2SCodeQueue;
	//gameserver ==> gateserver
	CSafePtr<CShmMessQueue>	m_S2CCodeQueue;
	CSafePtr<CByteBuff>		m_pRecvBuff;
	BYTE						m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};

#endif //__GATE_SERVER_H__

