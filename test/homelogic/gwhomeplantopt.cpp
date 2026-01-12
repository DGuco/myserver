#include "gwhomeplantopt.h"

GWHomePlantOpt::GWHomePlantOpt()
{
    m_bOpt = 0;
}

GWHomePlantOpt::~GWHomePlantOpt()
{

}

BOOL GWHomePlantOpt::Read(SocketInputStream& is)
{
    __ENTER_FUNCTION
    m_GUID.Read(is);
    is.Read(m_bOpt, sizeof(BYTE));
    return TRUE;
    __LEAVE_FUNCTION
    return FALSE;
}

BOOL GWHomePlantOpt::Write(SocketOutputStream& os) const
{
    __ENTER_FUNCTION
    m_GUID.Write(os);
    os.Write(m_bOpt, sizeof(BYTE));
    return TRUE;
    __LEAVE_FUNCTION
    return FALSE;
}

UINT GWHomePlantOpt::Execute(Player* pPlayer)
{
    __ENTER_FUNCTION
    return GWHomePlantOptHandler::Execute(this, pPlayer);
    __LEAVE_FUNCTION
    return 0;
}


