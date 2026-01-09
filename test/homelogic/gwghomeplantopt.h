#ifndef GWGHOMEPLANTOPT_H
#define GWGHOMEPLANTOPT_H

#include "DB_Struct.h"
#include "FB_BaseType.h"

#define PACKET_GW_HOME_PLANT_OPT    0x00000002

class GWGHomePlantOpt : public Packet
{
public:
    GWGHomePlantOpt();
    virtual ~GWGHomePlantOpt();
    BOOL Read(SocketInputStream& is);
    BOOL Write(SocketOutputStream& os) const;
    UINT Execute(Player* pPlayer);
    PacketID_t GetPacketID() const { return PACKET_GW_HOME_PLANT_OPT; }
    UINT GetPacketSize() const { return sizeof(GWGHomePlantOpt); }
    GUID64_t GetGUID() const { return m_GUID; }
    BYTE GetOpt() const { return m_bOpt; }
    HomePlant* GetHomePlant() { return &m_HomePlant; }
    VOID SetGUID(GUID64_t guid) { m_GUID = guid; }
    VOID SetOpt(BYTE opt) { m_bOpt = opt; }
private:
    GUID64_t m_GUID;
    BYTE m_bOpt;
    HomePlant m_HomePlant;
};

class GWGHomePlantOptFactory : public PacketFactory
{
public:
    GWGHomePlantOptFactory()
    {
    }
    ~GWGHomePlantOptFactory()
    {
    }
    Packet* CreatePacket() 
    {
        return new GWGHomePlantOpt();
    }
    PacketID_t GetPacketID() const 
    {
        return PACKET_GW_HOME_PLANT_OPT;
    }
    UINT GetPacketMaxSize() const 
    {
        return sizeof(GWGHomePlantOpt);
    }
};


class GWGHomePlantOptHandler
{
public:
    static UINT Execute(GWGHomePlantOpt* pPacket,Player* pPlayer);
};

#endif // GWGHOMEPLANTOPT_H