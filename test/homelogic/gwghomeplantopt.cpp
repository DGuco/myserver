#include "gwghomeplantopt.h"

GWGHomePlantOpt::GWGHomePlantOpt()
{
    m_bOpt = 0;
}

GWGHomePlantOpt::~GWGHomePlantOpt()
{

}

BOOL GWGHomePlantOpt::Read(SocketInputStream& is)
{
    __ENTER_FUNCTION
    m_GUID.Read(is);
    is.Read(m_bOpt, sizeof(BYTE));
    m_HomePlant.Read(is);
    return TRUE;
    __LEAVE_FUNCTION
}

BOOL GWGHomePlantOpt::Write(SocketOutputStream& os) const
{
    __ENTER_FUNCTION
    m_GUID.Write(os);
    os.Write(m_bOpt, sizeof(BYTE));
    m_HomePlant.Write(os);
    return TRUE;
    __LEAVE_FUNCTION
}

UINT GWGHomePlantOpt::Execute(Player* pPlayer)
{
    __ENTER_FUNCTION
    return GWGHomePlantOptHandler::Execute(this, pPlayer);
    __LEAVE_FUNCTION
    return 0;
}


