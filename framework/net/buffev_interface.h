//
//  buffer_event.h
//  Created by DGuco on 18/01/27.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_CBUFFEREVENT_H
#define SERVER_CBUFFEREVENT_H


#include "network_interface.h"

class IBufferEvent: public IReactorHandler
{
public:
	//构造函数
	IBufferEvent();
	IBufferEvent(IEventReactor *pReactor, bufferevent *buffevent);
	//析构函数
	virtual ~IBufferEvent();
	//发送数据
	PipeResult Send(const void *pData, unsigned int uSize);
	//获取数据(返回读取数据长度)
	unsigned int RecvData(evbuffer *buff);
	unsigned int RecvData(char *data, unsigned int size);
	//获取读缓冲区数据长度
	unsigned int GetRecvDataSize();
	//获取写缓冲区数据长度
	unsigned int GetSendDataSize();
	//设置发送缓冲区最大值
	void SetMaxSendBufSize(unsigned int uSize);
	//获取发送缓冲区最大值
	unsigned int GetMaxSendBufSize();
	//设置接收缓冲区最大值
	void SetMaxRecvBufSize(unsigned int uSize);
	//获取接收缓冲区最大值
	unsigned int GetMaxRecvBufSize();
	//检测event_base是否有效
	bool IsEventBuffAvailable();

public:    //获取event base
	IEventReactor *GetReactor() override;
	//注册event
	bool RegisterToReactor() override;
	//卸载event
	bool UnRegisterFromReactor() override;
private:
	//bufferEvent 无效处理
	virtual void BuffEventAvailableCall() = 0;
	//event buffer 创建成功后处理
	virtual void AfterBuffEventCreated() = 0;
protected:
	IEventReactor *m_pReactor;
	bufferevent *m_pStBufEv;
	unsigned int m_uMaxOutBufferSize;
	unsigned int m_uMaxInBufferSize;
};


#endif //SERVER_CBUFFEREVENT_H
