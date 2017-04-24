#include "codequeue.h"
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
  *功能描述        : 改变CCodeQueue的new操作，在共享内存空间上生成对象，
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
	if( iReadIndex >= 0 && iReadIndex < m_stQueueHead.m_iSize )
	{
		m_stQueueHead.m_iReadIndex = iReadIndex;
	}
	if( iWriteIndex >= 0 && iWriteIndex < m_stQueueHead.m_iSize )
	{
		m_stQueueHead.m_iWriteIndex = iWriteIndex;
	}

	return 0;
}

/**
  *函数名          : IsQueueFull
  *功能描述        : 共享内存管道是否已满
**/
int CCodeQueue::IsQueueFull()
{
	int nTempMaxLength = 0;
	int nTempRead = -1;
	int nTempWrite = -1;

	
	GetCriticalData( &nTempRead, &nTempWrite );

	//计算当前可用长度
	if( nTempRead == nTempWrite )
	{
		nTempMaxLength = m_stQueueHead.m_iSize;
	}
	else if( nTempRead > nTempWrite )
	{
		nTempMaxLength = nTempRead - nTempWrite;
	}
	else
	{
		nTempMaxLength = (m_stQueueHead.m_iSize - nTempWrite) + nTempRead;
	}
	/**
	 * 最大长度应该减去预留部分长度，保证首尾不会相接,
     * 以此区分数据头不在共享内存区头部写满数据，和没有数据的情况
	 */
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
  *功能描述        : 写共享内存管道，仅改变wrietIndex，都共享内存仅改变readIndex，
  *                 保证读进程读和写进程写不会造成数据不一致
  * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CCodeQueue::AppendOneCode(const BYTE *pInCode, int sInLength)
{
	int nTempMaxLength = 0;
	int nTempRt = -1;
	int nTempRead;
	int nTempWrite;
	BYTE *pbyCodeBuf;

	BYTE *pTempSrc = NULL;
	BYTE *pTempDst = NULL;
	unsigned int i;

	if( !pInCode || sInLength <= 0 )
	{
		return -1;
	}
    if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_iSize <= 0 )
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
		|| nTempWrite < 0 || nTempWrite >= m_stQueueHead.m_iSize)
	{
        //清楚共享内存管道数据区
        nTempWrite = nTempRead;
		return -3;
	}

    //获得剩余空间大小
    if( nTempRead == nTempWrite )
    {
        nTempMaxLength = m_stQueueHead.m_iSize;  // nTempMaxLength 为剩余空间
    }
    else if( nTempRead > nTempWrite )
    {
        nTempMaxLength = nTempRead - nTempWrite;
    }
    else
    {
        nTempMaxLength = (m_stQueueHead.m_iSize - nTempWrite) + nTempRead;
    }

    /**
     * 最大长度应该减去预留部分长度，保证首尾不会相接,
     * 以此区分数据头不在共享内存区头部写满数据，和没有数据的情况
     */
    nTempMaxLength -= QUEUERESERVELENGTH;
	nTempRt = nTempWrite;

    //剩余空间不足
    if( (int)(sInLength + sizeof(short)) > nTempMaxLength || sInLength > 0xFFFF)
    {
        return -2;
    }

	unsigned short usInLength = (unsigned short) sInLength;

	pTempDst = &pbyCodeBuf[0];
	pTempSrc = (BYTE *)&usInLength;
	//把数据长度写在数据头部
	for( i = 0; i < sizeof(usInLength); i++ )
	{
        // 拷贝 Code 的长度
        pTempDst[nTempWrite] = pTempSrc[i];
        // % 用于防止 Code 结尾的 idx 超出 codequeue
        nTempWrite = (nTempWrite+1) % m_stQueueHead.m_iSize;
	}

    //可用内存在共享内存的中间连续区域
    if( nTempRead > nTempWrite )
    {
        memcpy((void *)&pbyCodeBuf[nTempWrite], (const void *)pInCode, (size_t)sInLength );
    }
    //可用内存在共享内存的两头
    else
    {
        // 需要分段拷贝的情况
        if( (int)sInLength > (m_stQueueHead.m_iSize - nTempWrite) )
        {
            memcpy((void *)&pbyCodeBuf[nTempWrite], (const void *)&pInCode[0], (size_t)(m_stQueueHead.m_iSize - nTempWrite) );
            memcpy((void *)&pbyCodeBuf[0],(const void *)&pInCode[(m_stQueueHead.m_iSize - nTempWrite)],
                   (size_t)(sInLength - (m_stQueueHead.m_iSize - nTempWrite)));
        }
        //内存区尾部够够长不需要分片
        else
        {
            memcpy((void *)&pbyCodeBuf[nTempWrite], (const void *)&pInCode[0], (size_t)sInLength);
        }
    }
    nTempWrite = (nTempWrite + sInLength) % m_stQueueHead.m_iSize;

    // 重新设置 codequeue 结尾的 idx
	SetCriticalData(-1, nTempWrite );
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
	int nTempRead;
	int nTempWrite;
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

    if( nTempRead == nTempWrite )
    {
        *psOutLength = 0;
        return nTempRet;
    }

    //获取数据大小
    if( nTempRead < nTempWrite )
    {
        nTempMaxLength = nTempWrite - nTempRead;
    }
    else
    {
        nTempMaxLength = m_stQueueHead.m_iSize  - (nTempRead - nTempWrite);
    }

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
        nTempRead = (nTempRead+1) % m_stQueueHead.m_iSize;
    }

	*psOutLength = usOutLength;

	//数据长度不合法
	if( (*psOutLength) > (int)(nTempMaxLength - sizeof(short)) || *psOutLength < 0 )
	{
		*psOutLength = 0;
		return -3;
	}

	pTempDst = (BYTE *)&pOutCode[0];

    //数据在内存去中间
    if( nTempRead < nTempWrite )
    {
        memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));
    }
    //数据在内存区两头
    else
    {
        // 如果出现分片，则分段拷贝
        if( m_stQueueHead.m_iSize - nTempRead < *psOutLength)
        {
            memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(m_stQueueHead.m_iSize - nTempRead));
            pTempDst += (m_stQueueHead.m_iSize - nTempRead);
            memcpy((void *)pTempDst, (const void *)&pTempSrc[0], (size_t)(*psOutLength - (m_stQueueHead.m_iSize - nTempRead));
        }
        // 否则，直接拷贝
        else
        {
            memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));
        }
    }
    return nTempRet;
}

