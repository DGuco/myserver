//
// Created by dguco on 17-6-21.
//

#include "../inc/clienthandle.h"
#include "../../framework/sharemem/shm.h"
#include "../../framework/sharemem/sharemem.h"
#include "../../framework/sharemem/codequeue.h"
#include "../../framework/base/my_assert.h"
#include "../../framework/base/commondef.h"
#include "../../framework/net/client_comm_engine.h"

CClientHandle::CClientHandle()
{
}

CClientHandle::~CClientHandle()
{
}

int CClientHandle::Initialize()
{
    int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

    ////////////////////////////////mS2CPipe/////////////////////////////////////////
    system("touch ./scpipefile");
    char* pcTmpSCPipeID = getenv("SC_PIPE_ID");
    int iTmpSCPipeID = 0;
    if (pcTmpSCPipeID)
    {
        iTmpSCPipeID = atoi(pcTmpSCPipeID);
    }
    key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
    BYTE* pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
    MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize);
    //mS2CPipe地址相对pbyTmpS2CPipe的偏移地址是sizeof(CSharedMem) + sizeof(CCodeQueue)
    mS2CPipe = CCodeQueue::CreateInsance(PIPE_SIZE, EnLockIdx::IDX_PIPELOCK_S2C);

    ////////////////////////////////mC2SPipe/////////////////////////////////////////
    system("touch ./cspipefile");
    char* pcTmpCSPipeID = getenv("CS_PIPE_ID");
    int iTmpCSPipeID = 0;
    if (pcTmpCSPipeID)
    {
        iTmpCSPipeID = atoi(pcTmpCSPipeID);
    }
    key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
    BYTE* pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
    MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize);
    //mC2SPipe地址相对pbyTmpS2CPipe的偏移地址是sizeof(CSharedMem) + sizeof(CCodeQueue)
    mC2SPipe = CCodeQueue::CreateInsance(PIPE_SIZE, EnLockIdx::IDX_PIPELOCK_C2S);
    return 0;
}

int CClientHandle::Send2Tcp(CMessageSet *pMsgSet, long lMsgGuid)
{
    MY_ASSERT(pMsgSet != NULL, return -1);
    BYTE aTmpMsgBuf[MAX_PACKAGE_LEN] = {0};
    unsigned short iTmpLen = sizeof(aTmpMsgBuf);
//    CCSHead tmpCsHead;
//    int iRet = ClientCommEngine::ConvertMsgToStream(&mNetHead,
//             &tmpCsHead,
//             pMsgSet,
//             aTmpMsgBuf,
//             iTmpLen,
//             false);
//    mS2CPipe->AppendOneCode()
}

int CClientHandle::Send(CMessageSet *pMsgSet, stPointList *pTeamList)
{

}

int CClientHandle::Send(CMessageSet *pMsgSet, long lMsgGuid, int iSocket, time_t tCreateTime, unsigned int uiIP,
                        unsigned short unPort, bool bKickOff)
{

}