//
//  coremodule.h
//  coremodule.h类头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//
#ifndef SERVER_COREMODULE_H
#define SERVER_COREMODULE_H

#include "logicmodule.h"
#include "../../../framework/base/servertool.h"

class CCoreModule : public CLogicModule,public CSingleton<CCoreModule>
{
public:
    CCoreModule();
    virtual  ~CCoreModule();
};

#endif //SERVER_COREMODULE_H
