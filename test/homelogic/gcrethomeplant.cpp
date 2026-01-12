#include "gcrethomeplant.h"

GCRetHomePlant::GCRetHomePlant()
{
    m_bOpt = 0;
}

GCRetHomePlant::~GCRetHomePlant()
{
    
}

BOOL GCRetHomePlant::Read(SocketInputStream& is)
{
    __ENTER_FUNCTION
    is.Read(m_bOpt, sizeof(m_bOpt));
    m_HomePlant.Read(is);
    return true;
    __LEAVE_FUNCTION
    return false;
}

BOOL GCRetHomePlant::Write(SocketOutputStream& os) const
{
    __ENTER_FUNCTION
    os.Write(m_bOpt, sizeof(m_bOpt));
    m_HomePlant.Write(os);
    return true;
    __LEAVE_FUNCTION
    return false;
}

UINT GCRetHomePlant::Execute(Player* pPlayer)
{
    __ENTER_FUNCTION
    return GCRetHomePlantHandler::Execute(this, pPlayer);
    __LEAVE_FUNCTION
    return 0;
}