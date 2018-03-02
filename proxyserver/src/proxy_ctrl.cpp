//
//  gate_ctrl.cpp
//  gate_ctrl 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include "config.h"
#include "my_assert.h"
#include "acceptor.h"
#include "../inc/proxy_ctrl.h"
#include "client_comm_engine.h"
#include "server_comm_engine.h"

using namespace std;

template<> CProxyCtrl *CSingleton<CProxyCtrl>::spSingleton = NULL;

std::map<int/*key*/, int/*socket id*/> CProxyCtrl::m_mapRegister;

std::map<int/*key*/, int/*socket id*/> CProxyCtrl::m_mapSocket2Key;

char CProxyCtrl::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

CProxyCtrl::CProxyCtrl()
{
	m_pNetWork = new CNetWork(eNetModule::NET_SELECT);
}

CProxyCtrl::~CProxyCtrl()
{

}

int CProxyCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/proxyserver.log", level_enum::trace);
#else
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/proxyserver.log", level_enum::info);
#endif

	// 读取配置
	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingleton().LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		delete pTmpConfig;
		pTmpConfig = NULL;
		exit(0);
	}
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	m_pNetWork->BeginListen(proxyInfo->m_sHost.c_str(),
							proxyInfo->m_iPort,
							&CProxyCtrl::lcb_OnAcceptCns,
							&CProxyCtrl::lcb_OnCnsDisconnected,
							&CProxyCtrl::lcb_OnCnsSomeDataRecv,
							-1,
							CServerConfig::GetSingletonPtr()->GetTcpKeepAlive());
	return 0;
}

int CProxyCtrl::Run()
{
	LOG_INFO("default", "Libevent run with net module {}",
			 event_base_get_method(reinterpret_cast<const event_base *>(CNetWork::GetSingletonPtr()
				 ->GetEventReactor()->GetEventBase())));
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	LOG_INFO("default", "ProxyServer is going to run at {} : {}", proxyInfo->m_sHost.c_str(), proxyInfo->m_iPort);
	LOG_INFO("default", "ProxyServer startup successes");
	m_pNetWork->DispatchEvents();
	return 0;
}

CAcceptor *CProxyCtrl::GetConnByKey(int iKey)
{
	auto it = m_mapRegister.find(iKey);
	if (it != m_mapRegister.end()) {
		return m_pNetWork->FindAcceptor(it->second);
	}
	return NULL;
}

int CProxyCtrl::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}

void CProxyCtrl::lcb_OnAcceptCns(uint32 uId, CAcceptor *pAcceptor)
{
	MY_ASSERT(pAcceptor != NULL, return);
	CNetWork::GetSingletonPtr()->InsertNewAcceptor(uId, pAcceptor);
}

void CProxyCtrl::lcb_OnCnsDisconnected(CAcceptor *pAcceptor)
{
	MY_ASSERT(pAcceptor != NULL, return);
	CNetWork::GetSingletonPtr()->ShutDownAcceptor(pAcceptor->GetSocket().GetSocket());
}

void CProxyCtrl::lcb_OnCnsSomeDataRecv(CAcceptor *pAcceptor)
{
	MY_ASSERT(pAcceptor != NULL, return);
	//消息不完整
	if (!pAcceptor->IsPackageComplete()) {
		return;
	}
	PACK_LEN unTmpLen = pAcceptor->GetRecvPackLen();
	pAcceptor->RecvData(m_acRecvBuff + sizeof(PACK_LEN), unTmpLen - sizeof(PACK_LEN));
	auto it = m_mapSocket2Key.find(pAcceptor->GetSocket().GetSocket());
	//未注册
	if (it != m_mapSocket2Key.end()) {
		CProxyCtrl::GetSingletonPtr()->DealRegisterMes(pAcceptor, m_acRecvBuff + sizeof(PACK_LEN));
	}
	else {    //未注册
		*(PACK_LEN *) m_acRecvBuff = (pAcceptor->GetRecvPackLen());
		CProxyCtrl::GetSingletonPtr()->TransferOneCode(pAcceptor, unTmpLen);
	}
}

