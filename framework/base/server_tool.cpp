#include "server_tool.h"
#include <string.h>

// ********************************************************************** //
// 双向链表头尾信息
// ********************************************************************** //
bool CDoubleLinkerInfo::operator == (const CDoubleLinkerInfo& __x) const
{
	if ((__x.mpHead == mpHead) && (__x.mpEnd == mpEnd) && (__x.miCnt == miCnt))
	{
		return true;
	}

	return false;
}


bool CDoubleLinkerInfo::operator != (const CDoubleLinkerInfo& __x) const
{
	return !(*this == __x);
}


CDoubleLinkerInfo& CDoubleLinkerInfo::operator = (const CDoubleLinkerInfo& __x)
{
	mpHead = __x.mpHead;
	mpEnd = __x.mpEnd;
	miCnt = __x.miCnt;

	return (*this);
}


// 将结点插入链表
void CDoubleLinkerInfo::insert(CDoubleLinker* pNode)
{
	if (pNode == NULL)
	{
		return;
	}

	// 没有Next和Prev的节点才能插入链表(无效节点),防止重复插入
	if (pNode->GetNext() != NULL || pNode->GetPrev() != NULL)
	{
		return;
	}

	if (mpEnd != NULL)
	{
		// 有数据，则直接将数据插入尾部，并设为尾指针
		mpEnd->SetNext(pNode);
		pNode->SetPrev(mpEnd);
		pNode->SetNext(NULL);
		SetEnd(pNode);
	}
	else
	{
		// 尾指针没数据，那么头指针也肯定没数据
		SetHead(pNode);
		SetEnd(pNode);
		pNode->Clear();
	}
	miCnt++;
}

// 从链表中删除该结点
// 这个函数应该是不安全的，但为了效率，所以这样处理
// 如果该node不是这个链表中的元素，这里也会处理，当该元素是其他链表的中间元素，则不会出任何问题
// 如果该node不是其他链表的中间元素，那么那个链表将被破坏，而且下面会崩溃，因为prev或next其中一个肯定为空
// 所以在外层使用的是有一定得注意
int CDoubleLinkerInfo::erase(CDoubleLinker* pNode)
{
	int iRet = -1;

	if (pNode == NULL)
	{
		return iRet;
	}

	if (pNode == mpHead && pNode == mpEnd)
	{
		// 只有一个元素
		Clear();
		iRet = 0;
	}
	else if (pNode == mpHead)
	{
		// 头指针
		if (pNode->GetNext() == NULL) return -1;
		pNode->GetNext()->SetPrev(NULL);
		SetHead(pNode->GetNext());
		iRet = 1;
	}
	else if (pNode == mpEnd)
	{
		// 尾指针
		if (pNode->GetPrev() == NULL) return -1;
		pNode->GetPrev()->SetNext(NULL);
		SetEnd(pNode->GetPrev());
		iRet = 2;
	}
	else
	{
		// 有Next和Prev的节点才能从链表中删除(有效节点)
		if (pNode->GetNext() == NULL || pNode->GetPrev() == NULL) return -1;
		// 在链表中间
		pNode->GetPrev()->SetNext(pNode->GetNext());
		pNode->GetNext()->SetPrev(pNode->GetPrev());
		iRet = 3;
	}

	if (miCnt > 0)
		miCnt--;

	pNode->Clear();

	return iRet;
}




















