#include "codequeue.h"
#include <stdio.h>
#include <string.h>

CSharedMem* CCodeQueue::pCurrentShm = NULL;

/**
  *函数名          : CountQueueSize
  *功能描述        : PIPE_SIZE + CountQueueSize
  *参数			  : iBufSize:PIPE_SIZE 共享内存管道长度
  *返回值         ： PIPE_SIZE + CountQueueSize
**/
size_t CCodeQueue::CountQueueSize(int iBufSize)
{
	size_t iTempSize = 0;

	iTempSize += sizeof(CCodeQueue);
	if( iBufSize > 0 )
	{
		iTempSize += iBufSize;
	}

	return iTempSize;
}

/**
  *函数名          : CCodeQueue重在new运算符
  *功能描述        : 改变CCodeQueue的new操作，在共享内存空间上生成内存空间生生成对象，
  *                 用来进程间通信，此时codequeue对象指向了一块 sizeof（CCodeQueue）
  *                 + PIPE_SIZE大小的内存快，调用new之后此时只有sizeof（CCodeQueue）
  *                 大小的内存快是有效的
  *参数           ： sizeof（CCodeQueue）
**/
void* CCodeQueue::operator new(size_t nSize)
{
	BYTE* pTemp;

	if( !pCurrentShm )
	{
		return (void *)NULL;
	}

	pTemp = (BYTE *)(pCurrentShm->CreateSegment(nSize));
	
	return (void *)pTemp;
}

void CCodeQueue::operator delete(void *pBase)
{
}

CCodeQueue::CCodeQueue()
{
	m_stQueueHead.m_iReadIndex = 0;
	m_stQueueHead.m_iWriteIndex = 0;
	m_stQueueHead.m_iSize = 0;
	m_stQueueHead.m_iCodeBufOffSet = -1;
	m_stQueueHead.m_iLockIdx = -1;
}

/**
  *函数名          : CCodeQueue
  *功能描述        : 调用new之后此时只有sizeof（CCodeQueue），构造函数通过Initialize
  *                 初始化剩下的nTotalSize大小的内存快
  *参数           ： 共享内存管道大小PIPE_SIZE
**/
CCodeQueue::CCodeQueue(int nTotalSize, int iLockIdx /* = -1  */)
{
	if( !pCurrentShm )
	{
		m_stQueueHead.m_iReadIndex = 0;
		m_stQueueHead.m_iWriteIndex = 0;
		m_stQueueHead.m_iSize = 0;
		m_stQueueHead.m_iCodeBufOffSet = -1;
		m_stQueueHead.m_iLockIdx = -1;
		return;
	}

	if( pCurrentShm->GetInitMode() == SHM_INIT )
	{
		m_stQueueHead.m_iLockIdx = iLockIdx;
		Initialize( nTotalSize );
	}
	else
	{
		Resume( nTotalSize );
	}
}

CCodeQueue::~CCodeQueue()
{
}

/**
  *函数名          : Initialize
  *功能描述        : 初始化CCodeQueue的共享内存管道内存区
  * 参数          ： 共享内存管道大小PIPE_SIZE
**/
int CCodeQueue::Initialize( int nTotalSize )
{
	BYTE *pbyCodeBuf;
	
	m_stQueueHead.m_iSize = nTotalSize;
	m_stQueueHead.m_iReadIndex = 0;
	m_stQueueHead.m_iWriteIndex = 0;

	pbyCodeBuf = GetPipeAddr();

	if( !pbyCodeBuf )
	{
		m_stQueueHead.m_iCodeBufOffSet = -1;
		return -1;
	}

    //计算共享内存管道在CCodeQueue对象地址中的地址偏移
	m_stQueueHead.m_iCodeBufOffSet = (int)((BYTE *)pbyCodeBuf - (BYTE *)this); 

	return 0;
}

int CCodeQueue::Resume(int nTotalSize)
{
	if( !pCurrentShm )
	{
		return -1;
	}

	pCurrentShm->CreateSegment((size_t)nTotalSize);

	return 0;
}

/**
  *函数名          : GetCriticalData
  *功能描述        : 获取共享内存管道内存区的读写地址索引
**/
void CCodeQueue::GetCriticalData(int *piBeginIdx, int *piEndIdx)
{
	if( piBeginIdx )
	{
		*piBeginIdx = m_stQueueHead.m_iReadIndex;
	}
	if( piEndIdx )
	{
		*piEndIdx = m_stQueueHead.m_iWriteIndex;
	}
}

