/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define		MAX_PACKAGE_LEN				(1024*60)        // ����������󳤶�
#define		PIPE_SIZE					(0x1000000)    // �����ڴ�ܵ���С 16M
#define		DB2S_SHM_KEY				(20000000)
#define		S2DB_SHM_KEY				(20000001)
#define		GAMEPLAYER_RECV_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#define		GAMEPLAYER_SEND_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#define		KEEP_ALIVE_TIME				(2 * 60 * 1000)   //����
#define		KICK_TIME_OUT_TIME			(4 * 60 * 1000)   //
#define		CHECK_TIME_OUT_TIME			(2 * 60 * 1000)   //
#define		TCP_CONN_TIME_OUT			(2 * 60)  //TCP��ʱʱ��2����

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
