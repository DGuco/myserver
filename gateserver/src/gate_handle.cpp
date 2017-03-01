//
//  gate_handle.cpp
//  gate_handle 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <sys/time.h>
#include "../inc/gate_handle.h"
#include "../inc/gate_ctrl.h"
#include "../../framework/base/base.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/message/tcpmessage.pb.h"
#include "../../framework/net/client_comm_engine.h"


CGateHandle::CGateHandle()
{
	m_pInfo = NULL;
	m_eHandleType = EHandleType_FIRST;
	m_pConnsMap = NULL;
	m_tCheckStatLog = 0;

	memset((void *)&m_tvLastCheck, 0, sizeof(m_tvLastCheck));
}


CGateHandle::~CGateHandle()
{

}


int CGateHandle::Initialize(EMHandleType eHandleType, CDoubleLinkerInfo* pInfo, CONNS_MAP* pMap)
{
}


int CGateHandle::IsToBeBlocked()
{

	return iRet;
}

int CGateHandle::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}


CMyTCPConn* CGateHandle::GetConnByKey(int iKey)
{
	return tpConn;
}


int CGateHandle::SendOneCodeTo(short nCodeLength, BYTE* pbyCode, int  iKey, bool bKeepalive/* = false*/)
{

	
	return 0;
}

int CGateHandle::TransferOneCode(short nCodeLength, BYTE* pbyCode)
{

	return 0;
}

int CGateHandle::DoTransfer()
{

	return 0;
}

int CGateHandle::CheckBlockCodes()
{
	int iCleanCount = 0;

	return iCleanCount;
}

int CGateHandle::PrepareToRun()
{
	return 0;
}

int CGateHandle::Run()
{
	while( True )
	{
		// 一直休眠直到被唤醒
		CondBlock();

		// 真正的转发过程
		DoTransfer();
	
		// 发送完在DoTransfer过程中被Block的数据
		CheckBlockCodes();

		// 打印接收以及发送的信息，统计时长可能大于1分钟
		CheckStatLog();
	}
	
	return 0;
}

void CGateHandle::CheckStatLog()
{
}
