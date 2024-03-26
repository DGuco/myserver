//
//  buffer_event.h
//  Created by DGuco on 18/01/27.
//  Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef SERVER_CBUFFEREVENT_H
#define SERVER_CBUFFEREVENT_H


#include "network_interface.h"
#include "socket.h"

class IBufferEvent: public IReactorHandler
{
public:
	//���캯��
	IBufferEvent(IEventReactor* pReactor,
				 int socket,
				 FuncBufferEventOnDataSend funcOnDataSend,
				 FuncBufferEventOnDataRecv funcOnDataRecv,
				 FuncBufferEventOnDisconnected funcDisconnected);
	//��������
	virtual ~IBufferEvent();
	//��������
	int Send(const void *pData, unsigned int uSize);
	//ͨ��socket����
	int SendBySocket(const void *pData, unsigned int uSize);
	//��ȡ����(���ض�ȡ���ݳ���)
	unsigned int RecvData(void *data, unsigned int size);
	//��ȡ����������ǰ���ݰ����ܳ���
	unsigned short ReadRecvPackLen();
	//��ȡ�����������ݳ���
	unsigned int GetRecvDataSize();
	//��ȡд���������ݳ���
	unsigned int GetSendDataSize();
	//���÷��ͻ��������ֵ
	void SetMaxSendBufSize(unsigned int uSize);
	//��ȡ���ͻ��������ֵ
	unsigned int GetMaxSendBufSize();
	//���ý��ջ��������ֵ
	void SetMaxRecvBufSize(unsigned int uSize);
	//��ȡ���ջ��������ֵ
	unsigned int GetMaxRecvBufSize();
	//���event_base�Ƿ���Ч
	bool IsEventBuffAvailable();
	//��ȡ��ǰ���ݰ����ܳ���
	unsigned short GetRecvPackLen() const;
	//��ǰ���ݰ��Ѷ�ȡ
	void CurrentPackRecved();
	//��ȡsocket
	const CSocket &GetSocket() const;
	//�����Ƿ�����
	bool IsPackageComplete();
protected:
	//���ص�
	static void lcb_OnRead(struct bufferevent *bev, void *arg);
	//д�ص�
	static void lcb_OnWrite(bufferevent *bev, void *arg);
	//����ص�
	static void lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg);
public:    //��ȡevent base
	IEventReactor * GetReactor() override;
	//ע��event
	bool RegisterToReactor() override;
private:
	//�¼��ص�
	virtual void OnEvent(int16 nWhat) = 0;
	//bufferEvent ��Ч����
	virtual void BuffEventUnavailableCall() = 0;
	//event buffer �����ɹ�����
	virtual void AfterBuffEventCreated() = 0;
protected:
	IEventReactor* m_pReactor;
	bufferevent *m_pStBufEv;
	CSocket m_oSocket;
	unsigned int m_uMaxOutBufferSize;
	unsigned int m_uMaxInBufferSize;
	unsigned short m_uRecvPackLen;
	FuncBufferEventOnDataSend m_pFuncOnDataSend;
	FuncBufferEventOnDataRecv m_pFuncOnDataRecv;
	FuncBufferEventOnDisconnected m_pFuncDisconnected;
};


#endif //SERVER_CBUFFEREVENT_H