int CProxyCtrl::DealRegisterMes(CAcceptor *pAcceptor, char *acTmpBuf)
{
	MY_ASSERT(pAcceptor != NULL, return -1);
//	// 8字节对齐补充长度
//	// 这里可以不取，因为不关心
//	unsigned short unAddLen = *((unsigned short*) (acTmpBuf + 2));
	CProxyHead stTmpProxyHead;
	// CProxyHead长度
	unsigned short iProxyHeadSize = *((unsigned short *) (acTmpBuf + 2));
	// 获取CProxyHead
	if (stTmpProxyHead.ParseFromArray(acTmpBuf + 4, iProxyHeadSize) == false) {
		LOG_ERROR("default", "CProxyHead::ParseFromArray error, stream_length is {}", iProxyHeadSize);
		return -1;
	}
	SOCKET iSocket = pAcceptor->GetSocket().GetSocket();
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	// 目标服务器是proxy，id不匹配或者不是注册消息，则直接关闭连接
	if ((stTmpProxyHead.dstfe() != FE_PROXYSERVER)
		|| (stTmpProxyHead.dstid() != proxyInfo->m_iServerId)
		|| (stTmpProxyHead.opflag() != enMessageCmd::MESS_REGIST)) {
		LOG_ERROR("default",
				  "Error CProxyHead is invalid, fd = {}, Src(FE = {} : ID = {}), Dst(FE = {} : ID = {}), OpFlag = {}, TimeStamp = %ld.",
				  pAcceptor->GetSocket().GetSocket(),
				  stTmpProxyHead.srcfe(),
				  stTmpProxyHead.srcid(),
				  stTmpProxyHead.srcfe(),
				  stTmpProxyHead.dstid(),
				  stTmpProxyHead.opflag(),
				  stTmpProxyHead.timestamp());
		CloseConnection(iSocket);
		return -1;
	}


#ifdef _DEBUG_
	LOG_DEBUG("default", "---- Recv Msg ----");
	LOG_DEBUG("default", "[{}]", stTmpProxyHead.ShortDebugString().c_str());
#endif

	// 检查该链接是否已占用，通过类型和ID判断
	int iKey = MakeConnKey(stTmpProxyHead.srcfe(), stTmpProxyHead.srcid());
	auto it = m_mapRegister.find(iKey);
	if (it != m_mapRegister.end()) {
		// 该连接已经存在
		LOG_ERROR("default", "conn(fe={} : id={} : key={}) exist, can't regist again.",
				  stTmpProxyHead.srcfe(), stTmpProxyHead.srcid());
		CloseConnection(iSocket);
		return -1;
	}

	//注册成功
	m_mapRegister.insert(std::make_pair(iKey, iSocket));
	m_mapSocket2Key.insert(std::make_pair(iSocket, iKey));
	return 0;
}

