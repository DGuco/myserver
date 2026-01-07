#ifndef __TABLE_INIT_H__
#define __TABLE_INIT_H__

#include "base.h"
#include "test_common.h"
#include <string.h>
#include <stdio.h>

#define FILE_MISSION_DIALOG "MissionDialog.txt"
#define FILE_PLANTING_GUIDE "PlantingGuide.txt"
#define FILE_SEED_BASE      "SeedBase.txt"
#define FILE_CROP_BASE      "CropBase.txt"  // 新添加的作物基础表文件名
#define FILE_PLANTING_GUIDE_STAR "PlantingGuideStar.txt"  // 新添加的种植图鉴星级奖励表文件名
#define FILE_PLANTING_GUIDE_LEVEL "PlantingGuideLevel.txt"  // 新添加的种植图鉴等级表文件名
#define FILE_PLANTING_GUIDE_WEIGHT "PlantingGuideWeight.txt"  // 新添加的种植图鉴权重表文件名
#define FILE_PLANT_GROUP            "PlantGroup.txt"

struct _MISSION_DIALOG_T
{
    SHORT        nDialogID;
	CHAR        szMissionName[256];        //任务名称

    _MISSION_DIALOG_T()
    {
        nDialogID = 0;
        memset(szMissionName, 0, sizeof(szMissionName));
    }
};

struct _MISSION_DIALOG
{
	_MISSION_DIALOG_T*    m_MissionDialog;
	SHORT                m_nMissionDialogCount;
	_MISSION_DIALOG_T*    Get(SHORT nDialogID) { return NULL; };

    _MISSION_DIALOG()
    {
        m_MissionDialog = NULL;
        m_nMissionDialogCount = 0;
    }

    ~_MISSION_DIALOG()
    {
        if (m_MissionDialog)
        {
            delete[] m_MissionDialog;
            m_MissionDialog = NULL;
        }
        m_nMissionDialogCount = 0;
    }
};

 ////////////////////////////AI生成/////////////////////////////////
// PlantingGuide配置结构
struct _PLANTING_GUIDE_T
{
    INT         m_nSeedId;      // 唯一识别id，与种子id一一对应
    CHAR        m_aName[64];    // 名称，读字典
    CHAR        m_aDesc[256];   // 描述，读字典
    INT         m_nQuality;     // 品质，2=绿，3=蓝，4=紫，5=橙
    INT         m_nBaseIcon;    // 图标，读取resourceid，此图标为左侧图鉴用大图标尺寸
    INT         m_nCropIcon;    // 图标，读取resourceid，此为作物用通用图标尺寸

    _PLANTING_GUIDE_T()
    {
        m_nSeedId = 0;
        memset(m_aName, 0, sizeof(m_aName));
        memset(m_aDesc, 0, sizeof(m_aDesc));
        m_nQuality = 0;
        m_nBaseIcon = 0;
        m_nCropIcon = 0;
    }
};

struct _PLANTING_GUIDE
{
    _PLANTING_GUIDE_T*    m_PlantingGuide;
    INT                   m_nPlantingGuideCount;

    _PLANTING_GUIDE_T* Get(INT nSeedId)
    {
        for (INT i = 0; i < m_nPlantingGuideCount; ++i)
        {
            if (m_PlantingGuide[i].m_nSeedId == nSeedId)
            {
                return &m_PlantingGuide[i];
            }
        }
        return NULL;
    }

    _PLANTING_GUIDE()
    {
        m_PlantingGuide = NULL;
        m_nPlantingGuideCount = 0;
    }

    ~_PLANTING_GUIDE()
    {
        if (m_PlantingGuide)
        {
            delete[] m_PlantingGuide;
            m_PlantingGuide = NULL;
        }
        m_nPlantingGuideCount = 0;
    }
};

extern _PLANTING_GUIDE    g_PlantingGuideTable;

