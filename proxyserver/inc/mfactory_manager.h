/*****************************************************************
* FileName:mfactory_manager.h
* Summary :
* Date	  :2024-3-25
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __MFACTORY_MANAGER_H__
#define __MFACTORY_MANAGER_H__
#include "singleton.h"
#include <unordered_map>
#include "safe_pointer.h"
#include "message_factory.h"

using namespace std;

class CMessageFactoryManager : public CSingleton<CMessageFactoryManager>
{
public:
	CMessageFactoryManager();
	~CMessageFactoryManager();
	void Init();
	void RegisterFactory(int messId, CSafePtr<CMessageFactory> pFactory);
	CSafePtr<CMessageFactory> GetFactory(int messId);
private:
	std::unordered_map<int, CSafePtr<CMessageFactory>>	m_FatoryMap;
};

#endif //__MFACTORY_MANAGER_H__

