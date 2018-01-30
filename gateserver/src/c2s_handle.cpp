//
// Created by dguco on 18-1-30.
//

#include <config.h>
#include <sharemem.h>
#include "../inc/c2s_handle.h"

CC2sHandle::CC2sHandle()
	: m_pNetWork(new CNetWork(eNetModule::NET_EPOLL))
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);
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
	m_pC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);
}

CC2sHandle::~CC2sHandle()
{

}

bool CC2sHandle::BeginListen()
{
	ServerInfo *gateInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GATESERVER);
	return m_pNetWork->BeginListen(gateInfo->m_sHost.c_str(),
								   (short) gateInfo->m_iPort,
								   &OnAcceptCns,
								   &OnCnsDisconnected,
								   &OnCnsSomeDataSend,
								   &OnCnsSomeDataRecv);
}

static void CC2sHandle::OnAcceptCns(uint32 uId, CAcceptor *pAcceptorEx)
{
	std::cout << "OnAcceptCns : " << uId << std::endl;
}

static void CC2sHandle::OnCnsDisconnected(CAcceptor *pAcceptorEx)
{

}

static void CC2sHandle::OnCnsSomeDataSend(CAcceptor *pAcceptorEx)
{
}

static void CC2sHandle::OnCnsSomeDataRecv(CAcceptor *pAcceptorEx)
{
	unsigned int iTmpLen = 0;
//	pAcceptorEx->RecvData(iTmpLen);
}

int CC2sHandle::PrepareToRun()
{
	bool iRet = BeginListen();
	if (!iRet) {
		LOG_ERROR("default", "Listen failed...");
	}
	return 0;
}

int CC2sHandle::Run()
{
	//libevent事件循环
	m_pNetWork->DispatchEvents();
	return 0;
}

bool CC2sHandle::IsToBeBlocked()
{
	return false;
}