// 种子基础表结构
struct _SEED_BASE_T
{
    INT         m_nSeedId;      // 种子id，唯一识别id，此id需定义等级段，此等级段道具id与commonitem道具互斥，不进通用背包
    INT         m_nType;        // 作物类型，1=经济田种子，2=苗圃种子
    INT         m_nQuality;     // 品质，2=绿，3=蓝，4=紫，5=橙
    CHAR        m_aName[64];    // 名称，读字典
    CHAR        m_aDesc[256];   // 描述，读字典
    INT         m_nIcon;        // 图标
    INT         m_nLevel;       // 归属种植等级，播种等级页签筛选用
    INT         m_nHomeLevel;   // 家园等级达到配置条件方可执行播种
    INT         m_nLayerNum;    // 可叠加数量，均统一9999
    INT         m_nMatureTime;  // 配置秒数
    INT         m_nWitherTime;  // 配置秒数
    INT         m_nMonsterID1;  // 配置monster表id
    INT         m_nMonsterID2;  // 配置monster表id
    INT         m_nMonsterID3;  // 配置monster表id

    _SEED_BASE_T()
    {
        m_nSeedId = 0;
        m_nType = 0;
        m_nQuality = 0;
        memset(m_aName, 0, sizeof(m_aName));
        memset(m_aDesc, 0, sizeof(m_aDesc));
        m_nIcon = 0;
        m_nLevel = 0;
        m_nHomeLevel = 0;
        m_nLayerNum = 0;
        m_nMatureTime = 0;
        m_nWitherTime = 0;
        m_nMonsterID1 = 0;
        m_nMonsterID2 = 0;
        m_nMonsterID3 = 0;
    }
};

struct _SEED_BASE
{
    _SEED_BASE_T*    m_SeedBase;
    INT              m_nSeedBaseCount;

    _SEED_BASE_T* Get(INT nSeedId)
    {
        for (INT i = 0; i < m_nSeedBaseCount; ++i)
        {
            if (m_SeedBase[i].m_nSeedId == nSeedId)
            {
                return &m_SeedBase[i];
            }
        }
        return NULL;
    }

    _SEED_BASE()
    {
        m_SeedBase = NULL;
        m_nSeedBaseCount = 0;
    }

    ~_SEED_BASE()
    {
        if (m_SeedBase)
        {
            delete[] m_SeedBase;
            m_SeedBase = NULL;
        }
        m_nSeedBaseCount = 0;
    }
};

extern _SEED_BASE    g_SeedBaseTable;  // 种子基础表全局变量声明

// CropBase配置结构
struct _CROP_BASE_T
{
    INT         m_nCropId;      // 唯一识别id
    INT         m_nType;        // 类型，1=经济田作物，2=苗圃作物
    INT         m_nQuality;     // 品质，2=绿，3=蓝，4=紫，5=橙
    CHAR        m_aName[64];    // 名称，读字典
    INT         m_nIcon;        // 图标，读取resourceid
    CHAR        m_aDesc[256];   // 描述
    INT         m_nLayerNum;    // 可叠加数量

    _CROP_BASE_T()
    {
        m_nCropId = 0;
        m_nType = 0;
        m_nQuality = 0;
        memset(m_aName, 0, sizeof(m_aName));
        m_nIcon = 0;
        memset(m_aDesc, 0, sizeof(m_aDesc));
        m_nLayerNum = 0;
    }
};

struct _CROP_BASE
{
    _CROP_BASE_T*    m_CropBase;
    INT              m_nCropBaseCount;

    _CROP_BASE_T* Get(INT nCropId)
    {
        for (INT i = 0; i < m_nCropBaseCount; ++i)
        {
            if (m_CropBase[i].m_nCropId == nCropId)
            {
                return &m_CropBase[i];
            }
        }
        return NULL;
    }

    _CROP_BASE()
    {
        m_CropBase = NULL;
        m_nCropBaseCount = 0;
    }

    ~_CROP_BASE()
    {
        if (m_CropBase)
        {
            delete[] m_CropBase;
            m_CropBase = NULL;
        }
        m_nCropBaseCount = 0;
    }
};
_CROP_BASE         g_CropBaseTable;  // 新添加的作物基础表全局变量定义

// 新增的PlantingGuideStar配置结构
struct _PLANTING_GUIDE_STAR_T
{
    INT         m_nId;          // 唯一识别id，此id索引种子表id
    INT         m_nMinStar;     // 配置领奖需求达到的星级下限，≥即可领取奖励
    INT         m_nCropId;      // 产物id索引作物表id
    INT         m_nItem1;       // 读commonitem表配置，若无，则读SeedBase
    INT         m_nItem2;       // 读commonitem表配置，若无，则读SeedBase
    INT         m_nItem3;       // 读commonitem表配置，若无，则读SeedBase
    INT         m_nItem4;       // 读commonitem表配置，若无，则读SeedBase
    INT         m_nNum1;        // 配置奖励道具数量
    INT         m_nNum2;        // 配置奖励道具数量
    INT         m_nNum3;        // 配置奖励道具数量
    INT         m_nNum4;        // 配置奖励道具数量

