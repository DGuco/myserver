#ifndef __TABLE_INIT_H__
#define __TABLE_INIT_H__

#include "base.h"
#include "test_common.h"
#include "dbcfile.h"
#include <string.h>
#include <stdio.h>

#define FILE_MISSION_DIALOG "MissionDialog.txt"
#define FILE_PLANTING_GUIDE "PlantingGuide.txt"

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


class TableInit
{
public:
	TableInit() {};
	~TableInit() {};
    VOID InitMissionDialogTable();
    VOID InitPlantingGuideTable();
};
#endif