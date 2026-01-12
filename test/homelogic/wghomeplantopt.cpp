#include "wghomeplantopt.h"

WGHomePlantOpt::WGHomePlantOpt()
{
    m_bOpt = 0;
}

WGHomePlantOpt::~WGHomePlantOpt()
{
}

BOOL WGHomePlantOpt::Read(SocketInputStream& is)
{
    __ENTER_FUNCTION
    m_GUID.Read(is);
    is.Read(m_nPlayerID, sizeof(PlayerID_t));
    is.Read(m_bOpt, sizeof(BYTE));
    if(m_bOpt == GWG_HOME_PLANT_OPT_RET_PLANT_INFO)
    {
        m_HomePlant.Read(is);
    }
    return TRUE;
    __LEAVE_FUNCTION
    return FALSE;
}

BOOL WGHomePlantOpt::Write(SocketOutputStream& os) const
{
    __ENTER_FUNCTION
    m_GUID.Write(os);
    os.Write(m_nPlayerID, sizeof(PlayerID_t));
    os.Write(m_bOpt, sizeof(BYTE));
    if(m_bOpt == GWG_HOME_PLANT_OPT_RET_PLANT_INFO)
    {
        m_HomePlant.Write(os);
    }
    return TRUE;
    __LEAVE_FUNCTION
    return FALSE;
}

UINT WGHomePlantOpt::Execute(Player* pPlayer)
{
    __ENTER_FUNCTION
    return WGHomePlantOptHandler::Execute(this, pPlayer);
    __LEAVE_FUNCTION
    return 0;
}
