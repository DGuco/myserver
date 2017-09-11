#include "../inc/parameter_config.h"

ParameterItem::ParameterItem()
{
    _common.clear();
    _vi.clear();
}

ParameterItem::~ParameterItem()
{
    
}

ParameterConfig::ParameterConfig()
{
    _datas.clear();
}

ParameterConfig::~ParameterConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ParameterConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto oid = it->second["id"];
        auto value = it->second["value"];
        auto item = new ParameterItem;
        auto key = oid.ToString();
        if (key == "initialBuilding") {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "initialBuildingTest") {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "MonsterDailyMax")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "monsterMaxWeek")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "massedCastleLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "massTeamNum")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "massTime")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "ChainMilitaryTent_3")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainMilitaryTent_5")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainMilitaryTent_7")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainMilitaryTent_9")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainMilitaryTent_11")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainRes_3")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainRes_5")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainRes_7")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "ChainRes_9")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "ChainRes_11")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "AllianceInviteRefreshTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "LikeInRanking")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "DataVersion")
        {
            item->_str = value.ToString();
        }
        else if (key == "JoinAllianceHonor")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "initialLand")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "spyPlayerSkillNeedWatchTowerLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "initialBuildTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "version")
        {
            item->_str = value.ToString();
        }
        else if (key == "worldMapLimitPerArea")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "initTaskFirst")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "initialHead")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "ChangeTalentCost")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "initialPlayerLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "resource_for_InitUser")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "initialItem")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "playerHead")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "worldMapGridModel")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "worldMapAreaOrder")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "hospitalSoldier")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "dailyScore")
        {
            auto ar = value.ToArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if(key == "changeGuildNamePrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "changeGuildAbbreviationPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "replaceGuildOwner")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "buildGuildCastleLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "freeBuildGuildCastleLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "buildGuildPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "joinGuildReward")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "joinGuildReward")
        {
           item->_integer = value.ToInt();
        }
        else if (key == "recommendMemberRefreshPrice")
        {
            item->_integer = value.ToInt();

        }
        else if (key == "trapInitial")/////
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "changePositionPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "initArmyFirst") ////
        {
            auto obj =value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "changeNamePrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "changeNameExpend")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                auto v = new std::vector<int>;
                item->_common[it->first] = v;
                auto ar = it->second.ToArray();
                for (auto vl = ar.begin(); vl!=ar.end(); ++vl) {
                    v->push_back(vl->ToInt());
                }
            }
        }
        else if (key == "fristChangeNameRewards")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "changeHeadExpend")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                auto v = new std::vector<int>;
                item->_common[it->first] = v;
                auto ar = it->second.ToArray();
                for (auto vl = ar.begin(); vl!=ar.end(); ++vl) {
                    v->push_back(vl->ToInt());
                }
            }
        }
        else if (key == "changeHeadPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "changeHeadTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "farmOutputTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "NumberTroopDispatchFirst")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "outfirePrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "wallRepairValue")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "wallRepairTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "wishFreeTimes")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "wishCri")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "wishStoneCastleLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "wishIronCastleLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "resourceReturn")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "initialTraining")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "worldMapTimePerGrid")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "worldMapSpeedUpTimes")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "TowerUnlockLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "worldMapSpeedUp")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "woundOreder")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "ResParam")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "initBuildFirst")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "farmOutputPer")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "skillInitial") ////
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "DefaultArmy") ////
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_nomap[atoi(it->first.c_str())] = it->second.ToInt();
            }
        }
        else if (key == "addSpeedPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "massAddSpeedPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "returnPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "massReturnPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "moveCityExpend")
        {
            auto obj = value.ToObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                auto v = new std::vector<int>;
                item->_common[it->first] = v;
                auto ar = it->second.ToArray();
                for (auto vl = ar.begin(); vl!=ar.end(); ++vl) {
                    v->push_back(vl->ToInt());
                }
            }
        }
        else if (key == "moveCityPrice")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "timeContributeJoinGuild")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "GuildShopLimit")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "BarUnlockLevel")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "GuildBenefitsTimes")
        {
            item->_integer = value.ToInt();
        }
        else if(key == "AllianceConflict")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "MarketTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "MarketSpeed")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "MarketProfit")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "MarketAdd")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "MarketAddParam")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "MarketReduce")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "MarketReduceParam")
        {
            item->_floater = value.ToFloat();
        }
        else if (key == "GiftWeight")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "SeasonReward")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "DepotSpeed")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "collectLimit")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "PriceofSearch")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "ColourWeight")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "TypeWeight")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "IsPositiveWeight")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "BeginTime")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "DragonBrowTime")
        {
            item->_integer = value.ToInt();
        }
        else if(key == "RobotLimitPerArea")
        {
            //机器人饱和度
            item->_floater = value.ToFloat();
        }
        else if(key == "RobotNum")
        {
            //初始机器人数量
            item->_integer = value.ToInt();
        }
        else if (key == "ObstacleNum")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        else if (key == "RobotTalkInterval")
        {
            item->_integer = value.ToInt();
        }
        else if (key == "talent")
        {
            auto obj = value.ToArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->ToInt());
            }
        }
        
        _datas[oid.ToString()] = item;
    }
    return 0;
}

void ParameterConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

ParameterItem* ParameterConfig::item(const std::string& key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}
