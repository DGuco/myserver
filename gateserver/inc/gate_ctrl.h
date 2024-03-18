//
//  gatectrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __GATE_CTRL_H__
#define __GATE_CTRL_H__

#include "gate_def.h"
#include "server_tool.h"
#include "base.h"
#include "config.h"
#include "tcp_server.h"
#include "safe_pointer.h"

class CGateCtrl: public CSingleton<CGateCtrl>
{
public:
	//构造函数
	CGateCtrl();
	//析构函数
	~CGateCtrl();
	//准备run
	bool PrepareToRun();
	//run
	int Run();
private:
	//读取配置文件
	bool ReadConfig();
private:
	SafePointer<CServerConfig> m_pConfig;
};

#endif
