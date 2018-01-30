//
// Created by dguco on 18-1-30.
//

#include <sharemem.h>
#include <my_assert.h>
#include <commondef.h>
#include "../inc/s2c_handle.h"

CS2cHandle::CS2cHandle()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);
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
	m_pS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);
}

CS2cHandle::~CS2cHandle()
{

}

int CS2cHandle::PrepareToRun()
{
	return 0;
}

int CS2cHandle::Run()
{
	while (true) {
		CondBlock();

	}
}

bool CS2cHandle::IsToBeBlocked()
{
	return m_pS2CPipe->IsQueueEmpty();
}

bool CS2cHandle::CheckData()
{
	if (!m_pS2CPipe->IsQueueEmpty()) {
		WakeUp();
	}
}

int CS2cHandle::RecvServerData()
{
	int unTmpCodeLength = MAX_PACKAGE_LEN;
	if (m_pS2CPipe->GetHeadCode((BYTE *) m_szSCMsgBuf, &unTmpCodeLength) < 0) {
		unTmpCodeLength = 0;
	}
	return unTmpCodeLength;
}
