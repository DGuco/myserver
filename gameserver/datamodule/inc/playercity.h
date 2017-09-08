//
// Created by dguco on 17-9-8.
//

#ifndef SERVER_PAYERCITY_H
#define SERVER_PAYERCITY_H

#include "playerbase.h"

class CPlayerCity : CPlayerData
{
public:
    CPlayerCity(std::shared_ptr<CPlayer> pPlayer);
    virtual ~CPlayerCity();
    virtual int Initialize();
};

#endif //SERVER_PAYERCITY_H
