#ifndef __CGASKHOMEPLANT_H__
#define __CGASKHOMEPLANT_H_

#include "FB_BaseType.h"

#define PACKET_CG_ASK_HOME_PLANT 0x00010001

enum HomePlantOpt
{
    HOME_PLANT_OPT_ASK = 0,   // 询问植物信息
    HOME_PLANT_OPT_UNLOCK_TRUNK = 1, // 解锁 trunk
    HOME_PLANT_OPT_WATER = 2, // 浇水
    HOME_PLANT_OPT_FERTILIZE = 3, // 施肥
};

class CGAskHomePlant : public Packet
{
public:
    CGAskHomePlant();
    virtual ~CGAskHomePlant();
    // 读取数据
    BOOL Read(SocketInputStream& inputStream);
    BOOL Write(SocketOutputStream& outputStream) const;
    UINT Execute(Player* pPlayer);
    // 执行
    PacketID_t GetPacketID() const   {  return PACKET_CG_ASK_HOME_PLANT; }
    UINT GetPacketSize() const  {   return sizeof(CGAskHomePlant); }
    BYTE GetOpt() const { return m_bOpt; }
    INT GetTrunkID() const { return m_nTrunkID; }
    INT GetSlotID() const { return m_nSlotID; }
private:
    BYTE m_bOpt;
    INT  m_nTrunkID;
    INT  m_nSlotID;
};

class CGAskHomePlantFactory : public PacketFactory
{
public:
    CGAskHomePlantFactory()
    {
    }
    ~CGAskHomePlantFactory()
    {
    }
    Packet* CreatePacket() 
    {
        return new CGAskHomePlant();
    }
    PacketID_t GetPacketID() const 
    {
        return PACKET_CG_ASK_HOME_PLANT;
    }
    UINT GetPacketMaxSize() const 
    {
        return sizeof(CGAskHomePlant);
    }
};


class CGAskHomePlantHandler
{
public:
    static UINT Execute(CGAskHomePlant* pPacket,Player* pPlayer);
};
#endif
