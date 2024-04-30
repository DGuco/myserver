//
//  gameserver.h
//  gameserver ͷ�ļ�
//  Created by DGuco on 17-3-1.
//  Copyright ? 2017�� DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H
#include "base.h"
#include "singleton.h"
#include "runflag.h"
#include "tcp_def.h"
#include "safe_pointer.h"
#include "tcp_server.h"
#include "game_player.h"
#include "server_client.h"
#include "time_helper.h"
#include "shm_queue.h"

class CGameServer: public CTCPServer,public CSingleton<CGameServer>
{
public:
	CGameServer();
	~CGameServer();
	// ����׼��
	bool PrepareToRun();
	// �˳�
	void Exit();
	//����ͻ�����������
	void ProcessClientMessage(CSafePtr<CGamePlayer> pGamePlayer);
	//�����ȡ����������
	void ProcessServerMessage(CSafePtr<CServerClient> pServerPlayer);
	//
	void ClearSocket(CSafePtr<CGamePlayer> pGamePlayer, short iError);
	//
	void DisConnect(CSafePtr<CGamePlayer> pGamePlayer, short iError);
	//
	void RecvDBMessage();
	//
	int  SendMessageToDB(char* data, int iTmpLen);
public:
	//�����ӻص�
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket socket);
public:
	// ���÷�����״̬
	void SetServerState(int iState)
	{ m_iServerState = (m_iServerState | iState); }
	// ���������״̬
	void EraseServerState(int iState)
	{ m_iServerState = (m_iServerState & (~iState)); }
	// �жϷ�����״̬
	bool IsOk(int iState)
	{ return ((m_iServerState & iState) == iState); }
	// ��������ȡ�������
	void LoadDataFinish();
	// ��������ʼ�����ʼ����
	void StartProcessingInitData();
	// ��DB Server����Ϣ
	//bool SendMessageToDB(CSafePtr <CProxyMessage> pMsg);
	// ͨ����ϢID��ȡģ������
	int GetModuleClass(int iMsgID);
private:
	CSafePtr<CByteBuff>		m_pRecvBuff;
	//gateserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_DB2SCodeQueue;
	//gameserver ==> gateserver
	CSafePtr<CShmMessQueue>	m_S2DBCodeQueue;
	CRunFlag				m_oRunFlag;                         // ����������״̬
	int						m_iServerState;    // ������״̬
	BYTE					m_CacheData[MAX_PACKAGE_LEN];
};
#endif //SERVER_GAMESERVER_H
