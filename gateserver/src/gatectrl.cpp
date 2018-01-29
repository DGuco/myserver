//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include "commondef.h"
#include "client_comm_engine.h"
#include "codequeue.h"
#include "sharemem.h"
#include "acceptor.h"
#include "../inc/gatectrl.h"

CGateCtrl::CGateCtrl()
{
}

CGateCtrl::~CGateCtrl()
{
}

int CGateCtrl::Initialize()
{
	m_iRunFlag = 0;
	m_iSendIndex = 0;
	m_bHasRecv = 0;
	m_pNetWork = new CNetWork();
	CreatePipe();
	return 0;
}

int CGateCtrl::Run()
{
	bool bRet = BeginListen();
	if (!bRet) {
		LOG_ERROR("default", "Listen failed");
		return 0;
	}
	LOG_NOTICE("default", "GateServet is runing....");
	m_pNetWork->DispatchEvents();
	return 0;
}

int CGateCtrl::SetRunFlag(int iRunFlag)
{
	m_iRunFlag = iRunFlag;
	return 0;
}

/**
  * 函数名          : CGateCtrl::CreatePipe
  * 功能描述        : 创建和游戏服通信的共享内存管道
  * 返回值         ： int(成功：0 失败：错误码)
**/
int CGateCtrl::CreatePipe()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

	////////////////////////////////mS2CPipe/////////////////////////////////////////
	system("touch ./scpipefile");
	char *pcTmpSCPipeID = getenv("SC_PIPE_ID");
	int iTmpSCPipeID = 0;
	if (pcTmpSCPipeID) {
		iTmpSCPipeID = atoi(pcTmpSCPipeID);
	}
	key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
	BYTE *pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
	MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize, EIMode::SHM_INIT);
	mS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);

	////////////////////////////////mC2SPipe/////////////////////////////////////////
	system("touch ./cspipefile");
	char *pcTmpCSPipeID = getenv("CS_PIPE_ID");
	int iTmpCSPipeID = 0;
	if (pcTmpCSPipeID) {
		iTmpCSPipeID = atoi(pcTmpCSPipeID);
	}
	key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
	BYTE *pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
	MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize, EIMode::SHM_INIT);
	mC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);
	return 0;
}

bool CGateCtrl::BeginListen()
{
	ServerInfo *gateInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GATESERVER);
	return m_pNetWork->BeginListen(gateInfo->m_sHost.c_str(),
								   (short) gateInfo->m_iPort,
								   &OnAcceptCns,
								   &OnCnsDisconnected,
								   &OnCnsSomeDataSend,
								   &OnCnsSomeDataRecv);
}


