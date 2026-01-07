#ifndef __HOME_PLANT_MANAGER_H__
#define __HOME_PLANT_MANAGER_H__

#include <map>
#include "test_common.h"

using namespace my_std;
#define MAX_TRANK_GRIDE 4 // 每个地块4个种植格子
#define MAX_TRANK_GRIDE_PLANT 4 // 每个地块4个种植格子，每个格子可以种1个植物
#define MAX_TRANK_PLANT (MAX_TRANK_GRIDE * MAX_TRANK_GRIDE_PLANT) // 每个地块最多可以种16个植物
#define MAX_HOME_TRANK 8  // 每个家园最多可以有8个地块儿
#define MAX_PLANT_ITEM 100 //最多可以有100个物品
#define MAX_PLANT_GUIDE 16 //最多可以有16个植物图鉴
#define MAX_HOME_PLANT_COUNT 1024 //最大家园数量

// 植物状态
enum PlantState
{
    PlantState_UnPlant = 0, // 未种
    PlantState_ChengZhang = 1, //成长期
    PlantState_ChengShu = 2, //成熟期
    PlantState_JianChan = 3, //减产期
};

// 植物信息
struct PlantInfo
{
    SHORT   m_nPlantID; // 植物 id
    BYTE    m_bPlantState; // 植物状态
    UINT    m_nPlantTime; // 种植时间
    UINT    m_nSpeedTime; // 加速时间
    USHORT  m_nWateringTimes; //被浇水次数

    PlantInfo()
    {
        m_nPlantID = 0;
        m_bPlantState = PlantState_UnPlant;
        m_nPlantTime = 0;
        m_nSpeedTime = 0;
        m_nWateringTimes = 0;
    }

    bool Read(SocketInputStream& inputStream)
    {
        inputStream.Read(m_nPlantID, sizeof(m_nPlantID));
        inputStream.Read(m_bPlantState, sizeof(m_bPlantState));
        inputStream.Read(m_nPlantTime, sizeof(m_nPlantTime));
        inputStream.Read(m_nSpeedTime, sizeof(m_nSpeedTime));
        inputStream.Read(m_nWateringTimes, sizeof(m_nWateringTimes));
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        outputStream.Write(m_nPlantID, sizeof(m_nPlantID));
        outputStream.Write(m_bPlantState, sizeof(m_bPlantState));
        outputStream.Write(m_nPlantTime, sizeof(m_nPlantTime));
        outputStream.Write(m_nSpeedTime, sizeof(m_nSpeedTime));
        outputStream.Write(m_nWateringTimes, sizeof(m_nWateringTimes));
        return true;
    }
};

// 地块儿信息
struct PlantTrunk
{
    SHORT   m_nTrunkID; //地块儿 id
    BYTE    m_bState; // 地块儿状态  0 未解锁 1 已解锁
    SHORT   m_nLevel; // 地块儿等级
    TArray<PlantInfo, MAX_TRANK_PLANT> m_PlantArray; // 地块儿上的植物数组

    PlantTrunk()
    {
        m_nTrunkID = 0;
        m_bState = 0;
        m_nLevel = 0;
    }

    bool Read(SocketInputStream& inputStream)
    {
        inputStream.Read(m_nTrunkID, sizeof(m_nTrunkID));
        inputStream.Read(m_bState, sizeof(m_bState));
        inputStream.Read(m_nLevel, sizeof(m_nLevel));
        for(int i = 0; i < MAX_TRANK_PLANT; ++i)
        {
            m_PlantArray[i].Read(inputStream);
        }
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        outputStream.Write(m_nTrunkID, sizeof(m_nTrunkID));
        outputStream.Write(m_bState, sizeof(m_bState));
        outputStream.Write(m_nLevel, sizeof(m_nLevel));
        for(int i = 0; i < MAX_TRANK_PLANT; ++i)
        {
            m_PlantArray[i].Write(outputStream);
        }
        return true;
    }
};

// 物品信息
struct PlantItemInfo
{
    INT   m_nItemId;    // 物品id
    INT   m_nItemCount; // 物品数量
    
    PlantItemInfo()
    {
        m_nItemId = 0;
        m_nItemCount = 0;
    }