/**
  *函数名          : GetCriticalData
  *功能描述        : 设置共享内存管道内存区的读写地址索引
**/
int CCodeQueue::SetCriticalData(int iReadIndex, int iWriteIndex)
{
    m_stQueueHead.m_iReadIndex = iReadIndex;
    m_stQueueHead.m_iWriteIndex = iWriteIndex;
	return 0;
}

/**
  *函数名          : IsQueueFull
  *功能描述        : 共享内存管道是否已满
**/
int CCodeQueue::IsQueueFull()
{
	int nTempMaxLength = 0;
	int nTempRead = 0;
	int nTempWrite = 0;

	
	GetCriticalData( &nTempRead, &nTempWrite );

	//最大长度
	nTempMaxLength = nTempWrite - nTempRead;

	// 重要：最大长度应该减去预留部分长度，保证首尾不会相接
	nTempMaxLength -= QUEUERESERVELENGTH;

	if( nTempMaxLength > 0 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
  *函数名          : AppendOneCode
  *功能描述        : 写共享内存管道
  * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CCodeQueue::AppendOneCode(const BYTE *pInCode, int sInLength)
{
	int nTempMaxLength = 0;
	int nTempRt = -1;
	int nTempRead = 0;
	int nTempWrite = 0;
    int nRequireLen = 0;
	//char pcTempBuf[8192];
	BYTE *pbyCodeBuf;
	
	BYTE *pTempSrc = NULL;
	BYTE *pTempDst = NULL;
	unsigned int i;

	if( !pInCode || sInLength <= 0 )
	{
		return -1;
	}
	if( m_stQueueHead.m_iCodeBufOffSet <= 0)
	{
		return -1;
	}
	//获取共享内存管道起始地址
	pbyCodeBuf = GetPipeAddr();

	// 首先判断是否队列已满
	if( IsQueueFull() )
	{
		return -2;
	}

	GetCriticalData(&nTempRead, &nTempWrite);

	if( nTempRead < 0 || nTempRead >= m_stQueueHead.m_iSize
		|| nTempWrite < 0 || nTempWrite >= m_stQueueHead.m_iSize ||
            nTempRead > nTempWrite)
	{
        //清楚共享内存管道数据区
		CleanQueue();
		return -3;
	}

    //获得剩余空间大小
    nTempMaxLength = m_stQueueHead.m_iSize - (nTempWrite - nTempRead);

	// 重要：最大长度应该减去预留部分长度8，保证首尾不会相接
	nTempMaxLength -= QUEUERESERVELENGTH;

	nTempRt = nTempWrite;

    //存储数据所需长度
    nRequireLen = sInLength + sizeof(short);
    //剩余空间不足
	if( nRequireLen > nTempMaxLength || sInLength > 0xFFFF)
	{
		return -2;
	}


	unsigned short usInLength = (unsigned short) sInLength;
    //内存尾部存储不下
    if ((m_stQueueHead.m_iSize - nTempWrite) < nRequireLen)
    {
        //避免分片
        DiscardReadBytes();
    }
    //重新获取读写索引
    GetCriticalData(&nTempRead, &nTempWrite);

	pTempDst = &pbyCodeBuf[0];
	pTempSrc = (BYTE *)&usInLength;

	//把数据长度写在数据头部
	for( i = 0; i < sizeof(usInLength); i++ )
	{
		pTempDst[nTempWrite] = pTempSrc[i];  // 拷贝 Code 的长度
        nTempWrite = (nTempWrite+1);
	}

    memcpy((void *)&pbyCodeBuf[nTempWrite], (const void *)pInCode, (size_t)sInLength );

    nTempWrite = (nTempWrite + sInLength);

	SetCriticalData(nTempRead, nTempWrite );  // 重新设置 codequeue 结尾的 idx

	return nTempRt;
}

/**
  *函数名          : PeekHeadCode
  *功能描述        : 查看共享内存管道（不改变读写索引）
  * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CCodeQueue::PeekHeadCode(BYTE *pOutCode, int *psOutLength)
{
	int nTempMaxLength = 0;
	int nTempRet = -1;
	int nTempRead = 0;
	int nTempWrite = 0;
	BYTE *pTempSrc;
	BYTE *pTempDst;
	unsigned int i;
	BYTE *pbyCodeBuf;

	if( !pOutCode || !psOutLength )
	{
		return -1;
	}
	if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_iSize <= 0 )
	{
		return -1;
	}

	//获取共享内存管道的地址
	pbyCodeBuf = GetPipeAddr();

    //获取读写索引
	GetCriticalData(&nTempRead, &nTempWrite);

	nTempRet = nTempRead;

	if(IsQueueEmpty())
	{
		*psOutLength = 0;
        CleanQueue();
		return nTempRet;
	}

    //获取数据大小
    nTempMaxLength = nTempWrite - nTempRead;

	if( nTempMaxLength < (int)sizeof(short) )
	{
		*psOutLength = 0;
		return -3;
	}

	unsigned short usOutLength = 0;

	//数据目的地址
	pTempDst = (BYTE *)&usOutLength;
	//数据源地址
	pTempSrc = (BYTE *)&pbyCodeBuf[0];
	//读取数据长度
	for( i = 0; i < sizeof(short); i++ )
	{
		pTempDst[i] = pTempSrc[nTempRead];
		nTempRead = (nTempRead+1);
	}

	*psOutLength = usOutLength;

	//数据长度不合法
	if( (*psOutLength) > (int)(nTempMaxLength - sizeof(short)) )
	{
		*psOutLength = 0;
		return -3;
	}

	pTempDst = (BYTE *)&pOutCode[0];

    //拷贝数据
	memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));

	return nTempRet;
}

/**
  *函数名          : GetHeadCode
  *功能描述        : 读取共享内存管道（改变读写索引）
  * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CCodeQueue::GetHeadCode(BYTE *pOutCode, int *psOutLength)
{
    int nTempMaxLength = 0;
    int nTempRet = -1;
    int nTempRead = 0;
    int nTempWrite = 0;
    BYTE *pTempSrc;
    BYTE *pTempDst;
    unsigned int i;
    BYTE *pbyCodeBuf;

    if( !pOutCode || !psOutLength )
    {
        return -1;
    }
    if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_iSize <= 0 )
    {
        return -1;
    }

    //获取共享内存管道的地址
    pbyCodeBuf = GetPipeAddr();

    //获取读写索引
    GetCriticalData(&nTempRead, &nTempWrite);

    nTempRet = nTempRead;

    if(IsQueueEmpty())
    {
        *psOutLength = 0;
        CleanQueue();
        return nTempRet;
    }

    //获取数据大小
    nTempMaxLength = nTempWrite - nTempRead;

    if( nTempMaxLength < (int)sizeof(short) )
    {
        *psOutLength = 0;
        return -3;
    }

    unsigned short usOutLength = 0;

    //数据目的地址
    pTempDst = (BYTE *)&usOutLength;
    //数据源地址
    pTempSrc = (BYTE *)&pbyCodeBuf[0];
    //读取数据长度
    for( i = 0; i < sizeof(short); i++ )
    {
        pTempDst[i] = pTempSrc[nTempRead];
        nTempRead = (nTempRead+1);
    }

    *psOutLength = usOutLength;

    //数据长度不合法
    if( (*psOutLength) > (int)(nTempMaxLength - sizeof(short)) )
    {
        *psOutLength = 0;
        return -3;
    }

    pTempDst = (BYTE *)&pOutCode[0];

    //拷贝数据
    memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));
    nTempRead = nTempRead + (*psOutLength);
    SetCriticalData(nTempRead,nTempWrite);
    return nTempRet;
}

//This function just change the begin idx of code queue
int CCodeQueue::DeleteHeadCode()
{
	short sTempShort = 0;
	int nTempMaxLength = 0;
	int nTempRet = -1;
	int nTempBegin = -1;
	int nTempEnd = -1;

	BYTE *pTempSrc;
	BYTE *pTempDst;
	BYTE *pbyCodeBuf;

	if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_nSize <= 0 )
	{
		return -1;
	}

	pbyCodeBuf = GetPipeAddr();
	
	if( GetCriticalData(&nTempBegin, &nTempEnd) )
	{
		return -1;
	}
	nTempRet = nTempBegin;

	if( nTempBegin == nTempEnd )
	{
		return nTempRet;
	}

	if( nTempBegin < nTempEnd )
	{
		nTempMaxLength = nTempEnd - nTempBegin;
	}
	else
	{
		nTempMaxLength = m_stQueueHead.m_nSize - nTempBegin + nTempEnd;
	}
	
	if( nTempMaxLength < ( int )sizeof(short) )
	{
		nTempBegin = nTempEnd;
		SetCriticalData(nTempBegin, -1);
		return -3;
	}

	pTempDst = (BYTE *)&sTempShort;
	pTempSrc = (BYTE *)&pbyCodeBuf[0];
	for( unsigned int i = 0; i < sizeof(short); i++ )
	{
		pTempDst[i] = pTempSrc[nTempBegin];
		nTempBegin = (nTempBegin+1) % m_stQueueHead.m_nSize; 
	}

	if( sTempShort > (int)(nTempMaxLength - sizeof(short)) || sTempShort < 0 )
	{
		nTempBegin = nTempEnd;
		SetCriticalData(nTempBegin, -1);
		return -3;
	}

	nTempBegin = (nTempBegin + sTempShort) % m_stQueueHead.m_nSize;

	SetCriticalData(nTempBegin, -1);
	
	return nTempBegin;
}

int CCodeQueue::GetOneCode(int iCodeOffset, int nCodeLength, BYTE *pOutCode, int *psOutLength)
{
	short nTempShort = 0;
	int iTempMaxLength = 0;
	int iTempBegin;
	int iTempEnd;
	int iTempIdx;
	BYTE *pTempSrc;
	BYTE *pTempDst;
	BYTE *pbyCodeBuf;

	if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_nSize <= 0 )
	{
		return -1;
	}

	pbyCodeBuf = (BYTE *)((BYTE *)this + m_stQueueHead.m_iCodeBufOffSet);

	if( !pOutCode || !psOutLength )
	{
		//LOG("In GetOneCode, invalid input paraments.\n");
		return -1;
	}

	if( iCodeOffset < 0 || iCodeOffset >= m_stQueueHead.m_nSize)
	{
		/*LOG("In GetOneCode, invalid code offset %d.\n", iCodeOffset);*/
		return -1;
	}
	if( nCodeLength < 0 || nCodeLength >= m_stQueueHead.m_nSize )
	{
		/*LOG("In GetOneCode, invalid code length %d.\n", nCodeLength);*/
		return -1;
	}

	if( GetCriticalData(&iTempBegin, &iTempEnd) )
	{
		return -1;
	}
	
	if( iTempBegin == iTempEnd )
	{
		*psOutLength = 0;
		return 0;
	}

	if( iTempBegin == iCodeOffset )
	{
		return GetHeadCode(pOutCode, psOutLength);
	}

	/*LOG("Warning: Get code is not the first one, there might be sth wrong.\n");*/

	if( iCodeOffset < iTempBegin || iCodeOffset >= iTempEnd )
	{
		/*LOG("In GetOneCode, code offset out of range.\n");*/
		*psOutLength = 0;
		return -1;
	}
	
	if( iTempBegin < iTempEnd )
	{
		iTempMaxLength = iTempEnd - iTempBegin;		
	}
	else
	{
		iTempMaxLength = m_stQueueHead.m_nSize - iTempBegin + iTempEnd;
	}

	if( iTempMaxLength < ( int )sizeof(short) )
	{
		*psOutLength = 0;
		iTempBegin = iTempEnd;
		SetCriticalData(iTempBegin, -1);
		return -3;
	}

	pTempDst = (BYTE *)&nTempShort;
	pTempSrc = (BYTE *)&pbyCodeBuf[0];
	iTempIdx = iCodeOffset;
	for( unsigned int i = 0; i < sizeof(short); i++ )
	{
		pTempDst[i] = pTempSrc[iTempIdx];
		iTempIdx = (iTempIdx+1) % m_stQueueHead.m_nSize; 
	}

	if( nTempShort > (int)(iTempMaxLength - sizeof(short)) || nTempShort < 0 || nTempShort != nCodeLength )
	{
		/*LOG("Can't get code, code length not matched.\n");*/
		*psOutLength = 0;
		return -2;
	}

	SetCriticalData(iCodeOffset, -1);
	
	return GetHeadCode( pOutCode, psOutLength );
}

