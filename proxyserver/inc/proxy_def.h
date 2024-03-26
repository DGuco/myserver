//
//  gatedef.h
//  tcpserver �궨��
//  Created by DGuco on 16/12/1.
//  Copyright ? 2016�� DGuco. All rights reserved.
//

#ifndef __PROXY_DEF_H__
#define __PROXY_DEF_H__

#include <time.h>
#include <string.h>

#define TCP_BUFFER_LEN             300000               //TCP���ջ�������С
#define RECV_QUEUQ_MAX             1024                 //���ն��д�С
#define MESSAGE_EXTRA_LEN          10                   //��Ϣ���ⳤ�����������Ѷ�

#define MAX_PORT_NUM                3                    // ���˿���
#define LISTEN_SOCKET                1                    // ����socket
#define CONNECT_SOCKET                2                    // ����socket

#define RECV_DATA                    1                    // ��־socketҪ��������
#define SEND_DATA                    2                    // ��־socketҪ��������

#define IP_LENGTH                    20                    // IP��ַ����

#define sMAX_SEND_PKGS_ONCE            100                // һ�δ��ڴ�ܵ���ȡ���������ݰ�


#define MAIN_HAVE_SEND_DATA            1                    // ��������������͹����ݸ�ĳSocket


//enum ENSocketCloseSrcEn
//{
//	CLOSESOCKET_PLAYER			=	1,					// �ͻ��������������
//	CLOSESOCKET_MAINSERVER		=	2,					// ��̨���������������
//};


#define CONFIG_FILE                    "../config/tcpsvrd.cfg"
#define LOG_FILE                    "../log/tcpsvrd"
#define MAX_PATH                    260                       // �ļ������·����
#define CloseSocket                 close                     // �ر�socket

#define ERRPACKETLENGTH                -2

#define RcMutex                     pthread_mutex_t           // �߳���
#define ThreadType                  void*                     // �߳�����

typedef enum _EnRunFlag
{
	reloadcfg = 1,
	tcpexit
} EnRunFlag;

// ��ǰʱ��
typedef struct _TTime
{
	unsigned int ulYear;    // 0000-9999
	unsigned int ulMonth;    // 00-12
	unsigned int ulDay;    // 01-31
	unsigned int ulHour;    // 00-23
	unsigned int ulMinute;    // 00-59
	unsigned int ulSecond;    // 00-59
} TTime;

// �����������Ϣ
typedef struct _TConfig
{
	int m_iSocketTimeOut;                        // socket�ĳ�ʱʱ��
	int m_iConnTimeOut;                        // socket�ĳ�ʱʱ��
	int m_iCheckTimeGap;
	int m_iListenPortNum;                        // ����˿���Ϣ
	int m_iListenPorts[MAX_PORT_NUM];            // ����˿���Ϣ
	int m_iTcpBufLen;                            // tcp���ͻ�������С
	int m_iMaxLogCount;
	int m_iWriteStatGap;                        // log������״̬�ļ��
	int m_iShmMax;                                // �����ڴ������(size)
	int m_iOpenFileMax;                        // �ļ���������
	int m_iCoreFileSize;                        // core�ļ���С
} TConfig;

typedef struct
{
	int m_iConnIncoming;
	int m_iConnTotal;
	int m_iPkgRecv;
	int m_iPkgSend;
	int m_iPkgSizeRecv;
	int m_iPkgSizeSend;
} TTcpStat;

#endif // __GATE_DEF_H__
