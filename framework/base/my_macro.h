//
// Created by DGcuo on 17-7-5.
//

#ifndef SERVER_MY_NACRO_H
#define SERVER_MY_NACRO_H

// 获取数组元素最大个数
#define ARRAY_CNT(a)	int(sizeof(a) / sizeof(a[0]))

//仅用于value为指针类型的map
#define MAP_SAFE_FIND(map, key) 	\
	do 								\
	{ 								\
        auto it = map.find(key);    \
        if (it != map.end())        \
        {                           \
            return it->second;      \
        }                           \
        return NULL;                \
	} while(0);

//仅用于value为指针类型的map
#define MAP_SAFE_DELETE(map, key) 	\
	do 								\
	{ 								\
        auto it = map.find(key);    \
        if (it != map.end())        \
        {                           \
            delete it->second;      \
        }                           \
        map.erase(key);             \
	} while(0);

#define SAFE_DELETE(pointer)        \
    do                              \
    {                               \
        if (pointer != NULL)        \
        {                           \
            delete pointer;         \
        }                           \
    }while(0);


#define POP_SESSION(func, id1, id2, rtn1, rtn2)	if (pTmpSession == NULL)																																							\
																						{																																																		\
																							CConfigure* pTmpConfig = CConfigMgr::GetSingletonPtr()->GetConfig();																		\
																							MY_ASSERT_LOG("db", pTmpConfig != NULL , rtn1);																											\
																							time_t tTmpTimeout = (time_t) pTmpConfig->gameconfig().sessiontimeout();																\
																							pTmpSession = (CSession*) CTimerManager::GetSingletonPtr()->CreateSession(tTmpTimeout, func, id1, id2);		\
																							MY_ASSERT_LOG("db", pTmpSession != NULL, rtn2)																											\
																						}


#define SET_SESSION_PARAM2(p1, p2, rtn)	int iTmpParaNum = 2;																										\
																				int aiTmpParas[] = {p1, p2};																							\
																				int iRet = pTmpSession->SetOtherInfos(iTmpParaNum, aiTmpParas);					\
																				if (iRet != 0)																														\
																				{																																			\
																					LOG_ERROR("db", "[{} : {} : {}] SetOtherInfos failed, iRet={}.",						\
																							__MY_FILE__, __LINE__, __FUNCTION__, iRet);													\
																					CTimerManager::GetSingletonPtr()->DestroyObject(pTmpSession->get_id());	\
																					rtn;																																	\
																				}


#define SET_SESSION_PARAM1(p1, rtn)	int iTmpParaNum = 1;																										\
																		int aiTmpParas[] = {p1};																									\
																		int iRet = pTmpSession->SetOtherInfos(iTmpParaNum, aiTmpParas);					\
																		if (iRet != 0)																														\
																		{																																			\
																			LOG_ERROR("db", "[{} : {} : {}] SetOtherInfos failed, iRet={}.",						\
																					__MY_FILE__, __LINE__, __FUNCTION__, iRet);													\
																			CTimerManager::GetSingletonPtr()->DestroyObject(pTmpSession->get_id());	\
																			rtn;																																	\
																		}

#endif //SERVER_MY_NACRO_H