/**
  *函数名          : GetPipeAddr
  *功能描述        : 获取共享内存管道的地址
**/
BYTE* CCodeQueue::GetPipeAddr()
{
	return (BYTE *)((BYTE *)this + m_stQueueHead.m_iCodeBufOffSet);
}

int CCodeQueue::IsQueueEmpty()
{
	int iTempRead;
	int iTempWrite;

	GetCriticalData(&iTempRead, &iTempWrite);

	if( iTempRead == iTempWrite)
	{
		return 1;
	}

	return 0;
}

/**
  *函数名          : DiscardReadBytes
  *功能描述        : 避免写数据发上分片（数据在内存管道中前面一部分后面一部分）,
  * 				将待读取数据移动至共享内存管道的头部
**/
void CCodeQueue::DiscardReadBytes()
{
	int iTmpReadIndex = -1;
	int iTmpWriteIndex = -1;
	GetCriticalData(&iTmpReadIndex,&iTmpWriteIndex);
	//数据已经在共享内存管道的头部
	if (iTmpReadIndex == 0)
	{
		return;
	}

    //索引错误
    if (iTmpReadIndex <= iTmpWriteIndex)
    {
        return;
    }

	//获取共享内存管道起始地址
	BYTE* npbyCodeBuf = GetPipeAddr();
    //将数据拷贝到共享内存管道的头部
	memmove((void*)(npbyCodeBuf),(void *)(npbyCodeBuf + iTmpReadIndex),iTmpWriteIndex - iTmpReadIndex);
    //设置读写索引
    SetCriticalData(0,iTmpWriteIndex - iTmpReadIndex);
}


