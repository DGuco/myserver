//
//  gatectrl.h
//  �ͻ���tcp������ͷ�ļ�
//  Created by DGuco on 16/12/6.
//  Copyright ? 2016�� DGuco. All rights reserved.
//

#ifndef __PROXY_CTRL_H__
#define __PROXY_CTRL_H__

#include "base.h"
#include "proxy_def.h"
#include "tcp_server.h"
#include "safe_pointer.h"
#include "server_config.h"

class CProxyCtrl: public CSingleton<CProxyCtrl>
{
public:
	//���캯��
	CProxyCtrl();
	//��������
	~CProxyCtrl();
	//׼��run
	bool PrepareToRun();
	//run
	int Run();
private:
	//��ȡ�����ļ�
	bool ReadConfig();
};

#endif