/**
  *函数名          : TransferOneCode
  *功能描述        : 转发一个数据包
**/
int CProxyCtrl::TransferOneCode(CAcceptor *pAcceptor, unsigned short nCodeLength)
{
	CProxyMessage stTmpMessage;

	int iTempRet = 0;
	if (nCodeLength <= 0) {
		LOG_INFO("default", "In TransferOneCode, invalid input params.");
		return -1;
	}

	unsigned short unOffset = 0;
	int iRet = ServerCommEngine::ConvertStreamToMsg(m_acRecvBuff, nCodeLength, &stTmpMessage);
	if (iRet < 0) {
		LOG_INFO("default", "In TransferOneCode, ConvertStreamToCSHead return {}.", iRet);
		return -1;
	}

	CProxyHead stTmpHead = stTmpMessage.msghead();
	LOG_INFO("default", "TransMsg({}).", nCodeLength);

#ifdef _DEBUG_
	LOG_INFO("default", "---- Recv Msg ----");
	LOG_INFO("default", "[{}]", stTmpHead.ShortDebugString().c_str());
#endif

	LOG_INFO("default", "Transfer code begin, from(FE = {} : ID = {}) to(FE = {} : ID = {}), timestamp = %ld.",
			 stTmpHead.srcfe(), stTmpHead.srcid(),
			 stTmpHead.dstfe(), stTmpHead.dstid(), stTmpHead.timestamp());

	// 处理直接发送到 proxy 的消息
	if (stTmpHead.dstfe() == FE_PROXYSERVER) {
		switch (stTmpHead.opflag()) {
		case enMessageCmd::MESS_KEEPALIVE: {
			CProxyHead stRetHead;
			ServerInfo *serverInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
			stRetHead.set_srcfe(FE_PROXYSERVER);
			stRetHead.set_srcid(serverInfo->m_iServerId);
			stRetHead.set_dstfe(stTmpHead.srcfe());
			stRetHead.set_dstid(stTmpHead.srcid());
			stRetHead.set_opflag(enMessageCmd::MESS_KEEPALIVE);
			stRetHead.set_timestamp(GetMSTime());

			// keepalive的包长度一般都很短
			char message_buffer[1024];

			PACK_LEN unHeadLen = stRetHead.ByteSize();
			PACK_LEN unAddLen = ((6 + unHeadLen) % 8);
			if (unAddLen > 0) {
				unAddLen = (8 - unAddLen);
			}
			PACK_LEN unTotalLen = (unHeadLen + 6 + unAddLen);

			int typeLen = sizeof(PACK_LEN);
			*((PACK_LEN *) message_buffer) = unTotalLen;
			*((PACK_LEN *) (message_buffer + typeLen * 1)) = unAddLen;
			*((PACK_LEN *) (message_buffer + typeLen * 2)) = unHeadLen;

			if (stRetHead.SerializeToArray((message_buffer + 6), sizeof(message_buffer) - 6) == false) {
				LOG_INFO("default", "send keepalive to (FE = {} : ID = {}), CProxyHead::SerializeToArray failed.",
						 stRetHead.dstfe(), stRetHead.dstid());
			}
			else {
				int iKey = MakeConnKey(stRetHead.dstfe(), stRetHead.dstid());
				int iRet = SendOneCodeTo(unTotalLen, (BYTE *) message_buffer, iKey, true);
				if (iRet != 0) {
					LOG_INFO("default", "send keepalive to (FE = {} : ID = {}), SendOneCodeTo failed, iRet = {}.",
							 stRetHead.dstfe(), stRetHead.dstid(), iRet);
				}
				else {
					LOG_INFO("default",
							 "send keepalive to (FE = {} : ID = {}) succeed.",
							 stRetHead.dstfe(),
							 stRetHead.dstid());
				}
			}

			break;
		}
		default: {
			LOG_INFO("default",
					 "unknown command id {}, from(FE = {} : ID = {}) to(FE = {} : ID = {}), timestamp = %ld.",
					 stTmpHead.opflag(),
					 stTmpHead.srcfe(),
					 stTmpHead.srcid(),
					 stTmpHead.dstfe(),
					 stTmpHead.dstid(),
					 stTmpHead.timestamp());
			break;
		}
		}

		return 0;
	}

	int iKey = MakeConnKey(stTmpHead.dstfe(), stTmpHead.dstid());
	iTempRet = SendOneCodeTo(nCodeLength, (BYTE *) m_acRecvBuff, iKey, true);
	m_stStatLog.iSndCnt++;
	m_stStatLog.iSndSize += nCodeLength;
	if (iTempRet) {
		LOG_INFO("default", "transfer one code from (FE = {} : ID = {}) to (FE = {} : ID = {}) failed of {}.",
				 stTmpHead.srcfe(), stTmpHead.srcid(), stTmpHead.dstfe(), stTmpHead.dstid(), iTempRet);
		m_stStatLog.iSndCnt--;
		m_stStatLog.iSndSize -= nCodeLength;
	}

	LOG_INFO("default", "Transfer code ended.");

	return 0;
}

int CProxyCtrl::SendOneCodeTo(short nCodeLength, BYTE *pbyCode, int iKey, bool bKeepalive/* = false*/)
{
	CAcceptor *pWriteConn = NULL;
	int iTempRet = 0;

	if (nCodeLength <= 0 || !pbyCode) {
		LOG_ERROR("default", "While send one code to (key={}), null code.", iKey);
		return -1;
	}

	pWriteConn = GetConnByKey(iKey);
	if (!pWriteConn) {
		LOG_ERROR("default", "While send one code to (key={}), invalid key.", iKey);
		return -1;
	}

	iTempRet = pWriteConn->Send(pbyCode, nCodeLength);
	if (iTempRet == 0 && bKeepalive == true) {
		// 设置keepalive时间
//		pWriteConn->SetLastKeepalive(time(NULL));
	}
	return iTempRet;
}

void CProxyCtrl::CloseConnection(int socket)
{
	auto it = m_mapSocket2Key.find(socket);
	if (it != m_mapSocket2Key.end()) {
		m_mapRegister.erase(it->second);
		m_mapSocket2Key.erase(it);
	}
	m_pNetWork->ShutDownAcceptor(socket);
}
