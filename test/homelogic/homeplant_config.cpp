#include "homeplant_config.h"

_MISSION_DIALOG    g_MissionDialogTable;

VOID HomePlantConfig::InitMissionDialogTable()
{
    __ENTER_FUNCTION
	enum
	{
		DialogIdx_DialogID,
		DialogIdx_MissionName,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_MISSION_DIALOG );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_MissionDialogTable.m_MissionDialog = new _MISSION_DIALOG_T[iTableCount];
    Assert(g_MissionDialogTable.m_MissionDialog);
	g_MissionDialogTable.m_nMissionDialogCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_MissionDialogTable.m_MissionDialog[i].nDialogID		= ThirdFile.Search_Posistion(i, DialogIdx_DialogID)->iValue;
		strncpy( g_MissionDialogTable.m_MissionDialog[i].szMissionName, ThirdFile.Search_Posistion(i,DialogIdx_MissionName)->pString, sizeof( g_MissionDialogTable.m_MissionDialog[i].szMissionName ) - 1 );
	}
    Disk64Log( SERVER_LOG_FILE, "Load MissionDialog.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

 ////////////////////////////AI生成/////////////////////////////////
_PLANTING_GUIDE    g_PlantingGuideTable;
VOID HomePlantConfig::InitPlantingGuideTable()
{
    __ENTER_FUNCTION
	enum
	{
		PlantingGuideIdx_SeedId,
		PlantingGuideIdx_Name,
		PlantingGuideIdx_Desc,
		PlantingGuideIdx_Quality,
		PlantingGuideIdx_BaseIcon,
		PlantingGuideIdx_CropIcon,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_PLANTING_GUIDE );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_PlantingGuideTable.m_PlantingGuide = new _PLANTING_GUIDE_T[iTableCount];
    Assert(g_PlantingGuideTable.m_PlantingGuide);
	g_PlantingGuideTable.m_nPlantingGuideCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_PlantingGuideTable.m_PlantingGuide[i].m_nSeedId = ThirdFile.Search_Posistion(i, PlantingGuideIdx_SeedId)->iValue;
		strncpy( g_PlantingGuideTable.m_PlantingGuide[i].m_aName, ThirdFile.Search_Posistion(i, PlantingGuideIdx_Name)->pString, sizeof( g_PlantingGuideTable.m_PlantingGuide[i].m_aName ) - 1 );
		strncpy( g_PlantingGuideTable.m_PlantingGuide[i].m_aDesc, ThirdFile.Search_Posistion(i, PlantingGuideIdx_Desc)->pString, sizeof( g_PlantingGuideTable.m_PlantingGuide[i].m_aDesc ) - 1 );
		g_PlantingGuideTable.m_PlantingGuide[i].m_nQuality = ThirdFile.Search_Posistion(i, PlantingGuideIdx_Quality)->iValue;
		g_PlantingGuideTable.m_PlantingGuide[i].m_nBaseIcon = ThirdFile.Search_Posistion(i, PlantingGuideIdx_BaseIcon)->iValue;
		g_PlantingGuideTable.m_PlantingGuide[i].m_nCropIcon = ThirdFile.Search_Posistion(i, PlantingGuideIdx_CropIcon)->iValue;
	}
    Disk64Log( SERVER_LOG_FILE, "Load PlantingGuide.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

_SEED_BASE         g_SeedBaseTable; 
// 新添加的种子基础表初始化函数
VOID HomePlantConfig::InitSeedBaseTable()
{
    __ENTER_FUNCTION
	enum
	{
		SeedBaseIdx_SeedId,
		SeedBaseIdx_Type,
		SeedBaseIdx_Quality,
		SeedBaseIdx_Name,
		SeedBaseIdx_Desc,
		SeedBaseIdx_Icon,
		SeedBaseIdx_Level,
		SeedBaseIdx_HomeLevel,
		SeedBaseIdx_LayerNum,
		SeedBaseIdx_MatureTime,
		SeedBaseIdx_WitherTime,
		SeedBaseIdx_MonsterID1,
		SeedBaseIdx_MonsterID2,
		SeedBaseIdx_MonsterID3,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_SEED_BASE );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_SeedBaseTable.m_SeedBase = new _SEED_BASE_T[iTableCount];
    Assert(g_SeedBaseTable.m_SeedBase);
	g_SeedBaseTable.m_nSeedBaseCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_SeedBaseTable.m_SeedBase[i].m_nSeedId = ThirdFile.Search_Posistion(i, SeedBaseIdx_SeedId)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nType = ThirdFile.Search_Posistion(i, SeedBaseIdx_Type)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nQuality = ThirdFile.Search_Posistion(i, SeedBaseIdx_Quality)->iValue;
		strncpy( g_SeedBaseTable.m_SeedBase[i].m_aName, ThirdFile.Search_Posistion(i, SeedBaseIdx_Name)->pString, sizeof( g_SeedBaseTable.m_SeedBase[i].m_aName ) - 1 );
		strncpy( g_SeedBaseTable.m_SeedBase[i].m_aDesc, ThirdFile.Search_Posistion(i, SeedBaseIdx_Desc)->pString, sizeof( g_SeedBaseTable.m_SeedBase[i].m_aDesc ) - 1 );
		g_SeedBaseTable.m_SeedBase[i].m_nIcon = ThirdFile.Search_Posistion(i, SeedBaseIdx_Icon)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nLevel = ThirdFile.Search_Posistion(i, SeedBaseIdx_Level)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nHomeLevel = ThirdFile.Search_Posistion(i, SeedBaseIdx_HomeLevel)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nLayerNum = ThirdFile.Search_Posistion(i, SeedBaseIdx_LayerNum)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nMatureTime = ThirdFile.Search_Posistion(i, SeedBaseIdx_MatureTime)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nWitherTime = ThirdFile.Search_Posistion(i, SeedBaseIdx_WitherTime)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nMonsterID1 = ThirdFile.Search_Posistion(i, SeedBaseIdx_MonsterID1)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nMonsterID2 = ThirdFile.Search_Posistion(i, SeedBaseIdx_MonsterID2)->iValue;
		g_SeedBaseTable.m_SeedBase[i].m_nMonsterID3 = ThirdFile.Search_Posistion(i, SeedBaseIdx_MonsterID3)->iValue;
	}
    Disk64Log( SERVER_LOG_FILE, "Load SeedBase.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

// 新添加的作物基础表初始化函数
VOID HomePlantConfig::InitCropBaseTable()
{
    __ENTER_FUNCTION
	enum
	{
		CropBaseIdx_CropId,
		CropBaseIdx_Type,
		CropBaseIdx_Quality,
		CropBaseIdx_Name,
		CropBaseIdx_Icon,
		CropBaseIdx_Desc,
		CropBaseIdx_LayerNum,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_CROP_BASE );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_CropBaseTable.m_CropBase = new _CROP_BASE_T[iTableCount];
    Assert(g_CropBaseTable.m_CropBase);
	g_CropBaseTable.m_nCropBaseCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_CropBaseTable.m_CropBase[i].m_nCropId = ThirdFile.Search_Posistion(i, CropBaseIdx_CropId)->iValue;
		g_CropBaseTable.m_CropBase[i].m_nType = ThirdFile.Search_Posistion(i, CropBaseIdx_Type)->iValue;
		g_CropBaseTable.m_CropBase[i].m_nQuality = ThirdFile.Search_Posistion(i, CropBaseIdx_Quality)->iValue;
		strncpy( g_CropBaseTable.m_CropBase[i].m_aName, ThirdFile.Search_Posistion(i, CropBaseIdx_Name)->pString, sizeof( g_CropBaseTable.m_CropBase[i].m_aName ) - 1 );
		g_CropBaseTable.m_CropBase[i].m_nIcon = ThirdFile.Search_Posistion(i, CropBaseIdx_Icon)->iValue;
		strncpy( g_CropBaseTable.m_CropBase[i].m_aDesc, ThirdFile.Search_Posistion(i, CropBaseIdx_Desc)->pString, sizeof( g_CropBaseTable.m_CropBase[i].m_aDesc ) - 1 );
		g_CropBaseTable.m_CropBase[i].m_nLayerNum = ThirdFile.Search_Posistion(i, CropBaseIdx_LayerNum)->iValue;
	}
    Disk64Log( SERVER_LOG_FILE, "Load CropBase.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

// 新增的全局变量定义
_PLANTING_GUIDE_STAR    g_PlantingGuideStarTable;  // 新添加的种植图鉴星级奖励表全局变量定义

// 新增的初始化函数实现
// 新添加的种植图鉴星级奖励表初始化函数
VOID HomePlantConfig::InitPlantingGuideStarTable()
{
    __ENTER_FUNCTION
	enum
	{
		PlantingGuideStarIdx_Id,
		PlantingGuideStarIdx_MinStar,
		PlantingGuideStarIdx_CropId,
		PlantingGuideStarIdx_Item1,
		PlantingGuideStarIdx_Item2,
		PlantingGuideStarIdx_Item3,
		PlantingGuideStarIdx_Item4,
		PlantingGuideStarIdx_Num1,
		PlantingGuideStarIdx_Num2,
		PlantingGuideStarIdx_Num3,
		PlantingGuideStarIdx_Num4,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_PLANTING_GUIDE_STAR );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_PlantingGuideStarTable.m_PlantingGuideStar = new _PLANTING_GUIDE_STAR_T[iTableCount];
    Assert(g_PlantingGuideStarTable.m_PlantingGuideStar);
	g_PlantingGuideStarTable.m_nPlantingGuideStarCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nId = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Id)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nMinStar = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_MinStar)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nCropId = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_CropId)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nItem1 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Item1)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nItem2 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Item2)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nItem3 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Item3)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nItem4 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Item4)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nNum1 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Num1)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nNum2 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Num2)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nNum3 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Num3)->iValue;
		g_PlantingGuideStarTable.m_PlantingGuideStar[i].m_nNum4 = ThirdFile.Search_Posistion(i, PlantingGuideStarIdx_Num4)->iValue;
	}
    Disk64Log( SERVER_LOG_FILE, "Load PlantingGuideStar.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

// 新增的全局变量定义
_PLANTING_GUIDE_WEIGHT    g_PlantingGuideWeightTable;  // 新添加的种植图鉴权重表全局变量定义

// 新增的初始化函数实现
// 新添加的种植图鉴权重表初始化函数
VOID HomePlantConfig::InitPlantingGuideWeightTable()
{
    __ENTER_FUNCTION
	enum
	{
		PlantingGuideLevelIdx_Id,
		PlantingGuideLevelIdx_Level,
		PlantingGuideLevelIdx_PlantId1,
		PlantingGuideLevelIdx_PlantId2,
		PlantingGuideLevelIdx_PlantId3,
		PlantingGuideLevelIdx_PlantId4,
		PlantingGuideLevelIdx_Weight1,
		PlantingGuideLevelIdx_Weight2,
		PlantingGuideLevelIdx_Weight3,
		PlantingGuideLevelIdx_Weight4,
	};

	DBCFile ThirdFile(0);
	BOOL ret = ThirdFile.OpenFromTXT( FILE_PLANTING_GUIDE_WEIGHT );

	INT iTableCount = ThirdFile.GetRecordsNum();
	Assert(iTableCount>0);

	g_PlantingGuideWeightTable.m_PlantingGuideWeight = new _PLANTING_GUIDE_WEIGHT_T[iTableCount];
    Assert(g_PlantingGuideWeightTable.m_PlantingGuideWeight);
	g_PlantingGuideWeightTable.m_nPlantingGuideWeightCount = iTableCount;

	for (INT i = 0; i < iTableCount; ++i)
	{
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nId = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Id)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nLevel = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Level)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nPlantId1 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_PlantId1)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nPlantId2 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_PlantId2)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nPlantId3 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_PlantId3)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nPlantId4 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_PlantId4)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nWeight1 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Weight1)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nWeight2 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Weight2)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nWeight3 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Weight3)->iValue;
		g_PlantingGuideWeightTable.m_PlantingGuideWeight[i].m_nWeight4 = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Weight4)->iValue;
	}
    Disk64Log( SERVER_LOG_FILE, "Load PlantingGuideWeight.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

// 全局变量定义
_PLANTING_GUIDE_LEVEL g_PlantingGuideLevelTable;
// 初始化函数实现
void HomePlantConfig::InitPlantingGuideLevelTable()
{
    enum PlantingGuideLevelIdx 
    {
        PlantingGuideLevelIdx_Id = 0,
        PlantingGuideLevelIdx_Level,
        PlantingGuideLevelIdx_CropId,
        PlantingGuideLevelIdx_CropExpGain,
        PlantingGuideLevelIdx_ReqExp,
    };

    DBCFile ThirdFile(0);
    BOOL ret = ThirdFile.OpenFromTXT(FILE_PLANTING_GUIDE_LEVEL);

    INT iTableCount = ThirdFile.GetRecordsNum();
    Assert(iTableCount > 0);

    g_PlantingGuideLevelTable.m_nCount = iTableCount;
    g_PlantingGuideLevelTable.m_pPlantingGuideLevelT = new _PLANTING_GUIDE_LEVEL_T[iTableCount];
    _PLANTING_GUIDE_LEVEL_T *pdata = g_PlantingGuideLevelTable.m_pPlantingGuideLevelT;

    for (INT i = 0; i < iTableCount; i++)
    {
        pdata->m_nId = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Id)->iValue;
        pdata->m_nLevel = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_Level)->iValue;
        pdata->m_nCropId = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_CropId)->iValue;
        pdata->m_nCropExpGain = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_CropExpGain)->iValue;
        pdata->m_nReqExp = ThirdFile.Search_Posistion(i, PlantingGuideLevelIdx_ReqExp)->iValue;
        pdata++;
    }

    Disk64Log( SERVER_LOG_FILE, "Load PlantingGuideLevel.txt ... OK! " ) ;
}

