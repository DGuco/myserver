/*****************************************************************
* FileName:game_ctrl.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GAME_CTRL_H__
#define __GAME_CTRL_H__
#include "server_tool.h"

class CGameCtrl : public CSingleton<CGameCtrl>
{
public:
	//���캯��
	CGameCtrl();
	//��������
	~CGameCtrl();
	//׼��run
	bool PrepareToRun();
	//run
	int Run();
	//
	bool ReadConfig();
};

#endif //__GAME_CTRL_H__