    bool Read(SocketInputStream& inputStream)
    {
        inputStream.Read(m_nItemId, sizeof(m_nItemId));
        inputStream.Read(m_nItemCount, sizeof(m_nItemCount));
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        outputStream.Write(m_nItemId, sizeof(m_nItemId));
        outputStream.Write(m_nItemCount, sizeof(m_nItemCount));
        return true;
    }
};

//植物图鉴
struct PlantGuideInfo
{
    SHORT   m_nPlantID; // 植物 id
    BYTE    m_bLock; // 是否解锁 0 未解锁 1 已解锁
    USHORT  m_nLevel; //作物等级

    PlantGuideInfo()
    {
        m_nPlantID = 0;
        m_bLock = 0;
        m_nLevel = 0;
    }

    bool Read(SocketInputStream& inputStream)
    {
        inputStream.Read(m_nPlantID, sizeof(m_nPlantID));
        inputStream.Read(m_bLock, sizeof(m_bLock));
        inputStream.Read(m_nLevel, sizeof(m_nLevel));
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        outputStream.Write(m_nPlantID, sizeof(m_nPlantID));
        outputStream.Write(m_bLock, sizeof(m_bLock));
        outputStream.Write(m_nLevel, sizeof(m_nLevel));
        return true;
    }
};

struct HomePlant
{   
    GUID64_t m_OwnerID; // 家园主人
    TArray<PlantTrunk, MAX_HOME_TRANK> m_TrunkArray; // 家园上的地块儿数组
    TArray<PlantItemInfo, MAX_PLANT_ITEM> m_ItemArray; // 家园上的物品数组
    TArray<PlantGuideInfo, MAX_PLANT_GUIDE> m_GuideArray; // 家园上的植物图鉴
    USHORT  m_nWateringTimes; // 浇水次数

    HomePlant()
    {
        m_nWateringTimes = 0;
    }

    bool Read(SocketInputStream& inputStream)
    {
        m_OwnerID.Read(inputStream);
        for(int i = 0; i < MAX_HOME_TRANK; ++i)
        {
            m_TrunkArray[i].Read(inputStream);
        }
        
        USHORT nItemCount = 0;
        inputStream.Read(nItemCount, sizeof(nItemCount));
        for(int i = 0; i < MAX_PLANT_ITEM && i < nItemCount; ++i)
        {
            m_ItemArray[i].Read(inputStream);
        }

        for(int i = 0; i < MAX_PLANT_GUIDE; ++i)
        {
            m_GuideArray[i].Read(inputStream);
        }
        inputStream.Read(m_nWateringTimes, sizeof(m_nWateringTimes));
        return true;
    }

    bool Write(SocketOutputStream& outputStream)
    {
        m_OwnerID.Write(outputStream);
        for(int i = 0; i < MAX_HOME_TRANK; ++i)
        {
            m_TrunkArray[i].Write(outputStream);
        }
        USHORT nItemCount = 0;
        for(int i = 0; i < MAX_PLANT_ITEM; ++i)
        {
            if(m_ItemArray[i].m_nItemId > 0 && m_ItemArray[i].m_nItemCount > 0)
            {
                nItemCount++;
            }
        }
        outputStream.Write(nItemCount, sizeof(nItemCount));
        for(int i = 0; i < MAX_PLANT_ITEM; ++i)
        {
            if(m_ItemArray[i].m_nItemId > 0 && m_ItemArray[i].m_nItemCount > 0)
            {
                m_ItemArray[i].Write(outputStream);
            }
        }
        for(int i = 0; i < MAX_PLANT_GUIDE; ++i)
        {
            m_GuideArray[i].Write(outputStream);
        }
        outputStream.Write(m_nWateringTimes, sizeof(m_nWateringTimes));
        return true;
    }
};

//家园
struct Home
{
    HomePlant m_HomePlant; // 家园上的植物
};

class HomePlantManager
{
public:
    HomePlantManager()
    {

    }

    SafePointer<HomePlant> GetHomePlant(GUID64_t ownerID)
    {
        std::map<GUID64_t, HomePlant*>::iterator it = m_HomeMap.find(ownerID);
        if(it == m_HomeMap.end())
        {
            return NULL;
        }
        return it->second;
    }
private:
    TArray<Home, MAX_HOME_PLANT_COUNT> m_HomeArray; // 家园上的植物数组
    std::map<GUID64_t, HomePlant*> m_HomeMap; // 家园上的植物映射表
};

#endif