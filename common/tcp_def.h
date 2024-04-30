/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_DEF_H__
#define __TCP_DEF_H__

#define		MAX_PACKAGE_LEN				(1024 * 64)   // 单个包的最大长度
#define		PIPE_SIZE					(0x1000000)    // 共享内存管道大小 16M
#define		DB2S_SHM_KEY				(20000000)
#define		S2DB_SHM_KEY				(20000001)
#define		KEEP_ALIVE_TIME				(2 * 60 * 1000)   //发送
#define		KICK_TIME_OUT_TIME			(4 * 60 * 1000)   //
#define		CHECK_TIME_OUT_TIME			(2 * 60 * 1000)   //
#define		TCP_CONN_TIME_OUT			(2 * 60)  //TCP超时时间2分钟
#define		TCP_CHECK_BUFF_RESIZE		(5 * 60)  //缓冲区回收时间5分钟
#define     GAMEPLAYER_RECV_BUFF_LEN    (1024 * 1024 * 1)  //1mb
#define     GAMEPLAYER_SEND_BUFF_LEN    (1024 * 1024 * 1)  //1mb
#define     SERVER_CLIENT_RECV_BUFF		(1024 * 1024 * 8)  //2mb
#define     SERVER_CLIENT_SEND_BUFF		(1024 * 1024 * 8)  //2mb
#define     SERVER_CLIENT_RECV_BUFF_MAX (1024 * 1024 * 32)  //2mb
#define     SERVER_CLIENT_SEND_BUFF_MAX (1024 * 1024 * 32)  //2mb
#define		TCP_SERVER_RECV_BUFF_LEN	(1024 * 1024 * 2)  //tcp socket 接收缓冲区2mb
#define		TCP_SERVER_SEND_BUFF_LEN	(1024 * 1024 * 2)  //tcp socket 发送缓冲区2mb
#define		PROXY_SERVER_RECV_BUFF		(1024 * 1024 * 16)  //tcp socket 接收缓冲区2mb
#define		PROXY_SERVER_SEND_BUFF		(1024 * 1024 * 16)  //tcp socket 发送缓冲区2mb
#define		PROXY_SERVER_RECV_BUFF_MAX	(1024 * 1024 * 64)  //tcp socket 接收缓冲区2mb
#define		PROXY_SERVER_SEND_BUFF_MAX	(1024 * 1024 * 64)  //tcp socket 发送缓冲区2mb

typedef unsigned short mshead_size;

// Socket 连接错误码
typedef enum _EnSocketStatus
{
	Client_Succeed = 0,    // 客户端连接正常
	Err_ClientClose = -1,    // 客户端关闭
	Err_ClientTimeout = -2,    // 客户端超时
	Err_PacketError = -3,    // 客户端发送的包错误
	Err_TCPBuffOver = -4,    // TCP缓冲区已满
	Err_SendToMainSvrd = -5,    // 数据包放入内存管道失败
	Err_System = -6    // 系统错误，暂时未用
} EnSocketStatus;

// 定义返回结果
typedef enum _EnResult
{
	TCP_SUCCESS = 0,                        // 成功
	TCP_FAILED,                             // 失败
	TCP_CONTINUERECV,                       // 消息还未接收完毕继续接收
	TCP_LENGTHERROR,                        // 接收的消息长度和标实的长度不一致
} EnResult;
#endif //__COMMON_DEF_H__