/**
  * 函数名          : CGateCtrl::RecvClientData
  * 功能描述        : 接客户端的数据组织客户端数据为服务器其间数据传格式存入消息包缓冲区
  * 返回值          ：int
**/
int CGateCtrl::RecvClientData(int iSocketFd)
{
	int iTmpRet = 0;
	int iTmpRecvBytes = 0;
	int iTmpOffset;
	int nRecvAllLen;
	time_t tTempTime;
	char *pTemp;
	char *pTemp1;

	m_pSocketInfo = &m_astSocketInfo[iSocketFd];
	int iTmpSocket = m_pSocketInfo->m_iSocket;
	iTmpOffset = m_pSocketInfo->m_iRecvBytes;
	//读取tcp数据
	iTmpRecvBytes =
		TcpRead(iTmpSocket, m_pSocketInfo->m_szMsgBuf + iTmpOffset, sizeof(m_pSocketInfo->m_szMsgBuf) - iTmpOffset);
	//客户端关闭连接
	if (iTmpRecvBytes <= 0) {
		LOG_ERROR("default", "Client[%s] close the tcp connection,socket id = %d,the client's port = %d",
				  m_pSocketInfo->m_szClientIP, m_pSocketInfo->m_iSocket, m_pSocketInfo->m_iConnectedPort);
		ClearSocketInfo(Err_ClientClose);
		return -1;
	}

	//统计接受信息
	m_stTcpStat.m_iPkgSizeRecv += iTmpRecvBytes;
	//增加收到的总字节数
	m_pSocketInfo->m_iRecvBytes = m_pSocketInfo->m_iRecvBytes + iTmpRecvBytes;

	pTemp1 = m_pSocketInfo->m_szMsgBuf;
	nRecvAllLen = m_pSocketInfo->m_iRecvBytes;

	// 记录该socket接收客户端数据的时间
	time(&tTempTime);
	m_pSocketInfo->m_tStamp = tTempTime;

	while (1) {
		if (nRecvAllLen <= 0) {
			break;
		}
		//客户端上行数据长度(除去头部)
		MSG_LEN_TYPE unTmpDataLen = 0;
		MesHead tmpHead;
		//解析数据包头部信息，解析完成后指针指向数据包的末尾
		iTmpRet = ClientCommEngine::ParseClientStream((const void **) &pTemp1,
													  nRecvAllLen,
													  &tmpHead,
													  unTmpDataLen);
		//继续接收
		if (iTmpRet == 1) {
			break;
		}
		if (iTmpRet < 0) {
			ClearSocketInfo(Err_PacketError);
			return iTmpRet;
		}
		//组织转发消息
		if (0 == iTmpRet /*&& tmpHead.cmd() != CMsgPingRequest::MsgID */&& unTmpDataLen >= 0) {
			CSocketInfo *tmpSocketInfo = tmpHead.mutable_socketinfos()->Add();
			tmpSocketInfo->Clear();
			tmpSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
			tmpSocketInfo->set_socketid(m_pSocketInfo->m_iSocket);
			tmpSocketInfo->set_state(0);

			pTemp = m_szCSMsgBuf;
			MSG_LEN_TYPE tmpSendLen = sizeof(m_szCSMsgBuf);
//            char* pDataBuff = pTemp1;
			iTmpRet = ClientCommEngine::ConverToGameStream(pTemp,
														   tmpSendLen,
														   pTemp1,
														   unTmpDataLen,
														   &tmpHead);
			if (iTmpRet != 0) {
				ClearSocketInfo(Err_SendToMainSvrd);
			}

			iTmpRet = mC2SPipe->AppendOneCode((const BYTE *) pTemp, tmpSendLen);
			if (iTmpRet < 0) {
				LOG_ERROR("default", "CTCPCtrl::RecvClientData error,send data to gate error,error code = %d", iTmpRet);
				ClearSocketInfo(Err_PacketError);
				return iTmpRet;
			}
#ifdef _DEBUG_
			LOG_DEBUG("defalut", "tcp ==>gate [%d bytes]", tmpSendLen);
#endif
		}
		else {
			//心跳信息不做处理
		}
		m_stTcpStat.m_iPkgSizeRecv++;
	}
	//数据发送完
	if (nRecvAllLen == 0) {
		m_pSocketInfo->m_iRecvBytes = 0;
	}
	else {
		if ((MSG_MAX_LEN < nRecvAllLen) || (nRecvAllLen < 0)) {
			LOG_ERROR("default", "the package length is illeagl,the package len = %d", nRecvAllLen);
			ClearSocketInfo(Err_PacketError);
			return -1;
		}
	}
	return 0;
}

