#ifndef GW_HOMEPLANTOPT_H
#define GW_HOMEPLANTOPT_H

#include "DB_Struct.h"
#include "FB_BaseType.h"

#define PACKET_GW_HOME_PLANT_OPT    0x00000002

enum enGWGHomePlantOpt
{
    GWG_HOME_PLANT_OPT_ASK = 0,   // 询问植物信息
    GWG_HOME_PLANT_OPT_UNLOCK_TRUNK = 1, // 解锁 trunk
    GWG_HOME_PLANT_OPT_WATER = 2, // 浇水
    GWG_HOME_PLANT_OPT_FERTILIZE = 3, // 施肥
    GWG_HOME_PLANT_OPT_RET_PLANT_INFO = 4, // 返回植物信息
};

class GWHomePlantOpt : public Packet
{
public:
    GWHomePlantOpt();
    virtual ~GWHomePlantOpt();
    BOOL Read(SocketInputStream& is);
    BOOL Write(SocketOutputStream& os) const;
    UINT Execute(Player* pPlayer);
    PacketID_t GetPacketID() const { return PACKET_GW_HOME_PLANT_OPT; }
    UINT GetPacketSize() const { return sizeof(GWHomePlantOpt); }
    GUID64_t GetGUID() const { return m_GUID; }
    BYTE GetOpt() const { return m_bOpt; }
    VOID SetGUID(GUID64_t guid) { m_GUID = guid; }
    VOID SetOpt(BYTE opt) { m_bOpt = opt; }
private:
    GUID64_t m_GUID;
    BYTE m_bOpt;
};

class GWHomePlantOptFactory : public PacketFactory
{
public:
    GWHomePlantOptFactory()
    {
    }
    ~GWHomePlantOptFactory()
    {
    }
    Packet* CreatePacket() 
    {
        return new GWHomePlantOpt();
    }
    PacketID_t GetPacketID() const 
    {
        return PACKET_GW_HOME_PLANT_OPT;
    }
    UINT GetPacketMaxSize() const 
    {
        return sizeof(GWHomePlantOpt);
    }
};


class GWHomePlantOptHandler
{
public:
    static UINT Execute(GWHomePlantOpt* pPacket,Player* pPlayer);
};

#endif // GWGHOMEPLANTOPT_H