    _PLANTING_GUIDE_STAR_T()
    {
        m_nId = 0;
        m_nMinStar = 0;
        m_nCropId = 0;
        m_nItem1 = 0;
        m_nItem2 = 0;
        m_nItem3 = 0;
        m_nItem4 = 0;
        m_nNum1 = 0;
        m_nNum2 = 0;
        m_nNum3 = 0;
        m_nNum4 = 0;
    }
};

struct _PLANTING_GUIDE_STAR
{
    _PLANTING_GUIDE_STAR_T*    m_PlantingGuideStar;
    INT                        m_nPlantingGuideStarCount;

    _PLANTING_GUIDE_STAR_T* Get(INT nId)
    {
        for (INT i = 0; i < m_nPlantingGuideStarCount; ++i)
        {
            if (m_PlantingGuideStar[i].m_nId == nId)
            {
                return &m_PlantingGuideStar[i];
            }
        }
        return NULL;
    }

    _PLANTING_GUIDE_STAR()
    {
        m_PlantingGuideStar = NULL;
        m_nPlantingGuideStarCount = 0;
    }

    ~_PLANTING_GUIDE_STAR()
    {
        if (m_PlantingGuideStar)
        {
            delete[] m_PlantingGuideStar;
            m_PlantingGuideStar = NULL;
        }
        m_nPlantingGuideStarCount = 0;
    }
};

extern _PLANTING_GUIDE_STAR    g_PlantingGuideStarTable;  // 种植图鉴星级奖励表全局变量声明


// 新增的PlantingGuideWeight配置结构
struct _PLANTING_GUIDE_WEIGHT_T
{
    INT         m_nId;          // 唯一识别id，此id索引种子表id
    INT         m_nLevel;       // 手册等级，初始0级
    INT         m_nPlantId1;    // 产物id索引作物表id，对应品质绿
    INT         m_nPlantId2;    // 产物id索引作物表id，对应品质蓝
    INT         m_nPlantId3;    // 产物id索引作物表id，对应品质紫
    INT         m_nPlantId4;    // 产物id索引作物表id，对应品质橙
    INT         m_nWeight1;     // 配置对应权重
    INT         m_nWeight2;     // 配置对应权重
    INT         m_nWeight3;     // 配置对应权重
    INT         m_nWeight4;     // 配置对应权重

    _PLANTING_GUIDE_WEIGHT_T()
    {
        m_nId = 0;
        m_nLevel = 0;
        m_nPlantId1 = -1;
        m_nPlantId2 = -1;
        m_nPlantId3 = -1;
        m_nPlantId4 = -1;
        m_nWeight1 = -1;
        m_nWeight2 = -1;
        m_nWeight3 = -1;
        m_nWeight4 = -1;
    }
};

struct _PLANTING_GUIDE_WEIGHT
{
    _PLANTING_GUIDE_WEIGHT_T*    m_PlantingGuideWeight;
    INT                         m_nPlantingGuideWeightCount;

    _PLANTING_GUIDE_WEIGHT_T* Get(INT nId)
    {
        for (INT i = 0; i < m_nPlantingGuideWeightCount; ++i)
        {
            if (m_PlantingGuideWeight[i].m_nId == nId)
            {
                return &m_PlantingGuideWeight[i];
            }
        }
        return NULL;
    }

    _PLANTING_GUIDE_WEIGHT()
    {
        m_PlantingGuideWeight = NULL;
        m_nPlantingGuideWeightCount = 0;
    }

    ~_PLANTING_GUIDE_WEIGHT()
    {
        if (m_PlantingGuideWeight)
        {
            delete[] m_PlantingGuideWeight;
            m_PlantingGuideWeight = NULL;
        }
        m_nPlantingGuideWeightCount = 0;
    }
};

extern _PLANTING_GUIDE_WEIGHT    g_PlantingGuideWeightTable;  // 种植图鉴权重表全局变量声明

