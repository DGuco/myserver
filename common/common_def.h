/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-2-23
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define		MAX_PACKAGE_LEN				(1024*60)        // 单个包的最大长度
#define		PIPE_SIZE					(0x1000000)    // 共享内存管道大小 16M
#define		DB2S_SHM_KEY				(20000000)
#define		S2DB_SHM_KEY				(20000001)
#define		GAMEPLAYER_RECV_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#define		GAMEPLAYER_SEND_BUFF_LEN	(1024 * 1024 * 2)  //2mb
#define		KEEP_ALIVE_TIME				(2 * 60 * 1000)   //发送
#define		KICK_TIME_OUT_TIME			(4 * 60 * 1000)   //
#define		CHECK_TIME_OUT_TIME			(2 * 60 * 1000)   //
#define		TCP_CONN_TIME_OUT			(2 * 60)  //TCP超时时间2分钟

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
