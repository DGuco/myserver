//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include <memory>
#include "byte_buff.h"
#include "net_work.h"
#include "thread_pool.h"
#include "query_result.h"
#include "database.h"
#include "message_factory.h"
#include "config.h"
#include "safe_pointer.h"

//#include "protoconfig.h"

using namespace std;

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER                2                    // ���proxy server ��Ŀ

#define MAXHANDLENUMBER                2

#define SECSOFONEHOUR                3600

#define CHECKINTERVAL                10                    // ���ʱ��������λ����
#define PROXYKEEPALIVESECONDS        (3*CHECKINTERVAL)    // ��proxy��������ʱʱ��
#define MAXPROXYCODELEN                1024                // ��proxy���ʹ���������鳤��
#define STATISTICSLEN                1024
#define MAXNUMBERLEN                10
#define SAVEINTERVAL                300


class CSharedMem;

class CDBCtrl: public CSingleton<CDBCtrl>
{
};

#endif


