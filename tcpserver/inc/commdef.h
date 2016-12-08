//
//  commdef.cpp
//  tcpserver 宏定义
//  Created by 杜国超 on 16/12/1.
//  Copyright © 2016年 杜国超. All rights reserved.
//

#ifndef __TCPDEF_H__
#define __TCPDEF_H__

#include <time.h>
#include <string.h>

/******************************************/
//配置信息暂时写死 以后加上配置
const int port = 13000;        //tcpserver 端口

/******************************************/

#define TCP_BUFFER_LEN             300000               //TCP接收缓冲区大小
#define RECV_QUEUQ_MAX             1024                 //接收队列大小

#define MEDIUM_SIZE

#ifdef SMALL_SIZE
#define MAX_SOCKET_NUM				500					// 单个TCPCtrl支持的连接数
#endif

#ifdef MEDIUM_SIZE
#define MAX_SOCKET_NUM				3500				// 单个TCPCtrl支持的连接数
#endif

#ifdef LARGE_SIZE
#define MAX_SOCKET_NUM				15000				// 单个TCPCtrl支持的连接数
#endif


#define MAX_PORT_NUM				3					// 最大端口数

#define MSG_BASE_LEN 				8 					// 接收或发送给客户端消息的最小字节数
#define MSG_OPT_LEN 				10*1024 			// 接收或发送给客户端消息的最大字节数

#define LISTEN_SOCKET				1					// 监听socket
#define CONNECT_SOCKET				2					// 连接socket

#define RECV_DATA					1					// 标志socket要接收数据
#define SEND_DATA					2					// 标志socket要发送数据

#define IP_LENGTH					20					// IP地址长度

#define MAX_BUF_LEN                 (60*1024)			// 服务器发送给客户端单个包的最大长度
#define RECV_BUF_LEN				(8 * 1024)			// 接收客户端信息的缓冲区
#define MAX_SEND_PKGS_ONCE			100				// 一次从内存管道中取的最大的数据包


#define MAIN_HAVE_SEND_DATA			1					// 后端主服务器发送过数据给某Socket


//enum ENSocketCloseSrcEn
//{
//	CLOSESOCKET_PLAYER			=	1,					// 客户端主动发起断连
//	CLOSESOCKET_MAINSERVER		=	2,					// 后台主服务器发起断连
//};


#define CONFIG_FILE					"../config/tcpsvrd.cfg"
#define LOG_FILE					"../log/tcpsvrd"
#define MAX_PATH                    260                       // 文件的最大路径名
#define INVALID_SOCKET              -1                        // 无效socket句柄
#define SOCKET_ERROR                -1                        // socket api的返回值
#define INVALID_HANDLE_VALUE        ((void * )(-1))           // 无效句柄值
#define closesocket                 close                     // 关闭socket

#define ERRPACKETLENGTH				-2

#define RcMutex                     pthread_mutex_t           // 线程锁
#define ThreadType                  void*                     // 线程类型

typedef void                        *(*ThreadFunc )(void *);  // 线程函数指针

typedef enum _EnRunFlag
{
    reloadcfg = 1,
    tcpexit
} EnRunFlag;


// Socket 连接错误码
typedef enum _EnSocketStatus
{
    Client_Succeed			= 0,	// 客户端连接正常
    Err_ClientClose			= -1,	// 客户端关闭
    Err_ClientTimeout		= -2,	// 客户端超时
    Err_PacketError			= -3,	// 客户端发送的包错误
    Err_TCPBuffOver			= -4,	// TCP缓冲区已满
    Err_SendToMainSvrd		= -5,	// 数据包放入内存管道失败
    Err_System				= -6	// 系统错误，暂时未用
} EnSocketStatus;



// 定义返回结果
typedef enum _EnResult
{
    TCP_SUCCESS = 0,                        // 成功
    TCP_FAILED,                             // 失败
    TCP_CONTINUERECV,                       // 消息还未接收完毕继续接收
    TCP_LENGTHERROR,                        // 接收的消息长度和标实的长度不一致
} EnResult;


// 当前时间
typedef struct _TTime
{
    unsigned int ulYear; 	// 0000-9999
    unsigned int ulMonth; 	// 00-12
    unsigned int ulDay; 	// 01-31
    unsigned int ulHour; 	// 00-23
    unsigned int ulMinute; 	// 00-59
    unsigned int ulSecond; 	// 00-59
} TTime;


// 程序的配置信息
typedef struct _TConfig
{
    int          miSocketTimeOut;						// socket的超时时间
    int          miConnTimeOut;						// socket的超时时间
    int          miCheckTimeGap;
    int          miListenPortNum;						// 保存端口信息
    int          miListenPorts[MAX_PORT_NUM];			// 保存端口信息
    int          miTcpBufLen;							// tcp发送缓冲区大小
    int          miMaxLogCount;
    int          miWriteStatGap;						// log服务器状态的间隔
    int          miShmMax;								// 共享内存最大数(size)
    int          miOpenFileMax;						// 文件句柄最大数
    int          miCoreFileSize;						// core文件大小
} TConfig;


// 和客户端通讯的socket结构
typedef struct _TSocketInfo
{
    int		miSocket;									// socket句柄
    int		miSocketType;								// socket类型：监听socket、连接socket
    int		miSocketFlag;								// socket标志：是否收包
    int		miRecvBytes;								// 接收的字节数
    char	mszClientIP[IP_LENGTH];			// 客户端IP地址
    char	mszMsgBuf[RECV_BUF_LEN];		// 存储发送给gamesvrd的消息
    time_t	mtCreateTime;							// socket的创建时间
    int		miSrcIP;										// 由tcpsvrd发给gamesvrd，记录了该socket的创建时间
    int		miDstIP;										// 由gamesvrd返回给tcpsvrd，值和m_iSrcIP相同
    short	mnSrcPort;									// 由tcpsvrd发给gamesvrd，记录了该socket在数组中的位置
    short	mnDstPort;									// 由gamesvrd返回给tcpsvrd，值和m_iSrcPort
    time_t	mtStamp;									// 接收到数据包的时间戳
    int		miSendFlag;								// 标识mainsvrd是否向其发送了包
    int		miConnectedPort;
    int		miUin;											// 用户唯一标识
} TSocketInfo;

#endif // __TCPDEF_H__
