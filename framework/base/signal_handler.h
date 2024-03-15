/*****************************************************************
* FileName:signal_handler.h
* Summary : 异常信号处理
* Date	  :2024-3-15
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include "server_tool.h"

class CSignalHandler : public CSingleton<CSignalHandler>
{
public:
	CSignalHandler();
	~CSignalHandler();
	void RegisterHandler();
};

#endif //__SIGNAL_HANDLER_H__
