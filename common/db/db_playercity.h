//
// Created by dguco on 17-9-8.
//

#ifndef PLAYERCITY_H
#define PLAYERCITY_H

#include "database.h"
#include "base.h"

struct DBPlayerCity
{
    int m_iCityId;

    int Save(CSafePtr<IDataBase> pDataBase)
    {
        return 0;
    }
};

#endif //PLAYERCITY_H
