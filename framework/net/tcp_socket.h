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

enum eTcpStatus
{
	eTcpClosed = 0,
	eTcpCreated = 1,
	eTcpConnecting = 2,
	eTcpConnected = 3,
	eTcpRegisting = 4,
	eTcpRegistered = 5,
};

class CTCPSocket
{
public:
	//
	CTCPSocket(unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	CTCPSocket(CSocket socket,unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPSocket();
	//��ȡsocketid
	SOCKET GetSocketFD();
	//socket 
	CSocket& GetSocket();
	//��ȡ����״̬
	eTcpStatus GetStatus();
	//����
	int ConnectTo(const char* szIPAddr, u_short unPort,bool block = true);
	//�������������Ƿ����ӳɹ�
	int CheckConnectedOk();
	//��ȡ����
	int Recv();
	//������д��������׼������
	//sendnow �Ƿ����̷��͵������У�1 ���̷��� 2 ������ֻ�ݴ浽�������е�tcp���ͻ�������д����һ�𷢳�ȥ
	int Write(BYTE* pCode, msize_t nCodeLength,bool sendnow = false);
	//�ѻ��������ݷ���
	int Flush();
	//��ȡ���������е�һ����Ϣ
	int GetOneCode(unsigned short& nCodeLength, BYTE* pCode);
	//���socket��fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//�Ƿ���ӵ�fdset��
	bool IsFDSetted(fd_set& pCheckSet);
	//�Ƿ����ӳɹ�
	int CheckConnectedOk();
	//�ر�
	int Close();
	//
	SafePointer<CByteBuff> GetReadBuff();
	//
	SafePointer<CByteBuff> GetSendBuff();
public:
	virtual int DoRecvLogic() = 0;
	virtual int DoWriteLogic() = 0;
	virtual int DoErrorLogic(int errcode) = 0;
protected:
	CSocket					m_Socket;	     //Socket ������
	int						m_nStatus;	     //����״̬
	int						m_nRecvBuffLen;  //���ܻ�������С
	int						m_nSendBuffLen;  //���ͻ�������С
	SafePointer<CByteBuff>	m_pReadBuff;     //������
	SafePointer<CByteBuff>	m_pWriteBuff;    //д����
};
#endif //__TCP_SOCKET_H__
