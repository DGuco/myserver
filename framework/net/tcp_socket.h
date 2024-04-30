/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include "safe_pointer.h"
#include "byte_buff.h"
#include "socket.h"
#include "time_helper.h"
#include "tcp_def.h"

enum eSocketStatus
{
	eSocketClosed = 0,
	eSocketOpen,
	eSocketConnecting,
	eSocketConnected,
	eSocketClosing,
};

class CTCPSocket
{
public:
	//
	CTCPSocket(int RecvBufLen_,
				int SendBufLen_, 
				int MaxRecvBufLen_,
				int MaxSendBufLen_);
	//
	CTCPSocket(CSocket socket,
				int RecvBufLen_,
				int SendBufLen_,
				int MaxRecvBufLen_,
				int MaxSendBufLen_);
			//
	virtual ~CTCPSocket();
	//��ȡsocketid
	SOCKET GetSocketFD();
	//socket 
	CSocket& GetSocket();
	//��ȡ����״̬
	u_short GetStatus();
	//����
	int ConnectTo(const char* szIPAddr, u_short unPort,bool block = true);
	//�������������Ƿ����ӳɹ�
	int CheckConnectedOk();
	//��ȡ����
	int Recv();
	//������д��������׼������
	int Write(BYTE* pCode, int nCodeLength);
	//�ѻ��������ݷ���
	int Flush();
	//���socket��fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//�Ƿ���ӵ�fdset��
	bool IsFDSetted(fd_set& pCheckSet);
	//�ر�
	int Close(bool now = true);
	//
	bool IsValid();
	//
	CSafePtr<CByteBuff> GetReadBuff();
	//
	CSafePtr<CByteBuff> GetSendBuff();
protected:
	CSocket					m_Socket;	     //Socket ������
	u_short					m_nStatus;	     //����״̬
	CSafePtr<CByteBuff>		m_pReadBuff;     //������
	CSafePtr<CByteBuff>		m_pWriteBuff;    //д����
};
#endif //__TCP_SOCKET_H__