/**
  *函数名          : GetHeadCode
  *功能描述        : 读取共享内存管道（改变读写索引）
  *                 注：写共享内存管道，仅改变wrietIndex，读共享内存仅改变readIndex，
  *                 保证读进程读和写进程写不会造成数据不一致
  * Error code: -1 invalid para; -2 not enough; -3 data crashed
**/
int CCodeQueue::GetHeadCode(BYTE *pOutCode, int *psOutLength)
{
    int nTempMaxLength = 0;
    int nTempRet = -1;
    int nTempRead;
    int nTempWrite;
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

    // 如果数据为空
    if( nTempRead == nTempWrite )
    {
        *psOutLength = 0;
        return nTempRet;
    }

    //获取数据大小
    if( nTempRead < nTempWrite )
    {
        nTempMaxLength = nTempWrite - nTempRead;
    }
    else  // 如果数据的开始 idx 大于结束 idx
    {
        nTempMaxLength = m_stQueueHead.m_iSize - nTempRead + nTempWrite;
    }

    // 如果数据的最大长度不到 2
    if( nTempMaxLength < (int)sizeof(short) )
    {
        *psOutLength = 0;
        nTempRead = nTempWrite;
        SetCriticalData(nTempRead, -1);
        return -4;
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
        nTempRead = (nTempRead+1) % m_stQueueHead.m_iSize;
    }

    *psOutLength = usOutLength;

    //数据长度不合法
    if( (*psOutLength) > (int)(nTempMaxLength - sizeof(short)) || *psOutLength < 0 )
    {
        *psOutLength = 0;
        nTempRead = nTempWrite;
        SetCriticalData(nTempRead, -1);
        return -5;
    }

    pTempDst = (BYTE *)&pOutCode[0];  // 设置接收 Code 的地址

    //数据在内存去中间
    if( nTempRead < nTempWrite )
    {
        memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));
    }
        //数据在内存区两头
    else
    {
        // 如果出现分片，则分段拷贝
        if( m_stQueueHead.m_iSize - nTempRead < *psOutLength)
        {
            memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(m_stQueueHead.m_iSize - nTempRead));
            pTempDst += (m_stQueueHead.m_iSize - nTempRead);
            memcpy((void *)pTempDst, (const void *)&pTempSrc[0], (size_t)(*psOutLength - (m_stQueueHead.m_iSize - nTempRead));
        }
            // 否则，直接拷贝
        else
        {
            memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempRead], (size_t)(*psOutLength));
        }
    }

    nTempRead = (nTempRead + (*psOutLength)) % m_stQueueHead.m_iSize;
    SetCriticalData(nTempRead,-1);
    return nTempRet;
}

