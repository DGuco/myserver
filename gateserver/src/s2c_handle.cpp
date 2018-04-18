//
// Created by dguco on 18-1-30.
//

#include <share_mem.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include "net_work.h"
#include "../inc/s2c_handle.h"
#include "../inc/gate_ctrl.h"

CCodeQueue *CS2cHandle::m_pS2CPipe = NULL;

CS2cHandle::CS2cHandle()
	: CMyThread("CS2cHandle"),
	  m_iSendIndex(0),
	  m_bHasRecv(false),
	  m_iSCIndex(0),
	  m_nSCLength(0)
{
}

CS2cHandle::~CS2cHandle()
{

}

int CS2cHandle::PrepareToRun()
{
	return 0;
}

void CS2cHandle::RunFunc()
{
	//如果有数据需要发送
	CheckWaitSendData();
}

bool CS2cHandle::IsToBeBlocked()
{
	return m_pS2CPipe->IsQueueEmpty();
}

void CS2cHandle::CheckWaitSendData()
{
	int iTmpRet = 0;
	int unTmpCodeLength = 0;

	if (m_iSendIndex < m_oMesHead.socketinfos().size()) {
		//有数据未发送，继续发送
		SendClientData();
		m_bHasRecv = false;
	}
	else {
		//当前没有接收到数据，先接收数据
		if (false == m_bHasRecv) {
			//没有可发送的数据或者发送完成,则接收gate数据
			iTmpRet = RecvServerData();
			//没有数据可接收，则发送队列无数据发送，退出
			if (iTmpRet == 0) {
				return;
			}
			m_bHasRecv = true;
		}//处理已经接收到的数据
		if (m_bHasRecv == true) {
			//组织服务器发送到客户端的数据信息头，设置相关索引和游标
			m_iSendIndex = 0;
			m_iSCIndex = 0;
			m_nSCLength = 0;
			CMessage tmpMes;
			//反序列化消息的CTcpHead,取出发送游标和长度,把数据存入发送消息缓冲区m_szMsgBuf
			iTmpRet = CClientCommEngine::ConvertStreamToMessage(m_acSCMsgBuf,
																unTmpCodeLength,
																&tmpMes,
																NULL,
																&m_iSendIndex);
			CClientCommEngine::CopyMesHead(tmpMes.mutable_msghead(), &m_oMesHead);
			//序列化失败继续发送
			if (iTmpRet < 0) {
				LOG_ERROR("default",
						  "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return {}.",
						  iTmpRet);
				m_oMesHead.Clear();
				m_iSendIndex = 0;
				return;
			}

			//接收成功,取出数据长度
			char *pTmp = m_acSCMsgBuf;
			pTmp += m_iSCIndex;
			m_nSCLength = *(unsigned short *) pTmp;
		}
	}
}

int CS2cHandle::SendClientData()
{
	BYTE *pbTmpSend = NULL;
	unsigned short unTmpShort;
	int nTmpIndex;
	unsigned short unTmpPackLen;

	auto *pSendList = m_oMesHead.mutable_socketinfos();
	//client socket索引非法，不存在要发送的client
	if (m_iSendIndex >= pSendList->size())
		return 0;

	unTmpPackLen = m_nSCLength;
	//发送数据
	if (unTmpPackLen > 0) {
		//根据发送给客户端的数据在m_szSCMsgBuf中的数组下标取出数据
		pbTmpSend = (BYTE *) m_acSCMsgBuf[m_iSCIndex];
		memcpy((void *) &unTmpShort, (const void *) pbTmpSend, sizeof(unsigned short));
		if (unTmpShort != unTmpPackLen) {
			LOG_ERROR("default",
					  "Code length not matched,left length {} is less than body length {}",
					  unTmpPackLen,
					  unTmpShort);
			return -1;
		}
	}

	for (int i = 0; i < pSendList->size(); ++i) {
		//向后移动socket索引
		m_iSendIndex++;
		CSocketInfo tmpSocketInfo = pSendList->Get(m_iSendIndex);
		nTmpIndex = tmpSocketInfo.socketid();
		//socket 非法
		if (nTmpIndex <= 0 || MAX_SOCKET_NUM <= nTmpIndex) {
			LOG_ERROR("default", "Invalid socket index {}", nTmpIndex);
			continue;
		}
		CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
			[&tmpSocketInfo, pbTmpSend, unTmpPackLen, this]
			{
				SendToClientAsync(tmpSocketInfo, (const char *) pbTmpSend, unTmpPackLen);
			}
		);
	}
	return 0;
}

int CS2cHandle::CheckData()
{
	if (!IsToBeBlocked()) {
		//唤醒线程
		WakeUp();
		return 1;
	}
	return 0;
}

int CS2cHandle::RecvServerData()
{
	int unTmpCodeLength = MAX_PACKAGE_LEN;
	if (m_pS2CPipe->GetHeadCode((BYTE *) m_acSCMsgBuf, &unTmpCodeLength) < 0) {
		unTmpCodeLength = 0;
	}
	return unTmpCodeLength;
}

void CS2cHandle::SendToClientAsync(const CSocketInfo &socketInfo, const char *data, unsigned int len)
{
	CAcceptor *pAcceptor = CNetWork::GetSingletonPtr()->FindAcceptor(socketInfo.socketid());
	if (pAcceptor == NULL) {
		LOG_ERROR("default", "CAcceptor has gone, socket = {}", socketInfo.socketid());
		return;
	}

	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送
	*/
	if (pAcceptor->GetCreateTime() != socketInfo.createtime()) {
		LOG_ERROR("default",
				  "sokcet[{}] already closed(tcp createtime:{}:gate createtime:{}) : gate ==> client[{}] bytes failed",
				  socketInfo.socketid(),
				  pAcceptor->GetCreateTime(),
				  socketInfo.createtime(),
				  m_nSCLength);
		return;
	}
	int iTmpCloseFlag = socketInfo.state();
	int iRet = pAcceptor->Send(data, len);
	if (iRet != 0) {
		//发送失败
		CC2sHandle::ClearSocket(pAcceptor, Err_ClientClose);
		LOG_ERROR("default",
				  "send to client {} Failed due to error {}",
				  pAcceptor->GetSocket().GetSocket(),
				  errno);
		return;
	}

	//gameserver 主动关闭
	if (iTmpCloseFlag < 0) {
		CC2sHandle::ClearSocket(pAcceptor, Client_Succeed);
	}
}
