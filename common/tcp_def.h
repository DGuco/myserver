/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_DEF_H__
#define __TCP_DEF_H__

#define		MAX_PACKAGE_LEN				(1024 * 64)   // ����������󳤶�
#define		PIPE_SIZE					(0x1000000)    // �����ڴ�ܵ���С 16M
#define		DB2S_SHM_KEY				(20000000)
#define		S2DB_SHM_KEY				(20000001)
#define		KEEP_ALIVE_TIME				(2 * 60 * 1000)   //����
#define		KICK_TIME_OUT_TIME			(4 * 60 * 1000)   //
#define		CHECK_TIME_OUT_TIME			(2 * 60 * 1000)   //
#define		TCP_CONN_TIME_OUT			(2 * 60)  //TCP��ʱʱ��2����
#define		TCP_CHECK_BUFF_RESIZE		(5 * 60)  //����������ʱ��5����
#define     GAMEPLAYER_RECV_BUFF_LEN    (1024 * 1024 * 1)  //1mb
#define     GAMEPLAYER_SEND_BUFF_LEN    (1024 * 1024 * 1)  //1mb
#define     SERVER_CLIENT_RECV_BUFF		(1024 * 1024 * 8)  //2mb
#define     SERVER_CLIENT_SEND_BUFF		(1024 * 1024 * 8)  //2mb
#define     SERVER_CLIENT_RECV_BUFF_MAX (1024 * 1024 * 32)  //2mb
#define     SERVER_CLIENT_SEND_BUFF_MAX (1024 * 1024 * 32)  //2mb
#define		TCP_SERVER_RECV_BUFF_LEN	(1024 * 1024 * 2)  //tcp socket ���ջ�����2mb
#define		TCP_SERVER_SEND_BUFF_LEN	(1024 * 1024 * 2)  //tcp socket ���ͻ�����2mb
#define		PROXY_SERVER_RECV_BUFF		(1024 * 1024 * 16)  //tcp socket ���ջ�����2mb
#define		PROXY_SERVER_SEND_BUFF		(1024 * 1024 * 16)  //tcp socket ���ͻ�����2mb
#define		PROXY_SERVER_RECV_BUFF_MAX	(1024 * 1024 * 64)  //tcp socket ���ջ�����2mb
#define		PROXY_SERVER_SEND_BUFF_MAX	(1024 * 1024 * 64)  //tcp socket ���ͻ�����2mb

typedef unsigned short mshead_size;

// Socket ���Ӵ�����
typedef enum _EnSocketStatus
{
	Client_Succeed = 0,    // �ͻ�����������
	Err_ClientClose = -1,    // �ͻ��˹ر�
	Err_ClientTimeout = -2,    // �ͻ��˳�ʱ
	Err_PacketError = -3,    // �ͻ��˷��͵İ�����
	Err_TCPBuffOver = -4,    // TCP����������
	Err_SendToMainSvrd = -5,    // ���ݰ������ڴ�ܵ�ʧ��
	Err_System = -6    // ϵͳ������ʱδ��
} EnSocketStatus;

// ���巵�ؽ��
typedef enum _EnResult
{
	TCP_SUCCESS = 0,                        // �ɹ�
	TCP_FAILED,                             // ʧ��
	TCP_CONTINUERECV,                       // ��Ϣ��δ������ϼ�������
	TCP_LENGTHERROR,                        // ���յ���Ϣ���Ⱥͱ�ʵ�ĳ��Ȳ�һ��
} EnResult;
#endif //__COMMON_DEF_H__
