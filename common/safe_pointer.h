/*****************************************************************
* FileName:safe_pointer.h
* Summary :
* Date	  :2023-8-4
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __SAFE_POINTER_H__
#define __SAFE_POINTER_H__

#if defined(__x86_64__) || defined(_WIN64)
#define SPO_MAGIC_NUM       0X8000000000000000
#define SPO_MAGIC_NUM_H     0X5555555555555555
#define SPO_MAGIC_NUM_L     0XAAAAAAAAAAAAAAAA
#define SPO_FLAG_H          (0XFFFFFFFFFFFFFFFF & SPO_MAGIC_NUM_L)
#define SPO_FLAG_L          (0XFFFFFFFFFFFFFFFF & SPO_MAGIC_NUM_H)
#define SPO_DATA_TYPE       unsigned long long
#define SPO_ERROR_MSG       "SafePointer<Tp> Is not valid,FLAGH = 0X%lx,FLAGL = 0X%lx,pointer = %p\n"
#elif defined(__i386__) || defined(_WIN32)
#define SPO_MAGIC_NUM       0X80000000
#define SPO_MAGIC_NUM_H     0X55555555
#define SPO_MAGIC_NUM_L     0XAAAAAAAA
#define SPO_FLAG_H          (0XFFFFFFFF & SPO_MAGIC_NUM_L)
#define SPO_FLAG_L          (0XFFFFFFFF & SPO_MAGIC_NUM_H)
#define SPO_DATA_TYPE       unsigned long long
#define SPO_ERROR_MSG       "SafePointer<Tp> Is not valid,FLAGH = 0X%llx,FLAGL = 0X%llx,pointer = %p\n"

#endif

#include <stdexcept>
#include "base.h"

/**
 * 仅仅做空指针和bad指针的访问检测，不负责管理指针的内存释放
 */
template<typename Tp>
class CSafePtr
{
public:
	CSafePtr() : nDataH(SPO_FLAG_H), nDataL(SPO_FLAG_L) {}
	CSafePtr(Tp* pointer,bool autofree = false)
	{
		nDataH = SPO_FLAG_H;
		nDataL = SPO_FLAG_L;
		SPO_DATA_TYPE pvalue = (SPO_DATA_TYPE)pointer;
		for (size_t index = 0; index < sizeof(void*) * 8;)
		{
			nDataL = nDataL | (pvalue & (SPO_MAGIC_NUM >> index)/*取出要保存的位,其他位置为*/);//保有当前位的值到相应位愿
			index++;
			nDataH = nDataH | (pvalue & (SPO_MAGIC_NUM >> index)/*取出要保存的位,其他位置为*/);//保有当前位的值到相应位愿
			index++;
		}
	}

	CSafePtr<Tp>& operator=(const Tp* pOhter)
	{
		Reset(pOhter);
		return *this;
	}

	CSafePtr<Tp>& operator=(const CSafePtr<Tp> pOhter)
	{
		nDataH = pOhter.nDataH;
		nDataL = pOhter.nDataL;
		return *this;
	}
	bool IsPointerBad()
	{
		char eMsg[256] = { 0 };
		printf("SafePointer<Tp> FLAGH = 0X%lx,FLAGL = 0X%lx\n", static_cast<unsigned long>(GetFlagH()), static_cast<unsigned long>(GetFlagL()));
		return  (GetFlagH() != SPO_FLAG_H || GetFlagL() != SPO_FLAG_L);
	}

	void Reset()
	{
		nDataH = SPO_FLAG_H;
		nDataL = SPO_FLAG_L;
	}

	void Reset(const Tp* pointer)
	{
		nDataH = SPO_FLAG_H;
		nDataL = SPO_FLAG_L;
		SPO_DATA_TYPE pvalue = (SPO_DATA_TYPE)pointer;
		for (size_t index = 0; index < sizeof(void*) * 8;)
		{
			nDataL = nDataL | (pvalue & (SPO_MAGIC_NUM >> index)/*取出要保存的位,其他位置为*/);//保有当前位的值到相应位愿
			index++;
			nDataH = nDataH | (pvalue & (SPO_MAGIC_NUM >> index)/*取出要保存的位,其他位置为*/);//保有当前位的值到相应位愿
			index++;
		}
	}

	//获取高位fag
	SPO_DATA_TYPE GetFlagH() const { return nDataH & SPO_MAGIC_NUM_L; }
	SPO_DATA_TYPE GetFlagL() const { return nDataL & SPO_MAGIC_NUM_H; }
	inline Tp& operator*() const { return *GetThrow(); }
	inline Tp* operator->() const { return GetThrow(); }
	
	bool operator==(const Tp* pOhter)
	{
		return Get() == pOhter;
	}

	bool operator!=(const Tp* pOhter)
	{
		return Get() != pOhter;
	}

	bool operator==(const CSafePtr<Tp> pOhter)
	{
		return this->nDataH == pOhter->nDataH && this->nDataL == pOhter->nDataL;
	}

	bool operator != (const CSafePtr<Tp> pOhter)
	{
		return this->nDataH != pOhter->nDataH && this->nDataL != pOhter->nDataL;
	}

	Tp* operator()()
	{
		return Get();
	}

	template<typename NewTp>
	CSafePtr<NewTp> DynamicCastTo()
	{
		Tp* pPointer = GetThrow();
		if (pPointer != NULL)
		{
			return CSafePtr<NewTp>(dynamic_cast<NewTp*>(pPointer));
		}
		else
		{
			return CSafePtr<NewTp>();
		}
	}

	template<typename NewTp>
	void StaticCastTo()
	{
		Tp* pPointer = GetThrow();
		if (pPointer != NULL)
		{
			return CSafePtr<NewTp>(static_cast<NewTp>(pPointer))
		}
		else
		{
			return CSafePtr<NewTp>();
		}
	}

	Tp* Get() const
	{
		return GetThrow();
	}

	void Free()
	{
		Tp* pPointer = GetThrow();
		DELETE(pPointer);
		nDataH = SPO_FLAG_H;
		nDataL = SPO_FLAG_L;
	}
private:
	Tp* GetThrow() const
	{
		Tp* pPoint = (Tp*)((nDataL & SPO_MAGIC_NUM_L) | (nDataH & SPO_MAGIC_NUM_H));
		if (GetFlagH() != SPO_FLAG_H || GetFlagL() != SPO_FLAG_L)
		{
			char eMsg[256] = { 0 };
			sprintf_s(eMsg,256,SPO_ERROR_MSG, (unsigned long)(GetFlagH()), (unsigned long)GetFlagL(), (void*)pPoint);
			throw std::runtime_error(eMsg);
		}

		if (pPoint == NULL)
		{
			char eMsg[256] = { 0 };
			sprintf_s(eMsg,256, SPO_ERROR_MSG, (unsigned long)GetFlagH(), (unsigned long)GetFlagL(), (void*)pPoint);
			throw std::runtime_error(eMsg);
		}
		return pPoint;
	}

private:
	SPO_DATA_TYPE nDataH;
	SPO_DATA_TYPE nDataL;
};

#endif //__SAFE_POINTER_H__
