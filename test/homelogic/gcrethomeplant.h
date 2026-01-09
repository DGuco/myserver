#include "FB_BaseType.h"
#include "DB_Struct.h"

#define PACKET_GC_RET_HOME_PLANT    0x00000001

class GCRetHomePlant : public Packet
{
public:
    GCRetHomePlant();
    virtual ~GCRetHomePlant();
    BOOL Read(SocketInputStream& is);
    BOOL Write(SocketOutputStream& os) const;
    UINT Execute(Player* pPlayer);
    PacketID_t GetPacketID() const { return PACKET_GC_RET_HOME_PLANT; }
    UINT GetPacketSize() const { return sizeof(GCRetHomePlant); }
    VOID SetOpt(BYTE bOpt) { m_bOpt = bOpt; }
    HomePlant* GetHomePlant() { return &m_HomePlant; }
private:
    BYTE m_bOpt;
    HomePlant m_HomePlant;
};

class CRetHomePlantFactory : public PacketFactory
{
public:
    CRetHomePlantFactory()
    {
    }
    ~CRetHomePlantFactory()
    {
    }
    Packet* CreatePacket() 
    {
        return new GCRetHomePlant();
    }
    PacketID_t GetPacketID() const 
    {
        return PACKET_GC_RET_HOME_PLANT;
    }
    UINT GetPacketMaxSize() const 
    {
        return sizeof(GCRetHomePlant);
    }
};

class CRetHomePlantHandler
{
public:
    static UINT Execute(GCRetHomePlant* pPacket,Player* pPlayer);
};


UINT CRetHomePlantHandler::Execute(GCRetHomePlant* pPacket, Player* pPlayer)
{
    __ENTER_FUNCTION
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
}