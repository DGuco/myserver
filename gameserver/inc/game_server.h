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
#include "common_def.h"
#include "safe_pointer.h"
#include "tcp_server.h"
#include "game_player.h"
#include "server_client.h"

class CGameServer: public CTCPServer,public CSingleton<CGameServer>
{
public:
	enum EServerState
	{
		ESS_CONNECTPROXY = 0x0001,    // ����proxyl
		ESS_LOADDATA = 0x0002,    // ��������
		ESS_PROCESSINGDATA = 0x0004,    // ��������
		ESS_SAVEDATA = 0x0008,    // ͣ���洢����

		ESS_NORMAL = (ESS_CONNECTPROXY | ESS_LOADDATA | ESS_PROCESSINGDATA),        // ����
		ESS_PROCESSINGCLIENTMSG = (ESS_LOADDATA | ESS_PROCESSINGDATA)    // ������������ͻ������е���Ϣ
	};

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
public:
	//�����ӻص�
	virtual void OnNewConnect(CSafePtr<CTCPConn> pConnn);
	//
	virtual CSafePtr<CTCPConn> CreateTcpConn(CSocket tmSocket);
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
	// �������Ƿ�����
	bool IsNormal()
	{ return m_iServerState == ESS_NORMAL; }

	// ��������ȡ�������
	void LoadDataFinish();
	// ��������ʼ�����ʼ����
	void StartProcessingInitData();

	// �Ƿ�����������ͻ���������Ϣ
	bool CanProcessingClientMsg()
	{ return (m_iServerState & ESS_PROCESSINGCLIENTMSG) == ESS_PROCESSINGCLIENTMSG; }

	// ��DB Server����Ϣ
	//bool SendMessageToDB(CSafePtr <CProxyMessage> pMsg);
	// ͨ����ϢID��ȡģ������
	int GetModuleClass(int iMsgID);
private:

	CRunFlag	m_oRunFlag;                         // ����������״̬
	int			m_iServerState;    // ������״̬
	BYTE		m_CacheData[GAMEPLAYER_RECV_BUFF_LEN];
};
#endif //SERVER_GAMESERVER_H