// 结构体定义
struct _PLANTING_GUIDE_LEVEL_T
{
    _PLANTING_GUIDE_LEVEL_T()
    {
        m_nId = 0;
        m_nLevel = 0;
        m_nCropId = 0;
        m_nCropExpGain = 0;
        m_nReqExp = 0;
    }
    INT     m_nId;                // 唯一识别id，索引种子表id
    INT     m_nLevel;             // 手册等级，初始0级
    INT     m_nCropId;            // 产物id索引作物表id
    INT     m_nCropExpGain;       // 当前行消耗作物增加的经验
    INT     m_nReqExp;            // 提升至下一级需求的经验值
};

struct _PLANTING_GUIDE_LEVEL
{
    ~_PLANTING_GUIDE_LEVEL()
    {
        if (m_pPlantingGuideLevelT) 
        {
            delete[] m_pPlantingGuideLevelT;
            m_pPlantingGuideLevelT = NULL;
        }
        m_nCount = 0;
    }
    _PLANTING_GUIDE_LEVEL_T *m_pPlantingGuideLevelT;
    INT m_nCount;

        // Get方法实现
    _PLANTING_GUIDE_LEVEL_T* Get(INT nId)
    {
        for (INT i = 0; i < m_nCount; i++)
        {
            if (m_pPlantingGuideLevelT[i].m_nId == nId)
            {
                return &m_pPlantingGuideLevelT[i];
            }
        }
        return NULL;
    }
};
// 全局变量声明
extern _PLANTING_GUIDE_LEVEL g_PlantingGuideLevelTable;

// 结构体定义
struct _PLANT_GROUP_T
{
    _PLANT_GROUP_T()
    {
        m_nId = 0;
        m_nType = 0;
        memset(m_szName, 0, sizeof(m_szName));
        m_nLevel = 0;
        m_nCompanionPlanting1 = -1;
        m_nCompanionPlanting2 = -1;
        m_nCompanionPlanting3 = -1;
        m_nCompanionPlanting4 = -1;
    }
    INT     m_nId;                // 唯一识别id
    INT     m_nType;              // 类型，关联涉及地块可种植作物类型
    CHAR    m_szName[256]; // 配置名称字典
    INT     m_nLevel;             // 配置解锁需求的家园等级
    INT     m_nCompanionPlanting1;// 配置关联的具体种植田的id
    INT     m_nCompanionPlanting2;// 配置关联的具体种植田的id
    INT     m_nCompanionPlanting3;// 配置关联的具体种植田的id
    INT     m_nCompanionPlanting4;// 配置关联的具体种植田的id
};

struct _PLANT_GROUP
{
    _PLANT_GROUP()
    {
        m_pPlantGroupT = NULL;
        m_nCount = 0;
    }

    ~_PLANT_GROUP()
    {
        if (m_pPlantGroupT) delete[] m_pPlantGroupT;
    }

    // Get方法实现
    _PLANT_GROUP_T* GetPlantGroup(INT nId)
    {
        for (INT i = 0; i < m_nCount; i++)
        {
            if (m_pPlantGroupT[i].m_nId == nId)
            {
                return &m_pPlantGroupT[i];
            }
        }
        return NULL;
    }
    _PLANT_GROUP_T *m_pPlantGroupT;
    INT m_nCount;
};

// 全局变量声明
extern _PLANT_GROUP g_PlantGroupTable;

class HomePlantConfig
{
public:
	HomePlantConfig() {};
	~HomePlantConfig() {};
    VOID InitMissionDialogTable();
    ////////////////////////////AI生成/////////////////////////////////
    VOID InitPlantingGuideTable();
    VOID InitSeedBaseTable();  // 种子基础表初始化函数声明
    VOID InitCropBaseTable();  // 新添加的作物基础表初始化函数声明
    VOID InitPlantingGuideStarTable();  // 新添加的种植图鉴星级奖励表初始化函数声明
    VOID InitPlantingGuideWeightTable();  // 新添加的种植图鉴权重表初始化函数声明
    VOID InitPlantingGuideLevelTable(); // 新添加的种植图鉴等级表初始化函数声明
    VOID InitPlantGroupTable();// 新添加的种植群表初始化函数声明
};

#endif