void CCodeQueue::GetCriticalData(int& iBegin, int& iEnd, int& iLeft)
{
	iBegin = m_stQueueHead.m_nBegin;
	iEnd = m_stQueueHead.m_nEnd;

	int iTempMaxLength = 0;
	
	if( iBegin == iEnd )
	{
		iTempMaxLength = m_stQueueHead.m_nSize;
	}
	else if( iBegin > iEnd )
	{
		iTempMaxLength = iBegin - iEnd;		
	}
	else
	{
		iTempMaxLength = (m_stQueueHead.m_nSize - iEnd) + iBegin;
	}

	// 重要：最大长度应该减去预留部分长度，保证首尾不会相接
	iTempMaxLength -= QUEUERESERVELENGTH;

	iLeft = iTempMaxLength;
	
}

int CCodeQueue::DumpToFile(const char *szFileName)
{
	if( !szFileName || !m_pbyCodeBuffer )
	{
		return -1;
	}

	FILE *fpDumpFile = fopen(szFileName, "w");
	int iPageSize = 4096, iPageCount = 0, i;
	BYTE *pPage = m_pbyCodeBuffer;

	if( !fpDumpFile )
	{
		return -1;
	}

	fwrite((const void *)&m_stQueueHead, sizeof(m_stQueueHead), 1, fpDumpFile);
	iPageCount = m_stQueueHead.m_nSize/iPageSize;
	for( i = 0; i < iPageCount; i++ )
	{
		fwrite((const void *)pPage, iPageSize, 1, fpDumpFile);
		pPage += iPageSize;
	}
	fwrite((const void *)pPage, m_stQueueHead.m_nSize - iPageSize*iPageCount, 1, fpDumpFile);

	fclose(fpDumpFile);

	return 0;                                                                                                     
}                                                                                                                 

