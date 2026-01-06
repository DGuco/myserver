#include "table_init.h"

_MISSION_DIALOG    g_MissionDialogTable;
_PLANTING_GUIDE    g_PlantingGuideTable;

VOID TableInit::InitMissionDialogTable()
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
    Disk64Log( 111, "Load MissionDialog.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}

VOID TableInit::InitPlantingGuideTable()
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
    Disk64Log( 111, "Load PlantingGuide.txt ... OK! " ) ;
    __LEAVE_FUNCTION
}