// 全局变量定义
_PLANT_GROUP g_PlantGroupTable;
// 初始化函数实现
void HomePlantConfig::InitPlantGroupTable()
{
    enum PlantGroupIdx 
    {
        PlantGroupIdx_Id = 0,
        PlantGroupIdx_Type,
        PlantGroupIdx_Name,
        PlantGroupIdx_Level,
        PlantGroupIdx_CompanionPlanting1,
        PlantGroupIdx_CompanionPlanting2,
        PlantGroupIdx_CompanionPlanting3,
        PlantGroupIdx_CompanionPlanting4,
    };

    DBCFile ThirdFile(0);
    BOOL ret = ThirdFile.OpenFromTXT(FILE_PLANT_GROUP);

    INT iTableCount = ThirdFile.GetRecordsNum();
    Assert(iTableCount > 0);

    g_PlantGroupTable.m_pPlantGroupT = new _PLANT_GROUP_T[iTableCount];
    Assert(g_PlantGroupTable.m_pPlantGroupT);
    g_PlantGroupTable.m_nCount = iTableCount;

    for (INT i = 0; i < iTableCount; i++)
    {
        g_PlantGroupTable.m_pPlantGroupT[i].m_nId = ThirdFile.Search_Posistion(i, PlantGroupIdx_Id)->iValue;
        g_PlantGroupTable.m_pPlantGroupT[i].m_nType = ThirdFile.Search_Posistion(i, PlantGroupIdx_Type)->iValue;
        //strncpy(g_PlantGroupTable.m_pPlantGroupT[i].m_szName, ThirdFile.Search_Posistion(i, PlantGroupIdx_Name)->szValue, MAX_STRING - 1);
        g_PlantGroupTable.m_pPlantGroupT[i].m_nLevel = ThirdFile.Search_Posistion(i, PlantGroupIdx_Level)->iValue;
        g_PlantGroupTable.m_pPlantGroupT[i].m_nCompanionPlanting1 = ThirdFile.Search_Posistion(i, PlantGroupIdx_CompanionPlanting1)->iValue;
        g_PlantGroupTable.m_pPlantGroupT[i].m_nCompanionPlanting2 = ThirdFile.Search_Posistion(i, PlantGroupIdx_CompanionPlanting2)->iValue;        
        g_PlantGroupTable.m_pPlantGroupT[i].m_nCompanionPlanting3 = ThirdFile.Search_Posistion(i, PlantGroupIdx_CompanionPlanting3)->iValue;
        g_PlantGroupTable.m_pPlantGroupT[i].m_nCompanionPlanting4 = ThirdFile.Search_Posistion(i, PlantGroupIdx_CompanionPlanting4)->iValue;
    }

    Disk64Log(SERVER_LOG_FILE, "Load PlantGroup.txt ... OK! ");
}