/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
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
	eTcpListened = 6,
};

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
class CTCPSocket
{
public:
	//
	CTCPSocket();
	//
	~CTCPSocket();
	//��ȡsocketid
	SOCKET GetSocketFD();
	//��ȡ����״̬
	eTcpStatus GetStatus();
	//����
	int ConnectTo(char* szIPAddr, u_short unPort,bool block = true);
	//����
	int ConnectTo(u_long ulIPNetAddr, u_short unPort,bool block = true);
	//�������������Ƿ����ӳɹ�
	int CheckConnectedOk();
	//��ȡ����
	int RecvData();
	//������д��������׼������
	int Write(BYTE* pCode, msize_t nCodeLength);
	//��ȡ���������е�һ����Ϣ
	int GetOneCode(unsigned short& nCodeLength, BYTE* pCode);
	//���socket��fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//�Ƿ���ӵ�fdset��
	bool IsFDSetted(fd_set& pCheckSet);
	//�Ƿ����ӳɹ�
	int CheckConnectedOk();
	//Init tcp Server
	int InitTcpServer(const char* ip,int port);
	//�ر�
	int Close();
public:
	//�Ƿ�ɶ�д
	virtual CanReadWrite() { return 0};
protected:
	CSocket					m_Socket;	     //Socket ������
	int						m_iStatus;	     //����״̬
	SafePointer<CByteBuff>	m_pReadBuff;     //������
	SafePointer<CByteBuff>	m_pWriteBuff;    //д����
};


