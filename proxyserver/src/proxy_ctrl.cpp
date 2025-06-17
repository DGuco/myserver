//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#include "my_assert.h"
#include "proxy_ctrl.h"
#include "proxy_server.h"
#include "mfactory_manager.h"
#include "time_helper.h"

CProxyCtrl::CProxyCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
	m_pTcpManagerScheduler = new CThreadScheduler("TcpManagerScheduler");
	m_pTransferScheduler = new CThreadScheduler("TransferScheduler");
	for (int i = 0; i < MAX_TRANSFER_THREAD; i++)
	{
		m_TransferThread[i] = new CTransferThreadInfo();
		m_TransferThread[i]->m_nThreadIndex = i;
        m_TransferThread[i]->m_pTransfer = new CProxyTransfer();
	}
}

CProxyCtrl::~CProxyCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
	WSACleanup();
#endif
}

bool CProxyCtrl::PrepareToRun()
{
	int a = 0;
	//读取配置文件
	if (!ReadConfig())
	{
		return false;
	}

	//消息工厂注册
	CMessageFactoryManager::GetSingletonPtr()->Init();
	return true;
}

int CProxyCtrl::Run()
{
	if (!m_pTcpManagerScheduler->Init(1, &CProxyCtrl::ProxyServerInit,&CProxyCtrl::ProxyServerLogic,NULL,NULL))
	{
		return false;
	}

	if (!m_pTransferScheduler->Init(MAX_TRANSFER_THREAD, 
		&CProxyCtrl::TransferThreadInit, 
		&CProxyCtrl::TransferThreadLogic, 
		(void**)&m_TransferThread,
		(void**)&m_TransferThread))
	{
		return false;
	}

	while (true)
    {
        SLEEP(10);
    }
}

void CProxyCtrl::SelectTransferThread(CSocket socket)
{
	if(socket.IsValid() == false)
	{
		return;
	}
	int nIndex = socket.GetSocket() % MAX_TRANSFER_THREAD;
	m_TransferThread[nIndex]->m_pTransfer->AddNewIncomingConn(socket);
	CACHE_LOG(TCP_DEBUG,"CProxyCtrl::SelectTransferThread new socket nIndex = {},socket = {},host = {},port = {}",
		nIndex,socket.GetSocket(),socket.GetHost().c_str(),socket.GetPort());
}

bool CProxyCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) 
	{
		DISK_LOG(ERROR_DISK, "Get ServerConfig failed");
		return false;
	}
	return true;
}

void CProxyCtrl::ProxyServerLogic(void* args)
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	try
	{
		CProxyServer::GetSingletonPtr()->TcpTick(nNow);
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE,"CProxyServer TcpTick  cache execption msg {]", e.what());
	}
}

void CProxyCtrl::ProxyServerInit(void* args)
{
    if (!CProxyServer::GetSingletonPtr()->InitTcp())
	{
		DISK_LOG(ERROR_DISK, "CDBCtrl::GetSingletonPtr()->InitTcp failed");
		exit(0);
	}
}


void CProxyCtrl::TransferThreadLogic(void* args)
{
	if(args == NULL)
    {
		return;
    }

    CTransferThreadInfo* pInfo = (CTransferThreadInfo*)args;
    if(pInfo->m_pTransfer == NULL)
    {
        return;
    }

	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	try
	{
		pInfo->m_pTransfer->TcpTick(nNow);
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE,"CProxyCtrl::TransferThreadLogic TcpTick cache execption msg {]", e.what());
	}
    return;
}

void CProxyCtrl::TransferThreadInit(void* args)
{
	if(args == NULL)
    {
		return;
    }
	
    CTransferThreadInfo* pInfo = (CTransferThreadInfo*)args;
    if(pInfo->m_pTransfer == NULL)
    {
        return;
    }

	int nRet = pInfo->m_pTransfer->InitTcpServer(eTcpEpoll);
	if (nRet != 0)
	{
		DISK_LOG(ERROR_DISK, "CProxyCtrl::TransferThreadLogic() InitTcp failed,nRet = {}",nRet);
		exit(0);
	}else
	{
		DISK_LOG(DEBUG_DISK, "CProxyCtrl::TransferThreadLogic() InitTcp ok,threadindex = {}",pInfo->m_nThreadIndex);
	}
    return;
}