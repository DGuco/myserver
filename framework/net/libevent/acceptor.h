//
//  acceptor.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "base.h"
#include "buffev_interface.h"
#include "net_inc.h"
#include "socket.h"
#include "event_reactor.h"
#include "timer_event.h"
#include "net_addr.h"


class CAcceptor: public IBufferEvent
{
	enum eAcceptorState
	{
		eAS_Disconnected = 0,
		eAS_Connected,
	};

public:
	//���캯��
	CAcceptor(SOCKET socket,
			  IEventReactor *pReactor,
			  CNetAddr *netAddr,
			  FuncBufferEventOnDataSend funcOnDataSend,
			  FuncBufferEventOnDataRecv funcOnDataRecv,
			  FuncBufferEventOnDisconnected funcDisconnected);
	//��������
	virtual ~CAcceptor();
	//��ȡ�����ӵ�ip
	void GetRemoteIpAddress(char *szBuf, unsigned int uBufSize);
	//�ر�����
	void ShutDown();
	//��ǰ�Ƿ�����
	bool IsConnected();
	//��ȡ����ʱ��
	time_t GetCreateTime();
	//��ȡ�ϴλ�Ծʱ��
	time_t GetLastKeepAlive();
	//�����ϴλ�Ծʱ��
	void SetLastKeepAlive(time_t tmpLastKeepAlive);
private:
	//bufferEvent ��Ч����
	void BuffEventUnavailableCall() override;
	//event buffer �����ɹ�����
	void AfterBuffEventCreated() override;
private:
	//��ȡ����״̬
	eAcceptorState GetState();
	//��������״̬
	void SetState(eAcceptorState eState);
	//�¼��ص�
	void OnEvent(int16 nWhat) override;
private:
	CNetAddr* m_pNetAddr;
	eAcceptorState m_eState;
	time_t m_tCreateTime;
	time_t m_tLastKeepAlive;
};

#endif