/**
  * 函数名          : CGateCtrl::CheckTimeOut
  * 功能描述        : 检测超时
  * 返回值          ：int
**/
int CGateCtrl::CheckTimeOut()
{
	int i;
	time_t tempTimeGap;
	time(&m_iNowTime); // 计算当前时间

	// 和上次检测时间相比，如果达到了检测间隔则进行检测
	if (m_iNowTime - m_iLastTime < CServerConfig::GetSingleton().GetCheckTimeOutGap()) {
		return 0;
	}
	// 从第1个真正收发数据的socket结构开始到当前最大分配的socket
	// 判断每个socket通讯超时情况
	for (i = m_iSocket + 1; i <= m_iMaxfds; i++) {
		m_pSocketInfo = &m_astSocketInfo[i];
		if (0 == m_pSocketInfo->m_iSocketFlag) {
			continue;
		}
		// 如果Mainsvrd已经向该socket发送过数据，则判断最后一个接收包的时间
		if (MAIN_HAVE_SEND_DATA == m_pSocketInfo->m_iSendFlag) {
			tempTimeGap = m_iNowTime - m_pSocketInfo->m_tStamp;
			// 把当前时间和最近一次socket收到包的时间相比，如果超过了指定的时间间隔则关闭socket
			if (tempTimeGap >= CServerConfig::GetSingleton().GetSocketTimeOut()) {
				// 该socket通讯超时
				LOG_ERROR("default", "Client[%s] socket id = %d port %d not recv packet %d seconds, Close.",
						  m_pSocketInfo->m_szClientIP, m_pSocketInfo->m_iSocket,
						  m_pSocketInfo->m_iConnectedPort, tempTimeGap);

				ClearSocketInfo(Err_ClientTimeout);
			}
		}
		else {
			// 该客户端已经连接上来了，但是Mainsvrd还没有向它发送一个包，这时的超时更短，主要是防止恶意攻击
			tempTimeGap = m_iNowTime - m_pSocketInfo->m_tCreateTime;
			if (CServerConfig::GetSingleton().GetSocketTimeOut() < tempTimeGap) {
				// 该socket通讯超时
				LOG_ERROR("default", "Client[%s] connect port %d Timeout %d seconds, close!",
						  m_pSocketInfo->m_szClientIP, m_pSocketInfo->m_iConnectedPort, tempTimeGap);

				ClearSocketInfo(Err_ClientTimeout);
			}
		}
	}

	// 更新检测时间
	m_iLastTime = m_iNowTime;

	return 0;
}

/**
  * 函数名          : CGateCtrl::TcpRead
  * 功能描述        : 读取tcp数据
  * 返回值          ：int
**/
int CGateCtrl::TcpRead(int iSocket, char *pBuf, int iLen)
{
	int iTmpRecvBytes = 0;
	while (1) {
		iTmpRecvBytes = read(iSocket, pBuf, iLen);
		//读取成功
		if (iTmpRecvBytes > 0) {
			return iTmpRecvBytes;
		}
		else {
			if (iTmpRecvBytes < 0 && errno == EINTR) {
				continue;
			}
			return iTmpRecvBytes;
		}
	}
}
/**
  * 函数名          : CGateCtrl::TcpWrite
  * 功能描述        : 发送tcp数据
  * 返回值          ：int
**/
int CGateCtrl::TcpWrite(int iSocket, char *pBuf, int iPackLen)
{
	int iSendBytes;
//    int iLeftLen = iPackLen;

	while (1) {
		iSendBytes = write(iSocket, pBuf, iPackLen);
		if (iSendBytes == iPackLen) {
			return iSendBytes;
		}
		else {
			if (0 >= iSendBytes && EINTR == errno) {
				continue;
			}
			return iSendBytes;
		}
	}
}

/**
  * 函数名          : CGateCtrl::ClearSocketInfo
  * 功能描述        : 清楚socket
  * 返回值         ：void
**/
void CGateCtrl::ClearSocketInfo(short enError)
{
	if (TCP_SUCCESS != enError) {
		DisConnect(enError);
	}

	//关闭socket
	if (m_pSocketInfo->m_iSocket > 0) {
		m_stEpollEvent.data.fd = m_pSocketInfo->m_iSocket;
		if (epoll_ctl(m_iKdpfd, EPOLL_CTL_DEL, m_pSocketInfo->m_iSocket, &m_stEpollEvent) < 0) {
			LOG_ERROR("default", "epoll remove socket error,socket fd = %d", m_pSocketInfo->m_iSocket);
		}
		//关闭socket
		CloseSocket(m_pSocketInfo->m_iSocket);
		//更改当前对大分配socket
		if (m_pSocketInfo->m_iSocket >= m_iMaxfds) {
			int iTmpUnUseSocket;
			for (iTmpUnUseSocket = m_pSocketInfo->m_iSocket - 1; iTmpUnUseSocket >= m_iSocket; iTmpUnUseSocket--) {
				if (m_astSocketInfo[iTmpUnUseSocket].m_iSocketFlag != 0) {
					break;
				}
				m_iMaxfds = iTmpUnUseSocket;
			}
		}
		//总连接数减一
		m_stTcpStat.m_iConnTotal--;
	}
	m_pSocketInfo->m_iSocket = INVALID_SOCKET;
	m_pSocketInfo->m_iSrcIP = 0;
	m_pSocketInfo->m_nSrcPort = 0;
	m_pSocketInfo->m_iDstIP = 0;
	m_pSocketInfo->m_nDstPort = short(-1);
	m_pSocketInfo->m_iRecvBytes = 0;
	m_pSocketInfo->m_iSocketType = 0;
	m_pSocketInfo->m_iSocketFlag = 0;
	m_pSocketInfo->m_tCreateTime = 0;
	m_pSocketInfo->m_tStamp = 0;
	m_pSocketInfo->m_iSendFlag = 0;
	m_pSocketInfo->m_iConnectedPort = 0;
}

