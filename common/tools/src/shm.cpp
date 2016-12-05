#include "../inc/shm.h"
#include <stdio.h>
#include <time.h>
#include "../inc/log.h"

BYTE* CSharedMem::pbCurrentShm = NULL;

EIMode CSharedMem::GetInitMode()
{
	return m_InitMode;
}

void CSharedMem::SetInitMode( EIMode emMode )
{
	m_InitMode = emMode;
}

void* CSharedMem::operator new( size_t nSize )
{
	BYTE* pTemp = NULL;

	if(!pbCurrentShm )
	{
		return (void *)NULL;
	}

	pTemp = pbCurrentShm;
	
	return (void * )pTemp;
}

void CSharedMem::operator delete(void* pMem )
{
}

CSharedMem::CSharedMem()
{
	m_pbCurrentSegMent = pbCurrentShm + sizeof(CSharedMem);
}

CSharedMem::CSharedMem(unsigned int nKey, size_t nSize )
{
	time_t tNow;
	size_t nTempInt = 0;

	m_pbCurrentSegMent = pbCurrentShm + sizeof(CSharedMem);

	nTempInt = (size_t)m_nShmKey ^ (size_t)m_nShmSize ^ (size_t)m_tCreateTime ^ (size_t)m_tLastStamp ^ (size_t)m_nCRC;

	if( nTempInt )
	{
		Initialize(nKey, nSize);
		return;
	}
	if(!(nKey == m_nShmKey && nSize == (size_t)m_nShmSize ) )
	{
		Initialize(nKey, nSize);
		return;
	}
	
	time( &tNow);

	if(!((tNow - m_tCreateTime ) > 0 && (tNow - m_tCreateTime ) < 10*31536000 ) )
	{
		Initialize(nKey, nSize);
		return;
	}
	
	if(tNow - m_tLastStamp > 3600 )
	{
		Initialize(nKey, nSize);
		return;
	}

	SetStamp();

	m_InitMode = SHM_RECOVER;
}

CSharedMem::CSharedMem(unsigned int nKey, size_t nSize, int nInitFlag )
{	
	if( nInitFlag )
	{
		m_pbCurrentSegMent = pbCurrentShm + sizeof(CSharedMem);
		Initialize(nKey, nSize);
	}
	else
	{
		m_pbCurrentSegMent = pbCurrentShm + sizeof(CSharedMem);
		m_InitMode = SHM_RECOVER;
		
		//CSharedMem::CSharedMem(nKey, nSize);
	}
}

CSharedMem::~CSharedMem()
{
}

int CSharedMem::Initialize(unsigned int nKey, size_t nSize )
{
	time_t tNow;

	m_InitMode = SHM_INIT;

	m_nShmKey = nKey;
	m_nShmSize = nSize;

	time( &tNow);
	m_tCreateTime = tNow;
	m_tLastStamp = tNow;

	m_nCRC = (size_t)m_nShmKey ^ (size_t)m_nShmSize ^ (size_t)m_tCreateTime ^ (size_t)m_tLastStamp;

	return 0;
}

void* CSharedMem::CreateSegment( size_t nSize )
{
	size_t nTempUsedLength = 0;
	BYTE* pTemp;

	if(nSize <= 0 )
	{
		return NULL;
	}

	nTempUsedLength = (size_t)(m_pbCurrentSegMent - (BYTE * )this);
	if(m_nShmSize - nTempUsedLength < nSize )
	{
		LOG_ERROR( "default","in CSharedMem::CreateSegment, create segment error, too small size, shm size: %lld, used: %lld, nSize: %lld", m_nShmSize, nTempUsedLength, nSize);
		return NULL;
	}

	pTemp = m_pbCurrentSegMent;
	m_pbCurrentSegMent += nSize;

	return ( void* ) pTemp;
}

void CSharedMem::SetStamp()
{
	time_t tNow;

	time( &tNow);

	m_tLastStamp = tNow;
	m_nCRC = (size_t)m_nShmKey ^ (size_t)m_nShmSize ^ (size_t)m_tCreateTime ^ (size_t)m_tLastStamp;
}