template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPSocket<RecvBufLen_, SendBufLen_>::CTCPSocket()
{
	m_iStatus = eTcpClosed;
	m_pReadBuff = new CByteBuff(RecvBufLen_);
	m_pWriteBuff = new CByteBuff(SendBufLen_);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
CTCPSocket<RecvBufLen_, SendBufLen_>::~CTCPSocket()
{
	DELETE(m_pReadBuff);
	DELETE(m_pWriteBuff);
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::ConnectTo(char* szIPAddr, u_short unPort, bool block)
{
	m_Socket.Open();
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	m_iStatus = eTcpCreated;
	if (!m_Socket.SetRecvBufSize(RecvBufLen_))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(SendBufLen_))
	{
		Close();
		return -1;
	}

	if (!block)
	{
		m_Socket.SetSocketNoBlock();
	}

	sockaddr_in stTempAddr;
	memset((void*)&stTempAddr, 0, sizeof(sockaddr_in));
	stTempAddr.sin_family = AF_INET;
	stTempAddr.sin_port = htons(unPort);
	stTempAddr.sin_addr.s_addr = inet_addr(szIPAddr);
	int nConRet = m_Socket.Conn(stTempAddr, sizeof(stTempAddr), block);
	if (nConRet == SOCKET_ERROR)
	{
		Close();
		return -2;
	}
	else if (nConRet == 1)
	{
		m_iStatus = eTcpConnecting;
		//�����������У�����Ƿ����ӳɹ�
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::ConnectTo(u_long ulIPNetAddr, u_short unPort, bool block)
{
	m_Socket.Open();
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	m_iStatus = eTcpCreated;
	if (!m_Socket.SetRecvBufSize(RecvBufLen_))
	{
		Close();
		return -1;
	}

	if (!m_Socket.SetSendBufSize(SendBufLen_))
	{
		Close();
		return -1;
	}

	if (!block)
	{
		m_Socket.SetSocketNoBlock();
	}

	sockaddr_in stTempAddr;
	memset((void*)&stTempAddr, 0, sizeof(sockaddr_in));
	stTempAddr.sin_family = AF_INET;
	stTempAddr.sin_port = htons(unPort);
	stTempAddr.sin_addr.s_addr = ulIPNetAddr;
	int nConRet = m_Socket.Conn(stTempAddr, sizeof(stTempAddr), block);
	if (nConRet == SOCKET_ERROR)
	{
		Close();
		return -2;
	}
	else if(nConRet == 1)
	{
		m_iStatus = eTcpConnecting;
		//�����������У�����Ƿ����ӳɹ�
		CheckConnectedOk();
	}

	if (block)
	{
		m_Socket.SetSocketNoBlock();
		m_iStatus = eTcpConnected;
	}
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::CheckConnectedOk()
{
	if (!m_Socket.IsValid())
	{
		return -1;
	}
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000; //10����

	fd_set readfds, writefds, execpfds;
	SOCKET nSocketFd = m_Socket.GetSocket();

	FD_ZERO(&writefds);
	FD_SET(m_Socket.GetSocket(), &writefds);
	readfds = writefds;
	execpfds = writefds;
	int n = select(nSocketFd + 1, &readfds, &writefds, &execpfds, &tv);
	if (n < 0)
	{
		LOG_ERROR("default", "select error({}:{})", errno, strerror(errno));
		Close();
		return -2;
	}

	if (n == 0)
	{
		return 0;
	}

	if (!FD_ISSET(nSocketFd, &readfds) && !FD_ISSET(nSocketFd, &writefds))
	{
		LOG_ERROR("default", "socket {} can't read and write", nSocketFd);
		Close();
		return -3;
	}

	if (m_Socket.IsSocketError())
	{
		LOG_ERROR("default", "connect failure on SOCKET {} error:{} msg:{}.", m_nSocket, errno, strerror(errno))
		Close();
		return -4;
	}
	m_iStatus = eTcpConnected;
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::InitTcpServer(const char* ip, int port)
{
	m_Socket.Open();
	if (!m_Socket.IsValid()) return -1;
	m_iStatus = eTcpCreated;

	//�����׽ӿں�һ������ʹ���еĵ�ַ����
	if (!m_Socket.SetReuseAddr())  return -1;

	bool bRet = 0;
	if (ip != NULL)
	{
		bRet = m_Socket.Bind(port);
	}
	else
	{
		bRet = m_Socket.Bind(ip,port);
	}

	if (!bRet) return -1;

	if (!m_Socket.Listen()) return -1;

	m_Socket.SetSocketNoBlock();
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::Close()
{
	m_Socket.Close();
	m_iStatus = eTcpClosed;
	return 0;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
SOCKET CTCPSocket<RecvBufLen_, SendBufLen_>::GetSocketFD()
{
	return m_Socket.GetSocket();
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
eTcpStatus CTCPSocket<RecvBufLen_, SendBufLen_>::GetStatus()
{
	return m_iStatus;
}

// ����ֵ��-1 ��Socket״̬����-2 �����ջ�����������-3 ���Զ˶Ͽ����ӣ�-4 �����մ���
template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::RecvData()
{
	if (!CanReadWrite())
	{
		return ERR_RECV_NOT_READY;
	}
	int nRetCode =  m_pReadBuff->Recv(m_Socket);
	if (nRetCode == ERR_RECV_WOULD_BLOCK)
	{
		nRetCode = ERR_RECV_OK;
	}
	return nRetCode;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::Write(BYTE* pCode, msize_t nCodeLength)
{
	if (!m_Socket.IsValid())
	{
		return ERR_SEND_NOSOCK;
	}

	if (!pCode)
	{
		return ERR_SEND_NODATA;
	}

	if (!CanReadWrite())
	{
		return ERR_SEND_NOT_READY;
	}

	int retCode = ERR_SEND_OK;
	//�в������ݴ�����
	if (m_pWriteBuff->CanReadLen() > 0)
	{
		int retCode = m_pWriteBuff->Send(m_Socket);
		if (retCode < 0)
		{
			return retCode;
		}
	}

	//û�������ˣ���������0��
	if (m_pWriteBuff->CanReadLen() == 0)
	{
		m_pWriteBuff->Clear();
	}

	//�ѱ��ε�����д�뻺����
	SOCKET nTmSocket = m_Socket.GetSocket();
	if (m_pWriteBuff->WriteBytes(pCode, nCodeLength) == -1)
	{
		return ERR_SEND_NOBUFF;
	}

	if (retCode != ERR_SEND_WOULD_BLOCK)
	{
		//�ٴγ��Է��ͱ���д�뻺����������
		if (m_pWriteBuff->CanReadLen() > 0)
		{
			retCode = m_pWriteBuff->Send(m_Socket);
			if (retCode < 0)
			{
				return retCode;
			}
		}

		//û�������ˣ���������0��
		if (m_pWriteBuff->CanReadLen() == 0)
		{
			m_pWriteBuff->Clear();
		}
	}
	return ERR_SEND_OK;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
int CTCPSocket<RecvBufLen_, SendBufLen_>::GetOneCode(unsigned short& nCodeLength, BYTE* pCode, eByteMode emByte)
{

}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
bool CTCPSocket<RecvBufLen_, SendBufLen_>::AddToFDSet(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_iStatus == eTcpConnected)
	{
		FD_SET(nSocket, &tmFdSet);
		return true;
	}
	else if (m_Socket.IsValid())
	{
		Close();
		return 0;
	}
	return true;
}

template<unsigned int RecvBufLen_, unsigned int SendBufLen_>
bool CTCPSocket<RecvBufLen_, SendBufLen_>::IsFDSetted(fd_set& tmFdSet)
{
	SOCKET nSocket = m_Socket.GetSocket();
	if (nSocket > 0 && m_iStatus == eTcpConnected)
	{
		return  = FD_ISSET(nSocket, tmFdSet);
	}
	return false;
}

#endif //__TCP_CLIENT_H__
