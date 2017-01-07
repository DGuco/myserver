#ifndef _SHM_H_
#define _SHM_H_

#include "../base/base.h"

#define YEARSEC 31536000

typedef enum
{
	SHM_INIT = 1,
	SHM_RECOVER = 0
} EIMode;

class CSharedMem
{
public:
	CSharedMem();
	CSharedMem(unsigned int nKey, size_t nSize);
	CSharedMem(unsigned int nKey, size_t nSize, int nInitFlag);
	~CSharedMem();


	int Initialize(unsigned int nKey, size_t nSize);
	void SetStamp();
	void* operator new( size_t nSize);
	void  operator delete(void* pMem);
	EIMode GetInitMode();
	void SetInitMode( EIMode emMode );
	void*  CreateSegment( size_t nSize);
	static BYTE* pbCurrentShm;

protected:

private:
	unsigned int	m_nShmKey;
	size_t			m_nShmSize;
	time_t			m_tCreateTime;
	time_t			m_tLastStamp;
	unsigned int	m_nCRC;
	BYTE*			m_pbCurrentSegMent; 

	EIMode			m_InitMode;
};


#endif
