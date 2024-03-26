//
//  connector.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "network_interface.h"
#include "socket.h"
#include "event_reactor.h"
#include "net_addr.h"
#include "buffev_interface.h"
#include "timer_event.h"

class CConnector: public IBufferEvent
{
public:
	enum eConnectorState
	{
		eCS_Disconnected = 0,
		eCS_Connecting,
		eCS_Connected,
	};
public:
	//���캯��
	CConnector(IEventReactor* pReactor,
			   FuncBufferEventOnDataSend funcOnDataSend,
			   FuncBufferEventOnDataRecv funcOnDataRecv,
			   FuncBufferEventOnDisconnected funcDisconnected,
			   int iTargetId,
			   int iPingTick);
	//��������
	virtual ~CConnector(void);
	//��ȡ����ip
	void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
	//����
	bool Connect(const CNetAddr &addr);
	//��������
	bool ReConnect();
	//������ػص�
	void SetCallbackFunc(FuncConnectorOnConnectFailed pOnConnectFailed,
						 FuncConnectorOnConnectted pOnConnected,
						 FuncConnectorOnPingServer pOnPingServer);
	//�ر�����
	void ShutDown();
	//�Ƿ����ӳɹ�
	bool IsConnected();
	//�Ƿ���������
	bool IsConnecting();
	//�Ƿ�Ͽ�����
	bool IsDisconnected();
	int GetTargetId() const;
	//���õ�ǰ״̬
	void SetState(eConnectorState eState);
	//��ǰ����״̬
	eConnectorState GetState();
private:
	//bufferEvent ��Ч����
	void BuffEventUnavailableCall() override;
	//event buffer �����ɹ�����
	void AfterBuffEventCreated() override;
	//�¼��ص�
	void OnEvent(int16 nWhat) override;
private:
	//���ӳɹ�
	void OnConnectted();
private:
	static void lcb_OnPingServer(int fd, short event, void *param);

public:
	static std::shared_ptr<CConnector> NULL_CONN;
private:
	CNetAddr m_oAddr;
	eConnectorState m_eState;
	event m_oConnectEvent;
	int m_iTargetId;
	int m_iPingTick; //��λ��
	CTimerEvent *m_pKeepLiveEvent;
	FuncConnectorOnConnectFailed m_pFuncOnConnectFailed;
	FuncConnectorOnConnectted m_pFuncOnConnectted;
	FuncConnectorOnPingServer m_pFuncOnPingServer;
};

#endif