/**
  *函数名          : DeleteHeadCode
  *功能描述        : 删除共享内存管道的一个数据包（只改变读索引）
**/
int CCodeQueue::DeleteHeadCode()
{
	short sTempShort = 0;
	int nTempMaxLength = 0;
	int nTempRet = -1;
	int nTempRead;
	int nTempWrite;

	BYTE *pTempSrc;
	BYTE *pTempDst;
	BYTE *pbyCodeBuf;

	if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_iSize <= 0 )
	{
		return -1;
	}
	//获取共享内存管道的地址
	pbyCodeBuf = GetPipeAddr();
	//获取读写索引
	GetCriticalData(&nTempRead, &nTempWrite);
	nTempRet = nTempWrite;

	if(IsQueueEmpty())
	{
		CleanQueue();
		return nTempRet;
	}

    //获取数据大小
    if( nTempRead < nTempWrite )
    {
        nTempMaxLength = nTempWrite - nTempRead;
    }
    else  // 如果数据的开始 idx 大于结束 idx
    {
        nTempMaxLength = m_stQueueHead.m_iSize - nTempRead + nTempWrite;
    }

	if( nTempMaxLength < ( int )sizeof(short) )
	{
        nTempRead = nTempWrite;
        SetCriticalData(nTempRead, -1);
        return -3;
	}

    pTempDst = (BYTE *)&sTempShort;
    pTempSrc = (BYTE *)&pbyCodeBuf[0];
    for( unsigned int i = 0; i < sizeof(short); i++ )
    {
        pTempDst[i] = pTempSrc[nTempRead];
        nTempRead = (nTempRead+1) % m_stQueueHead.m_iSize;
    }

    if( sTempShort > (int)(nTempMaxLength - sizeof(short)) || sTempShort < 0 )
    {
        nTempRead = nTempWrite;
        SetCriticalData(nTempRead, -1);
        return -3;
    }

    nTempRead = (nTempRead + sTempShort) % m_stQueueHead.m_iSize;

    SetCriticalData(nTempRead, -1);

    return nTempRead;
}

/**
  *函数名          : GetOneCode
  *功能描述        : 从指定位置iCodeOffset获取指定长度nCodeLength数据
  * */