int CCodeQueue::LoadFromFile(const char *szFileName)                                                              
{                                                                                                                 
	if( !szFileName || !m_pbyCodeBuffer )                                                                         
	{                                                                                                             
		return -1;                                                                                                
	}                                                                                                             

	FILE *fpDumpFile = fopen(szFileName, "r");                                                                    
	int iPageSize = 4096, iPageCount = 0, i;                                                                      
	BYTE *pPage = m_pbyCodeBuffer;                                                                                

	if( !fpDumpFile )                                                                                             
	{                                                                                                             
		return -1;                                                                                                
	}                                                                                                             

	fread((void *)&m_stQueueHead, sizeof(m_stQueueHead), 1, fpDumpFile);
	iPageCount = m_stQueueHead.m_nSize/iPageSize;                                                                 
	for( i = 0; i < iPageCount; i++ )                                                                             
	{                                                                                                             
		fread((void *)pPage, iPageSize, 1, fpDumpFile);
		pPage += iPageSize;                                                                                       
	}
	fread((void *)pPage, m_stQueueHead.m_nSize - iPageSize*iPageCount, 1, fpDumpFile);

	fclose(fpDumpFile);                                                                                           

	return 0;   
}

int CCodeQueue::CleanQueue()                                                                                      
{
	m_stQueueHead.m_iReadIndex = 0;
	m_stQueueHead.m_iWriteIndex = 0;

	return 0;
}

