//
// Created by DGuco on 17-7-26.
// ��������˫������ʵ�ֶ����ڴ�أ�ע:ʹ�ø��ڴ����������޲εĹ��캯����
//

#ifndef SERVER_MEMPOOL_H
#define SERVER_MEMPOOL_H

#include "base.h"
#include <clocale>
template<class Type>
class CMemoryPool
{
public:
	CMemoryPool(void)
		: m_pElementsSetList(NULL), m_pUnusedElementsList(NULL)
	{}
	virtual ~CMemoryPool(void)
	{}
private:
	//�ڵ����ݽṹ
	struct TagElement
	{
		Type Element;
		TagElement *pNext;
		TagElement *pBefore;
		bool bAlloc;
	};

	//��������
	struct TagElementsSet
	{
		//��������
		TagElement *aElementsSet;
		//�����ʼ����Сʹ���꣬��������һ��ͬ�ȴ�С��������ָ�������
		TagElementsSet *pNext;
	};

	//���������
	TagElementsSet *m_pElementsSetList;
	//ʹ������
	int m_nNumOfAlloc;
	//��ǰ���õĽڵ�
	TagElement *m_pUnusedElementsList;
	//һ�����������������
	int m_nNumOfElements;
	//��������
	int m_nNumOfElementsSet;

public:
	//����ָ���������ڴ��
	virtual bool Create(uint nNumOfElements)
	{
		m_nNumOfElements = nNumOfElements;
		m_nNumOfElementsSet = 1;

		m_pElementsSetList = new TagElementsSet;
		m_pElementsSetList->pNext = NULL;
		m_pElementsSetList->aElementsSet = new TagElement[m_nNumOfElements];

		for (int i = 0; i < m_nNumOfElements; i++) {
			if (i > 0) {
				m_pElementsSetList->aElementsSet[i].pBefore = &m_pElementsSetList->aElementsSet[i - 1];
				m_pElementsSetList->aElementsSet[i - 1].pNext = &m_pElementsSetList->aElementsSet[i];
			}
			m_pElementsSetList->aElementsSet[i].bAlloc = FALSE;
		}
		m_pElementsSetList->aElementsSet[0].pBefore = NULL;
		m_pElementsSetList->aElementsSet[m_nNumOfElements - 1].pNext = NULL;

		m_pUnusedElementsList = m_pElementsSetList->aElementsSet;
		m_nNumOfAlloc = 0;
		return TRUE;
	}

	//�����ڴ�
	virtual void Destroy()
	{
		while (m_pElementsSetList) {
			if (m_pElementsSetList->aElementsSet) {
				delete[] m_pElementsSetList->aElementsSet;
				m_pElementsSetList->aElementsSet = NULL;
			}
			TagElementsSet *pOld = m_pElementsSetList;
			m_pElementsSetList = m_pElementsSetList->pNext;

			delete pOld;
		}
	}

	virtual Type *Alloc()
	{
		bool bTestAlloc = FALSE;
		//�����ǰ������ʹ���꣬�����µ�һ��ͬ�ȴ�С������
		if (m_pUnusedElementsList == NULL) {
			bTestAlloc = TRUE;
			TagElementsSet *pSet = new TagElementsSet;
			pSet->pNext = m_pElementsSetList;
			pSet->aElementsSet = new TagElement[m_nNumOfElements];

			for (int i = 0; i < m_nNumOfElements; i++) {
				if (i > 0) {
					pSet->aElementsSet[i].pBefore = &pSet->aElementsSet[i - 1];
					pSet->aElementsSet[i - 1].pNext = &pSet->aElementsSet[i];
				}
				pSet->aElementsSet[i].bAlloc = FALSE;
			}
			pSet->aElementsSet[0].pBefore = NULL;
			pSet->aElementsSet[m_nNumOfElements - 1].pNext = NULL;

			m_pUnusedElementsList = pSet->aElementsSet;

			m_pElementsSetList = pSet;

			m_nNumOfElementsSet++;
		}

		TagElement *pTagElement;

		pTagElement = m_pUnusedElementsList;
		m_pUnusedElementsList = m_pUnusedElementsList->pNext;
		if (m_pUnusedElementsList) {
			m_pUnusedElementsList->pBefore = NULL;

		}

		pTagElement->bAlloc = TRUE;

		m_nNumOfAlloc++;

		return (Type *) &(pTagElement->Element);
	}

	virtual void Free(Type *pElement)
	{
		TagElement *pTagElement = (TagElement *) pElement;
		pTagElement->pNext = m_pUnusedElementsList;
		pTagElement->pBefore = NULL;
		if (m_pUnusedElementsList)
			m_pUnusedElementsList->pBefore = pTagElement;
		m_pUnusedElementsList = pTagElement;
		m_nNumOfAlloc--;
		pTagElement->bAlloc = FALSE;
	}

	int GetAllocatedSize()
	{
		return m_nNumOfAlloc;
	}

	int GetPoolSize()
	{
		return m_nNumOfElements * m_nNumOfElementsSet;
	}

	bool IsCreated()
	{
		return m_pElementsSetList != NULL;
	}

};// class CMemoryPool

#endif //SERVER_MEMPOOL_H