/**
  * 函数名          : CGateCtrl::CheckWaitSendData
  * 功能描述        : 检测是否有缓存数据要下行客户端
  * 返回值          ：int
**/
int CGateCtrl::CheckWaitSendData()
{
	int iTmpRet = 0;
	int i = 0;
	int unTmpCodeLength = 0;

	//每次最多发送MAX_SEND_PKGS_ONCE个数据
	while (i < MAX_SEND_PKGS_ONCE) {
		//
		if (m_iSendIndex < m_S2CHead.socketinfos().size()) {
			//有数据未发送，继续发送
			if (SendClientData()) {
				i++;
				continue;
			}
		}
		else {
			//当前没有接收到数据，先接收数据
			if (false == m_bHasRecv) {
				//没有可发送的数据或者发送完成,则接收gate数据
				iTmpRet = RecvServerData();
				//没有数据可接收，则发送队列无数据发送，退出
				if (iTmpRet == 0) {
					break;
				}
				m_bHasRecv = true;
			}//处理已经接收到的数据
			else {
				//组织服务器发送到客户端的数据信息头，设置相关索引和游标
				m_iSendIndex = 0;
				m_iSCIndex = 0;
				m_nSCLength = 0;
				//反序列化消息的CTcpHead,取出发送游标和长度,把数据存入发送消息缓冲区m_szMsgBuf
				iTmpRet = ClientCommEngine::ConvertStreamToMessage(m_szSCMsgBuf,
																   unTmpCodeLength,
																   &m_S2CHead,
																   NULL,
																   NULL,
																   &m_iSendIndex);
				//序列化失败继续发送
				if (iTmpRet < 0) {
					LOG_ERROR("default",
							  "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return %d.",
							  iTmpRet);
					m_S2CHead.Clear();
					m_iSendIndex = 0;
					continue;
				}

				//接收成功,取出数据长度
				char *pTmp = m_szSCMsgBuf;
				pTmp += m_iSCIndex;
				m_nSCLength = *(unsigned short *) pTmp;
			}
		}
	}
	return 0;
}

/**
  * 函数名          : CGateCtrl::DisConnect
  * 功能描述        : 通知gameserver客户端断开连接
  * 返回值          ：void
**/
void CGateCtrl::DisConnect(int iError)
{

	MesHead tmpHead;
	CSocketInfo *pSocketInfo = tmpHead.mutable_socketinfos()->Add();
	if (pSocketInfo == NULL) {
		LOG_ERROR("default", "CTcpCtrl::DisConnect add_socketinfos ERROR");
		return;
	}
	pSocketInfo->set_socketid(m_pSocketInfo->m_iSocket);
	pSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
	pSocketInfo->set_state(iError);

	unsigned short unTmpMsgLen = (unsigned short) sizeof(m_szCSMsgBuf);

	int iRet = ClientCommEngine::ConvertToGameStream(m_szCSMsgBuf, unTmpMsgLen, &tmpHead);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s: %d : %s] ConvertMsgToStream failed,iRet = %d ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}

	iRet = mC2SPipe->AppendOneCode((BYTE *) m_szCSMsgBuf, unTmpMsgLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[%s: %d : %s] Send data to GateServer failed,iRet = %d ",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}
	return;
}

/**
  * 函数名          : CGateCtrl::RecvServerData
  * 功能描述        : 接收gate返回的消息
  * 返回值          ：int 接收数据长度
**/
int CGateCtrl::RecvServerData()
{
	int unTmpCodeLength = MAX_PACKAGE_LEN;
	if (mS2CPipe->GetHeadCode((BYTE *) m_szSCMsgBuf, &unTmpCodeLength) < 0) {
		unTmpCodeLength = 0;
	}
	return unTmpCodeLength;
}

