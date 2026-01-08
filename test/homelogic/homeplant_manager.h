#ifndef __HOME_PLANT_MANAGER_H__
#define __HOME_PLANT_MANAGER_H__

#include <map>
#include "DB_Struct.h"
#include "FB_BaseType.h"

class HomePlantManager
{
public:
    HomePlantManager()
    {

    }

    // 获取家园
    SafePointer<Home> GetHome(GUID64_t ownerID);
    //解锁地块
    VOID UnlockTrunk(Obj_Human* pOwner,SHORT nTrunkID);
    // 给地块儿浇水
    VOID WateringTrunk(Obj_Human* pOwner,SHORT nTrunkID);
private:
    TArray<Home, MAX_HOME_PLANT_COUNT> m_HomeArray; // 家园上的植物数组
    std::map<GUID64_t, Home*> m_HomeMap; // 家园上的植物映射表
};

#endif