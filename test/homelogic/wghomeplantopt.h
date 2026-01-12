
#ifndef WGHOMEPLANTOPT_H
#define WGHOMEPLANTOPT_H

#include "DB_Struct.h"
#include "FB_BaseType.h"
#include "gwhomeplantopt.h"

#define PACKET_WG_HOME_PLANT_OPT 0x00000001

class WGHomePlantOpt : public Packet
{
public:
    WGHomePlantOpt();
    virtual ~WGHomePlantOpt();
    BOOL Read(SocketInputStream& is);
    BOOL Write(SocketOutputStream& os) const;
    UINT Execute(Player* pPlayer);
    PacketID_t GetPacketID() const { return PACKET_WG_HOME_PLANT_OPT; }
    UINT GetPacketSize() const { return sizeof(WGHomePlantOpt); }
    GUID64_t GetGUID() const { return m_GUID; }
    BYTE GetOpt() const { return m_bOpt; }
    HomePlant* GetHomePlant() { return &m_HomePlant; }
    VOID SetHomePlant(HomePlant& homePlant) { m_HomePlant = homePlant; }
    VOID SetGUID(GUID64_t guid) { m_GUID = guid; }
    VOID SetOpt(BYTE opt) { m_bOpt = opt; }
    VOID SetPlayerID(PlayerID_t playerID) { m_nPlayerID = playerID; }
    PlayerID_t GetPlayerID() const { return m_nPlayerID; }
private:
    GUID64_t    m_GUID;
    PlayerID_t  m_nPlayerID;
    BYTE        m_bOpt;
    HomePlant   m_HomePlant;
};


class WGHomePlantOptFactory : public PacketFactory
{
public:
    WGHomePlantOptFactory()
    {
    }
    ~WGHomePlantOptFactory()
    {
    }
    Packet* CreatePacket() 
    {
        return new WGHomePlantOpt();
    }
    PacketID_t GetPacketID() const 
    {
        return PACKET_WG_HOME_PLANT_OPT;
    }
    UINT GetPacketMaxSize() const 
    {
        return sizeof(WGHomePlantOpt);
    }
};

class WGHomePlantOptHandler
{
public:
    static UINT Execute(WGHomePlantOpt* pPacket,Player* pPlayer);
};

#endif
