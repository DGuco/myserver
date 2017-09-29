//
//  gate_conn.h
//  gate_conn 管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _PROXY_CONN_H_
#define _PROXY_CONN_H_

#include "../../framework/net/tcp_conn.h"
#include "../../framework/base/servertool.h"

typedef CTCPSocket<RECVBUFLENGTH, POSTBUFLENGTH> CMyTCPSocket;

enum enConnFlags {   
	FLG_CONN_IGNORE = 0,
	FLG_CONN_REDO   = 1,
	FLG_CONN_CTRL   = 2                                                                                           
};  

enum enEntityStates  {
	ENTITY_OFF  = 0, 
	ENTITY_ON   = 1
};

enum enBlockStates {
	BLK_EMPTY    = 0,
	BLK_SAVING   = 1,
	BLK_REDOING  = 2
};  
    
class CMyTCPConn: public CDoubleLinker, public CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH>
{
public:
    CMyTCPConn()
    {
        //初始化双向链表信息
        CDoubleLinker::Init();
    }

    ~CMyTCPConn() {}
    enum enConnParas
    {
#ifdef _DEBUG_
        BLOCKQUEUESIZE = 0x2000,
#else
        BLOCKQUEUESIZE = 0x1000000,
#endif // _DEBUG_
    };
    //建立连接
    int EstConn(int iAcceptFD);
    //连接是否可以接收数据
    int IsConnCanRecv();
	//把socket添加到select集合中
    int RegToCheckSet(fd_set *pCheckSet);
	//判断socket是否在select集合和从
    int IsFDSetted(fd_set *pCheckSet);
	//接受所有数据
    int RecvAllData();
	//获取指定长度的数据
    int GetOneCode(short &nCodeLength, BYTE *pCode);
	//发送数据
    int SendCode(short nCodeLength, BYTE *pCode, int iFlag = FLG_CONN_IGNORE);
    int CleanBlockQueue(int iQueueLength);
	//设置连接状态
    int SetConnState(int iConnState);
    bool IsStateOn();
	//设置上次活跃时间
    int SetLastKeepalive(time_t tNow);
	//获取上次活跃时间
    time_t GetLastKeepalive();

private:
    int m_iConnState;
    time_t m_tLastKeepalive;

#ifdef _POSIX_MT_
	std::mutex m_stMutex;			//操作的互斥变量
#endif // _POSIX_MT_
};

#endif // _PROXY_CONN_H_
