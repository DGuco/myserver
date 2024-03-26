//
//  net_work.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
//


#ifndef _NET_WORK_H_
#define _NET_WORK_H_

#include <map>
#include <queue>
#include <unordered_map>
#include <server_tool.h>
#include "network_interface.h"
#include "event_reactor.h"
#include "mythread.h"
#include "listener.h"
#include "buffev_interface.h"
#include "acceptor.h"

using namespace std;

class CNetWork final: public CSingleton<CNetWork>
{
public:
	typedef unordered_map<unsigned int, CConnector *> MAP_CONNECTOR;

	typedef unordered_map<unsigned int, CAcceptor *> MAP_ACCEPTOR;

	typedef unordered_map<unsigned int, CFileListener *> MAP_FILELISTENER;

	typedef std::queue<CSystemSignal *> Queue_TimerOrSignals;
public:
	//���캯��
	CNetWork();
	//��������
	virtual ~CNetWork();

	//��ʼ����
	bool BeginListen(const char *szNetAddr,
					 unsigned int uPort,
					 FuncAcceptorOnNew pOnNew,
					 FuncBufferEventOnDataSend funcAcceptorOnDataSend,
					 FuncBufferEventOnDataSend funcAcceptorOnDataRecv,
					 FuncBufferEventOnDataSend funcAcceptorDisconnected,
					 FuncOnTimeOut funcAcceptorTimeOut,
					 int listenQueue,
					 unsigned int uCheckPingTickTime);
	//��������
	void EndListen();
	//����
	bool Connect(const char *szNetAddr,
				 uint16 uPort,
				 int targetId,
				 FuncBufferEventOnDataSend funcOnSomeDataSend,
				 FuncBufferEventOnDataSend funcOnSomeDataRecv,
				 FuncBufferEventOnDisconnected funcOnDisconnected,
				 FuncConnectorOnConnectFailed funcOnConnectFailed,
				 FuncConnectorOnConnectted funcOnConnectted,
				 FuncConnectorOnPingServer funcOnPingServer,
				 unsigned int uPingTick);
	//�����ļ��仯
// 	bool ListenFile(string filePath,
// 					FuncFileListenerOnEvent funcFileListenerOnEvent,
// 					int flags);
	//�ر�acceptor
	bool ShutDownAcceptor(unsigned int uId);
	//�����źŻص�
	void RegisterSignalHandler(unsigned int uSignal, FuncOnSignal pFunc, void *pContext);
	//�ر�connector
	bool ShutDownConnectorEx(unsigned int uId);
	//����
	void DispatchEvents();
	//����connector
	CConnector *FindConnector(unsigned int uId);
	//����acceptor
	CAcceptor *FindAcceptor(unsigned int uId);
	//����µ�acceptor
	void InsertNewAcceptor(unsigned int uid, CAcceptor *pAcceptor);
	//����µ�connector
	void InsertNewConnector(unsigned int uid, CConnector *pConnector);
	//����µ�fileListener
	void InsertNewFileListener(unsigned int uid, CFileListener *pFileListener);
	//��ȡevent
	IEventReactor *GetEventReactor();
	//��ȡ����map
	MAP_ACCEPTOR &GetAcceptorMap();
	//��ȡ�ļ�����map
	MAP_FILELISTENER &GetFileListenerMap();
private:
	//�µ����� accept�ص�
	static void lcb_OnAccept(IEventReactor *pReactor, SOCKET socket, sockaddr *sa);
private:
	//����acceptor
	void NewAcceptor(IEventReactor *pReactor, SOCKET socket, sockaddr *sa);
private:
	IEventReactor *m_pEventReactor;
	unsigned int m_uGcTime;

	MAP_CONNECTOR m_mapConnector;
	MAP_ACCEPTOR m_mapAcceptor;
	MAP_FILELISTENER m_mapFileListener;
	Queue_TimerOrSignals m_qTimerOrSignals;
	CListener *m_pListener;
	CTimerEvent *m_pCheckTimerOut;
	int m_iPingCheckTime;  //��λ����

	FuncAcceptorOnNew m_pOnNew;
	FuncBufferEventOnDataSend m_pFuncAcceptorOnDataSend;
	FuncBufferEventOnDataRecv m_pFuncAcceptorOnDataRecv;
	FuncBufferEventOnDisconnected m_pFuncAcceptorDisconnected;
};

#endif
