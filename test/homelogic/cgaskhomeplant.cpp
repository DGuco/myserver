#include "CGAskHomePlant.h"

CGAskHomePlant::CGAskHomePlant()
{
    m_bOpt = 0;
    m_nTrunkID = 0;
    m_nSlotID = 0;
}

CGAskHomePlant::~CGAskHomePlant()
{
}

BOOL CGAskHomePlant::Read(SocketInputStream& inputStream)
{
    __ENTER_FUNCTION
    inputStream.Read(m_bOpt, sizeof(BYTE));
    inputStream.Read(m_nTrunkID, sizeof(INT));
    inputStream.Read(m_nSlotID, sizeof(INT));
    return true;
    __LEAVE_FUNCTION
    return false;
}

BOOL CGAskHomePlant::Write(SocketOutputStream& outputStream) const
{
    __ENTER_FUNCTION
    outputStream.Write(m_bOpt, sizeof(BYTE));
    outputStream.Write(m_nTrunkID, sizeof(INT));
    outputStream.Write(m_nSlotID, sizeof(INT));
    return true;
    __LEAVE_FUNCTION
    return false;
}

UINT CGAskHomePlant::Execute(Player* pPlayer)
{
    __ENTER_FUNCTION
    return CGAskHomePlantHandler::Execute(this, pPlayer);
    __LEAVE_FUNCTION
    return 0;
}