//
// Created by DGuco on 17-6-21.
// 客户端连接管理
//

#ifndef SERVER_CLIENT_HANDLE_H
#define SERVER_CLIENT_HANDLE_H

#include <mythread.h>
#include <net_work.h>
#include "base.h"
#include "message_interface.h"
#include "message.pb.h"
#include "byte_buff.h"
#include "../datamodule/inc/player.h"


// 提取消息错误码
enum ClienthandleErrCode
{
	CLIENTHANDLE_SUCCESS = 0,            // 序列化消息成功
	CLIENTHANDLE_QUEUE_CRASH = 1,            // 从管道提取消息出错
	CLIENTHANDLE_QUEUE_EMPTY = 2,            // 从管道取出长度为空
	CLIENTHANDLE_SMALL_LENGTH = 3,            // 小于最小长度
	CLIENTHANDLE_TOTAL_LENGTH = 4,            // tcp转发过来的总长度不匹配
	CLIENTHANDLE_PARSE_FAILED = 5,            // 序列化ClientMessage失败
	CLIENTHANDLE_HASCLOSED = 6,            // 连接池中找不到玩家实体，已经关闭连接
	CLIENTHANDLE_NOTSAMETEAM = 7,            // 不是同一个玩家
	CLIENTHANDLE_NOTEAM = 8,            // 找不到玩家实体
	CLIENTHANDLE_CREATETEAMFAILED = 9,            // 创建玩家实体失败
	CLIENTHANDLE_STATUSINVALID = 10,            // 找到玩家但状态不对
	CLIENTHANDLE_ONLINEFULL = 11,            // 服务器在线已满
	CLIENTHANDLE_MSGINVALID = 12,            // 消息异常
	CLIENTHANDLE_ISNOTNORMAL = 13,            // 服务器状态异常
	CLIENTHANDLE_LOGINLIMITTIME = 14,            // 登陆受限制
	CLIENTHANDLE_LOGINCHECK = 15,            // 登陆去验证
	// CLIENTHANDLE_BUSY						        = 16,		    // 当前玩家有消息在处理中
};

class CClientHandle
{
public:
	//构造函数
	CClientHandle(shared_ptr<CNetWork> pNetWork);
	//析构函数
	virtual ~CClientHandle();
public:
	//准备run
	int PrepareToRun();
	shared_ptr<CByteBuff> &GetRecvBuff();
	shared_ptr<CByteBuff> &GetSendBuff();
	int SendResToPlayer(shared_ptr<CGoogleMessage> pMessage, CPlayer *pPlayer);
	int DealClientMessage(shared_ptr<CMessage> pMsg);
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, IBufferEvent *tmpAcceptor);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *tmpAcceptor);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *tmpAcceptor);
	//发送数据回调
	static void lcb_OnCnsSomeDataSend(IBufferEvent *tmpAcceptor);
	//检测连接超时
	static void lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param);
private:
	//开始监听
	bool BeginListen();
	//客户端断开连接
private:
	shared_ptr<CNetWork> m_pNetWork;
	shared_ptr<CByteBuff> m_pRecvBuff; //客户端上行数据buff
	shared_ptr<CByteBuff> m_pSendBuff; //客户端下行数据buff
};

#endif //SERVER_CLIENT_HANDLE_H