/**
  * 函数名          : CGateCtrl::SendClientData
  * 功能描述        : 向cliet发送数据
  * 返回值          ：int
**/
int CGateCtrl::SendClientData()
{
	BYTE *pbTmpSend = NULL;
	unsigned short unTmpShort;
	time_t tTmpTimeStamp;
	int iTmpSendBytes;
	int nTmpIndex;
	unsigned short unTmpPackLen;
	int iTmpCloseFlag;

	auto *pSendList = m_S2CHead.mutable_socketinfos();
	//client socket索引非法，不存在要发送的client
	if (m_iSendIndex >= pSendList->size())
		return 0;
	CSocketInfo tmpSocketInfo = pSendList->Get(m_iSendIndex);

	//向后移动socket索引
	m_iSendIndex++;
	nTmpIndex = tmpSocketInfo.socketid();
	tTmpTimeStamp = tmpSocketInfo.createtime();

	//socket 非法
	if (nTmpIndex <= 0 || MAX_SOCKET_NUM <= nTmpIndex) {
		LOG_ERROR("default", "Invalid socket index %d", nTmpIndex);
		return -1;
	}

	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送
	*/
	if (m_astSocketInfo[nTmpIndex].m_tCreateTime != tTmpTimeStamp) {
		LOG_ERROR("default", "sokcet[%d] already closed(tcp createtime:%d:gate createtime:%d) : gate ==> client[%d] bytes \
                failed", nTmpIndex, m_astSocketInfo[nTmpIndex].m_tCreateTime, tTmpTimeStamp, m_nSCLength);
		return -1;
	}
	iTmpCloseFlag = tmpSocketInfo.state();
	unTmpPackLen = m_nSCLength;
	m_pSocketInfo = &m_astSocketInfo[nTmpIndex];
	//发送数据
	if (unTmpPackLen > 0) {
		//根据发送给客户端的数据在m_szSCMsgBuf中的数组下标取出数据
		pbTmpSend = (BYTE *) m_szSCMsgBuf[m_iSCIndex];
		memcpy((void *) &unTmpShort, (const void *) pbTmpSend, sizeof(unsigned short));
		if (unTmpShort != unTmpPackLen) {
			LOG_ERROR("default",
					  "Code length not matched,left length %u is less than body length %u",
					  unTmpPackLen,
					  unTmpShort);
			return -1;
		}
		iTmpSendBytes = TcpWrite(m_pSocketInfo->m_iSocket, (char *) pbTmpSend, unTmpPackLen);
		if (unTmpPackLen == iTmpSendBytes) {
#ifdef _DEBUG_
			LOG_DEBUG("default", "[us:%lu]TCP gate ==> client[%d][%s][%d Bytes]", GetUSTime(), nTmpIndex, \
                    m_astSocketInfo[nTmpIndex].m_szClientIP, iTmpSendBytes);
#endif
			//统计信息
			m_stTcpStat.m_iPkgSend++;
			m_stTcpStat.m_iPkgSizeSend += iTmpSendBytes;
			//设置发送标志位已发送
			m_pSocketInfo->m_iSendFlag = MAIN_HAVE_SEND_DATA;
		}
		else {
			//发送失败
			ClearSocketInfo(Err_ClientClose);
			LOG_ERROR("default", "send to client %s Failed due to error %d", m_pSocketInfo->m_szClientIP, errno);
			return 0;
		}
	}
	if (unTmpPackLen > 0 && iTmpCloseFlag == 0) {
		return 0;
	}

	if (iTmpCloseFlag == -1) {
		//
	}
	ClearSocketInfo(TCP_SUCCESS);
	return 0;
}

static void CGateCtrl::OnAcceptCns(uint32 uId, CAcceptor *pAcceptorEx)
{
	std::cout << "OnAcceptCns : " << uId << std::endl;
}

static void CGateCtrl::OnCnsDisconnected(CAcceptor *pAcceptorEx)
{
}

static void CGateCtrl::OnCnsSomeDataSend(CAcceptor *pAcceptorEx)
{
}

static void CGateCtrl::OnCnsSomeDataRecv(CAcceptor *pAcceptorEx)
{
	unsigned int iTmpLen = 0;
//	pAcceptorEx->RecvData(iTmpLen);
}