int CCodeQueue::GetOneCode(int iCodeOffset, int nCodeLength, BYTE *pOutCode, int *psOutLength)
{
	short nTempShort = 0;
	int nTempMaxLength = 0;
	int nTempRead;
	int nTempWrite;
	int iTempIdx;
	BYTE *pTempSrc;
	BYTE *pTempDst;
	BYTE *pbyCodeBuf;

	if( m_stQueueHead.m_iCodeBufOffSet <= 0 || m_stQueueHead.m_iSize <= 0 )
	{
		return -1;
	}

	pbyCodeBuf = (BYTE *)((BYTE *)this + m_stQueueHead.m_iCodeBufOffSet);

	if( !pOutCode || !psOutLength )
	{
		return -1;
	}

	if( iCodeOffset < 0 || iCodeOffset >= m_stQueueHead.m_iSize)
	{
		return -1;
	}
	if( nCodeLength < 0 || nCodeLength >= m_stQueueHead.m_iSize )
	{
		return -1;
	}
	GetCriticalData(&nTempRead, &nTempWrite);

    if( nTempRead == nTempWrite )
    {
        *psOutLength = 0;
        return 0;
    }

	//如果该位置是共享内存管道数据的头部，则直接获取
	if( nTempRead == iCodeOffset )
	{
		return GetHeadCode(pOutCode, psOutLength);
	}

	if( iCodeOffset < nTempRead || iCodeOffset >= nTempWrite )
	{
		*psOutLength = 0;
		return -1;
	}

    //获取数据大小
    if( nTempRead < nTempWrite )
    {
        nTempMaxLength = nTempWrite - nTempRead;
    }
    else  // 如果数据的开始 idx 大于结束 idx
    {
        nTempMaxLength = m_stQueueHead.m_iSize - nTempRead + nTempWrite;
    }


    if( nTempMaxLength < ( int )sizeof(short) )
    {
        *psOutLength = 0;
        nTempRead = nTempWrite;
        SetCriticalData(nTempRead, -1);
        return -3;
    }

    pTempDst = (BYTE *)&nTempShort;
    pTempSrc = (BYTE *)&pbyCodeBuf[0];
    iTempIdx = iCodeOffset;
    //获取数据长度
    for( unsigned int i = 0; i < sizeof(short); i++ )
    {
        pTempDst[i] = pTempSrc[iTempIdx];
        iTempIdx = (iTempIdx+1) % m_stQueueHead.m_iSize;
    }

    if( nTempShort > (int)(nTempMaxLength - sizeof(short)) || nTempShort < 0 || nTempShort != nCodeLength )
    {
        *psOutLength = 0;
        return -2;
    }

    //设置读索引为指定的索引
    SetCriticalData(iCodeOffset, -1);
    //从指定位置读取数据
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
  *函数名          : GetCriticalData
  *功能描述        : 获取读写索引 readindex，writeindex，剩余空间
**/

void CCodeQueue::GetCriticalData(int& iBegin, int& iEnd, int& iLeft)
{

    iBegin = m_stQueueHead.m_iReadIndex;
    iEnd = m_stQueueHead.m_iWriteIndex;

    int iTempMaxLength = 0;

    if( iBegin == iEnd )
    {
        iTempMaxLength = m_stQueueHead.m_iSize;
    }
    else if( iBegin > iEnd )
    {
        iTempMaxLength = iBegin - iEnd;
    }
    else
    {
        iTempMaxLength = (m_stQueueHead.m_iSize - iEnd) + iBegin;
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
	iPageCount = m_stQueueHead.m_iSize/iPageSize;
	for( i = 0; i < iPageCount; i++ )
	{
		fwrite((const void *)pPage, iPageSize, 1, fpDumpFile);
		pPage += iPageSize;
	}
	fwrite((const void *)pPage, m_stQueueHead.m_iSize - iPageSize*iPageCount, 1, fpDumpFile);

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
	iPageCount = m_stQueueHead.m_iSize/iPageSize;
	for( i = 0; i < iPageCount; i++ )                                                                             
	{                                                                                                             
		fread((void *)pPage, iPageSize, 1, fpDumpFile);
		pPage += iPageSize;                                                                                       
	}
	fread((void *)pPage, m_stQueueHead.m_iSize - iPageSize*iPageCount, 1, fpDumpFile);

	fclose(fpDumpFile);                                                                                           

	return 0;   
}

int CCodeQueue::CleanQueue()                                                                                      
{
	m_stQueueHead.m_iReadIndex = 0;
	m_stQueueHead.m_iWriteIndex = 0;
	return 0;
}

