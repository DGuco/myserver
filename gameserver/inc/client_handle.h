//
// Created by DGuco on 17-6-21.
// 客户端连接管理
//

#ifndef SERVER_CLIENT_HANDLE_H
#define SERVER_CLIENT_HANDLE_H

#include <mythread.h>
#include <net_work.h>
#include <code_queue.h>
#include "base.h"
#include "message_interface.h"
#include "message.pb.h"
#include "byte_buff.h"
#include "player.h"


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

class CClientHandle final: public CMyThread
{
public:
	//构造函数
	CClientHandle();
	//析构函数
	virtual ~CClientHandle();
public:
	//准备run
	int PrepareToRun();
	//创建通信共享内存管道
	void CreatePipe();
	//回复消息
	int SendResToPlayer(shared_ptr<CGooMess> pMessage, CPlayer *pPlayer);
	//处理客户端上行消息
	int DealClientMessage(shared_ptr<CMessage> pMsg);
	//断开连接
	void DisconnectClient(int iSocket, time_t tCreateTime);
	//推送消息
	int Push(int cmd, shared_ptr<CGooMess> pMessage, stPointList *pTeamList);
	//接收客户端数据包
	void RecvClientData();
	int SendResponse(shared_ptr<CGooMess> pMessag, shared_ptr<CMesHead> mesHead);
public:
	void RunFunc() override;
	bool IsToBeBlocked() override;
private:
	//gateserver ==> gameserver
	std::shared_ptr<CCodeQueue> m_C2SCodeQueue;
	//gameserver ==> gateserver
	std::shared_ptr<CCodeQueue> m_S2CCodeQueue;
	shared_ptr<CByteBuff> m_pRecvBuff; //客户端上行数据buff
	shared_ptr<CByteBuff> m_pSendBuff; //客户端下行数据buff
};

#endif //SERVER_CLIENT_HANDLE_H
