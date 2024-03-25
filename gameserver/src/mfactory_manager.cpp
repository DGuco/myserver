#include "mfactory_manager.h"

CMessageFactoryManager::CMessageFactoryManager()
{

}

CMessageFactoryManager::~CMessageFactoryManager()
{
	for (auto it = m_FatoryMap.begin(); it != m_FatoryMap.end(); it++)
	{
		if (it->second != NULL)
		{
			it->second.Free();
		}
	}
	m_FatoryMap.clear();
}

void CMessageFactoryManager::RegisterFactory(int messId, CSafePtr<CMessageFactory> pFactory)
{
	auto it = m_FatoryMap.find(messId);
	if (it != m_FatoryMap.end())
	{
		m_FatoryMap.insert(std::make_pair(messId, pFactory));
	}
}

CSafePtr<CMessageFactory> CMessageFactoryManager::GetFactory(int messId)
{
	auto it = m_FatoryMap.find(messId);
	if (it != m_FatoryMap.end())
	{
		return it->second;
	}
	return NULL;
}