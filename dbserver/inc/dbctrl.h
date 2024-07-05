//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018年 DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "base.h"
#include "query_result.h"
#include "database.h"
#include "safe_pointer.h"

//#include "protoconfig.h"

using namespace std;

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER                2                    // 最大proxy server 数目

#define MAXHANDLENUMBER                2

#define SECSOFONEHOUR                3600

#define CHECKINTERVAL                10                    // 检查时间间隔，单位：秒
#define PROXYKEEPALIVESECONDS        (3*CHECKINTERVAL)    // 与proxy的心跳超时时间
#define MAXPROXYCODELEN                1024                // 向proxy发送纯命令的最大块长度
#define STATISTICSLEN                1024
#define MAXNUMBERLEN                10
#define SAVEINTERVAL                300


class CSharedMem;

class CDBCtrl: public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	int PrepareToRun();
};

#endif


