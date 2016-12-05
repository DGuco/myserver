#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../inc/dirtyword.h"
// #include "tdirtyapi.h"

template<> CDirtyWord* CSingleton<CDirtyWord >::spSingleton = NULL;
int CDirtyWord::OI_Dirty_start()
{	
	//DirtyShmKey=1;
	//pstDirtyCore=NULL;
	if( OI_Dirty_InitShm() )
	{
		return -1;
	}
	if( ReadDirtyWordDB() < 0 )
	{
		return -2;
	}

	return 0;
}

int CDirtyWord::ReadDirtyWordDB()
{
	Dirty_DB stTempDirtyDB;
	memset((void*)&stTempDirtyDB, 0, sizeof(stTempDirtyDB));
	if(OI_Dirty_Chn_GetFromFile("../../config/dirty/cf.txt", &stTempDirtyDB))  // 从脏词库中获取中文脏词
	{
		return -1;
	}
	if(OI_Dirty_Chn_LoadToShm(&stTempDirtyDB))  // 把中文脏词放到共享内存中
	{
		return -2;
	}

	memset((void*)&stTempDirtyDB, 0, sizeof(stTempDirtyDB));
	if(OI_Dirty_Eng_GetFromFile("../../config/dirty/ef.txt", &stTempDirtyDB))
	{
		return -3;
	}
	if(OI_Dirty_Eng_LoadToShm(&stTempDirtyDB))
	{
		return -4;
	}

	return 0;
}

int CDirtyWord::OI_Drity_Chn_Add_Item(unsigned char * sDirtyWord, unsigned char *sKeyWord,int iCount, short iLevel )
{
	OI_DIRTY_CHN_RECORD    *pstDirtyRecord;
	unsigned char          *sSubstr;
	int                    iIndex,iOffset;

	if (iCount >=C_MAX_WORD_NUM|| iCount<0)
	{
		return -1;
	}

	if ( pstChnIndexTab == NULL)
	{
		return -2;
	}

	pstDirtyRecord= pstDirtyCore->astChnDirtyRec ;
	strcpy((char *)(pstDirtyRecord[iCount].sDirtyStr), (const char *)sDirtyWord); 
	strcpy((char *)(pstDirtyRecord[iCount].sKeyWord), (const char *)sKeyWord);
	pstDirtyRecord[iCount].iLevel = iLevel;
	sSubstr=(unsigned char *)strstr((const char *)sDirtyWord, (const char *)sKeyWord);

	if (sSubstr==NULL){
		return -3;	
	}

	pstDirtyRecord[iCount].iKeyOffset=(sSubstr-sDirtyWord); 
	pstDirtyRecord[iCount].iNextKey=-1;

//#ifdef _DEBUG_
//	printf("Init_Chn_Table:KeyOffset:%d\n",pstDirtyRecord[iCount].iKeyOffset);
//	printf("Init_Chn_Table:iCount:%d\n",iCount);
//#endif
	
	//CAL_INDEX_OFFSET(iIndex,sKeyWord[0],sKeyWord[1], sKeyWord[2])
	//iOffset=pstChnIndexTab->iDirtyIndexTable[iIndex];
	
	unsigned char tCurX = sKeyWord[0]-0xE0;
	unsigned char tCurY = sKeyWord[1]-0x80;
	unsigned char tCurZ = sKeyWord[2]-0x80;
	// 不符合utf-8格式
	if ( tCurX > 0xF || tCurY > 0x3F || tCurY > 0x3F ){ return 0; }

	iOffset=pstChnIndexTab->iDirtyIndexTable[tCurX][tCurY][tCurZ];

	if (iOffset==-1)
	{
		pstChnIndexTab->iDirtyIndexTable[tCurX][tCurY][tCurZ]=iCount;
	}
	else
	{ 
		while (iOffset!=-1)
		{
			if (strcmp( (const char *)(pstDirtyRecord[iOffset].sDirtyStr), (const char *)sDirtyWord) == 0 )
			{
				memset( &(pstDirtyRecord[iCount]),0,sizeof(pstDirtyRecord[iCount]));
				return 0;
			}
			iIndex=iOffset;
			iOffset=pstDirtyRecord[iOffset].iNextKey;
		}
		pstDirtyRecord[iIndex].iNextKey=iCount;
	}

	return 0; 
}

int CDirtyWord::OI_Dirty_InitShm()
{
	int	iShmSize;
	int iShmID;
	iShmSize=sizeof(OI_DIRTY_CORE);

	if(pstDirtyCore==NULL && (!DirtyShmKey))
	{
		DirtyShmKey =Def_DirtyShmKey;
		strcpy(DirtyGbTable,Def_DirtyGbTable);
		strcpy(DirtyEngTable,Def_DirtyEngTable);		
	}

	iShmID = shmget(DirtyShmKey, iShmSize, IPC_CREAT|IPC_EXCL|0666);
	if( iShmID < 0 )
	{
		if( errno != EEXIST )
		{
			return -1;
		}

		iShmID = shmget(DirtyShmKey, iShmSize, 0666);
		if( iShmID < 0 )
		{
			iShmID = shmget(DirtyShmKey, 0, 0666);
			if( iShmID < 0 )
			{
				return -2;
			}
			else
			{
				if( shmctl(iShmID, IPC_RMID, NULL) )
				{
					return -3;
				}
				iShmID = shmget(DirtyShmKey, iShmSize, IPC_CREAT|IPC_EXCL|0666);
				if( iShmID < 0 )
				{
					return -4;
				}
			}
		}
	}

	pstDirtyCore = (OI_DIRTY_CORE *)shmat(iShmID, NULL, 0);
	pstChnIndexTab = &(pstDirtyCore->stChnDirtyIndex);
	pstDirtyEngRec= pstDirtyCore->astEngDirtyRec;

	return 0;
}

int CDirtyWord::OI_Dirty_Eng_LoadToShm(Dirty_DB *pstDirtyDB)
{
	int iCount;
	if ( pstDirtyCore == NULL)
	{
		if(OI_Dirty_InitShm()<0)	return -1;	
	}

	pstDirtyCore->iEnable		=0;
	pstDirtyCore->aiEngWordCount = 0;
	memset(pstDirtyCore->astEngDirtyRec,0,sizeof(pstDirtyCore->astEngDirtyRec));

	for(iCount=0;iCount<pstDirtyDB->iDirtyEngItemNum;iCount++)
	{
		strcpy((char *)(pstDirtyEngRec[iCount].sDirtyStr), (const char *)(pstDirtyDB->stDirtyEngItems[iCount].sDirtyStr));
		pstDirtyEngRec[iCount].iLevel = pstDirtyDB->stDirtyEngItems[iCount].iLevel;
	}

	pstDirtyCore->aiEngWordCount = iCount;
	pstDirtyCore->ulEngVer = pstDirtyDB->ulEngVer;
	pstDirtyCore->iEnable=1;	

	return 0;
}

int CDirtyWord::OI_Dirty_Chn_GetFromFile(const char *szChnFile, Dirty_DB * pstDirtyDB)
{
	FILE *fpChnFile = NULL;
	char szLineBuf[1024];
	char *pNextToken;
	int iCount;
	Dirty_Chn_Item *pcur;

	if(!szChnFile || !pstDirtyDB)
	{
		return -1;
	}

	fpChnFile = fopen(szChnFile, "r");
	if(!fpChnFile)
	{
		return -2;
	}

	iCount = 0;
	for(;;)
	{
		fgets((char *)szLineBuf, sizeof(szLineBuf)-1, fpChnFile);
		if(feof(fpChnFile)){	break;	}
		TrimStr(szLineBuf);  // 获取一个关键词
		if(szLineBuf[0] == '#')  // ?
		{
			continue;
		}

		pNextToken = strtok(szLineBuf, " \t\r\n");  // 分割字符串
		if(!pNextToken)
		{
			continue;
		}

		pcur =&pstDirtyDB->stDirtyChnItems[iCount];  // 把脏词的存放地址赋给pcur
		memset(pcur,0,sizeof(Dirty_Chn_Item));  // 将该地址清空
		pcur->iLevel = atoi(pNextToken);  // 把字符串转换成整数，单词等级
		pNextToken = strtok(NULL, "|\r\n");
		if(!pNextToken)
		{
			continue;
		}

		strncpy((char *)(pcur->sDirtyStr), (const char *)pNextToken, sizeof(pcur->sDirtyStr));  // 脏词
		pcur->sDirtyStr[sizeof(pcur->sDirtyStr)-1]=0;  // 置位
		pNextToken = strtok(NULL, "\r\n");
		if(pNextToken)
		{
			strncpy((char *)(pcur->sKeyWord), (const char *)pNextToken, sizeof(pcur->sKeyWord));  // 关键词
			pcur->sKeyWord[sizeof(pcur->sKeyWord)-1] = 0;
		}

		iCount++;

//#ifdef _DEBUG_
//		printf("Level : %d, Dirty String : \"%s\", Key Words : \"%s\"\n",
//		pcur->iLevel, pcur->sDirtyStr, pcur->sKeyWord);
//#endif

	}
	fclose(fpChnFile);
	pstDirtyDB->iDirtyChnItemNum = iCount;

	return 0;
}

int CDirtyWord::OI_Dirty_Chn_LoadToShm(Dirty_DB *pstDirtyDB)
{
	int iCount;
	Dirty_Chn_Item *pcur;

	if ( pstChnIndexTab == NULL)
	{
		if(OI_Dirty_InitShm()<0)	return -1;	 // 初始化共享内存错（获取已经申请的共享内存）
	}	

	pstDirtyCore->iEnable = 0;
	pstDirtyCore->aiChnWordCount = 0;  // 过滤个数
	memset( pstDirtyCore->astChnDirtyRec,0,sizeof(pstDirtyCore->astChnDirtyRec));
	//for (iCount=0;iCount<C_MAX_TABLE_LEN;iCount++)
	//{
	//pstChnIndexTab->iDirtyIndexTable[iCount]=-1;
	//}
	for ( int i = 0; i < MAX_X; i++ )
		for ( int j = 0; j < MAX_Y; j++ )
			for ( int k = 0; k < MAX_Z; k++ )
			{
				pstChnIndexTab->iDirtyIndexTable[i][j][k] = -1;
			}

	for(iCount =0;iCount<pstDirtyDB->iDirtyChnItemNum;iCount++)
	{
		pcur = &pstDirtyDB->stDirtyChnItems[iCount];
		if (OI_Drity_Chn_Add_Item(pcur->sDirtyStr,pcur->sKeyWord, iCount,
					pcur->iLevel)<0)
		{
			pstDirtyCore->iEnable		=1;
			printf("%d, word error!!",  iCount); 
			return -2;
		}

	}

	pstDirtyCore->aiChnWordCount = iCount;
	pstDirtyCore->ulChnVer = pstDirtyDB->ulChnVer;
	pstDirtyCore->iEnable		=1;

	return 0;
}

// 混合检查  只保留 中英数 
int CDirtyWord::OI_Dirty_Mix_Check(int iLevel,unsigned char *sCheckStr)
{
	unsigned char	cLoChar,cHiChar;  // 双字节字符的低字节和高字节
	unsigned char	cMidChar;		  // 对应于utf-8的中间字节
	unsigned char   *sReservBuff;
	int				iIsDirty=0,iCount=0,iIndex,iOffset;
//	int				iKeyOff;
	OI_DIRTY_CHN_RECORD	*pstDirtyRecord;

	if (sCheckStr==NULL||strlen((const char *)sCheckStr)==0)
	{
		return -1;	
	}

	sReservBuff=sCheckStr;  // 获取待比较的字符串的内存指针
	// 注释掉的部分
	iCount =0;
	pstDirtyRecord =pstDirtyCore->astChnDirtyRec;  // 获取脏词
	while((cLoChar=sReservBuff[iCount++])!='\0'  && iCount  < (int)strlen((char*)sCheckStr) )  // 比较脏词
	{
		if(IS_ENGLISH_CHAR(cLoChar) ||IS_DIGIT(cLoChar)||IS_ENGLISH_PUNCTUATION(cLoChar) )  
		{
			iIndex=0;    
			while (pstDirtyEngRec[iIndex++].sDirtyStr[0]!='\0'  && iIndex < E_MAX_WORD_NUM  ) 
			{				
				if( (iLevel & pstDirtyEngRec[iIndex-1].iLevel) 
						&& EQUAL_ENGLISH_CHAR(cLoChar,pstDirtyEngRec[iIndex-1].sDirtyStr[0]))  
				{
					if(strncasecmp((const char *)(sReservBuff+iCount-1),
								(const char *)(pstDirtyEngRec[iIndex-1].sDirtyStr),
								strlen((const char *)(pstDirtyEngRec[iIndex-1].sDirtyStr)))==0)								  {
						unsigned char *ChangeArea=sReservBuff+iCount-1;
						for (unsigned int i=0;i<strlen((const char *)(pstDirtyEngRec[iIndex-1].sDirtyStr));i++)
						{
							*ChangeArea='*';
							ChangeArea++;
						}
						iIsDirty=-1;
						iCount=iCount-1+strlen((const char *)(pstDirtyEngRec[iIndex-1].sDirtyStr));
						break;
					}
				}
			} 
		}  
		else if (IS_THREE_BYTES(cLoChar))	// 三个节字符
		{
			cMidChar=sReservBuff[iCount++];
			cHiChar=sReservBuff[iCount++];  // 获取高位字节
			if(cHiChar =='\0'||cMidChar == '\0') break;  // 如果高位字节为空则退出外层循环
			unsigned char tCurX = cLoChar-0xE0;
			unsigned char tCurY = cMidChar-0x80;
			unsigned char tCurZ = cHiChar-0x80; 
			if ( tCurX > 0xF || tCurY > 0x3F || tCurY > 0x3F )
			{
				// 不符合utf-8格式
				break;
			}
			iOffset=pstChnIndexTab->iDirtyIndexTable[tCurX][tCurY][tCurZ];
			while (iOffset>=0&&iOffset<C_MAX_WORD_NUM)	  // 比较中文字符
			{	
//				iKeyOff=pstDirtyRecord[iOffset].iKeyOffset;  // 获取脏字关键字的偏移量
				if(pstDirtyRecord[iOffset].sKeyWord[0]!=cLoChar ||
				   pstDirtyRecord[iOffset].sKeyWord[1]!=cMidChar ||
				   pstDirtyRecord[iOffset].sKeyWord[2]!=cHiChar )
					break;
				if ( pstDirtyRecord[iOffset].iLevel & iLevel )  // 如果屏蔽等级达到指定值
				{
					if(iCount-3 >= pstDirtyRecord[iOffset].iKeyOffset)
					{
						if(strncasecmp((const char *)(sReservBuff+(iCount-3 - pstDirtyRecord[iOffset].iKeyOffset)), (const char *)(pstDirtyRecord[iOffset].sDirtyStr),
									strlen((const char *)(pstDirtyRecord[iOffset].sDirtyStr)))==0)								{
							unsigned char *ChangeArea=sReservBuff+iCount-3-pstDirtyRecord[iOffset].iKeyOffset;
							for (unsigned int i=0;i<strlen((const char *)(pstDirtyRecord[iOffset].sDirtyStr));i++)
							{
								*ChangeArea='*';
								if( IS_THREE_BYTES((unsigned char)pstDirtyRecord[iOffset].sDirtyStr[i] )&&
										IS_MULTI_SUB_BYTE((unsigned char)pstDirtyRecord[iOffset].sDirtyStr[i+1] )&&
										IS_MULTI_SUB_BYTE((unsigned char)pstDirtyRecord[iOffset].sDirtyStr[i+2]) )
								{
									i += 2;
								}		

								ChangeArea++;
							}
							unsigned char *DesChar=sReservBuff+iCount-3+strlen((const char *)(pstDirtyRecord[iOffset].sDirtyStr));
							int copystrlen=strlen((const char *)DesChar);
							iCount=ChangeArea-sReservBuff;
							for (int i=0;i<copystrlen;i++)
							{
								*ChangeArea=*DesChar;
								DesChar++;
								ChangeArea++;
							}
							*ChangeArea='\0'; 
							iIsDirty=-1;

							break;
						}
					}	

					iOffset=pstDirtyRecord[iOffset].iNextKey;  // 下一个含有该关键字得短语
				}
			}  // chndirty
		}  // if chinese char
	}  // if doublechar

	return iIsDirty;
}

// 结果为0, 表示没有需要屏蔽的字,  用于检查
int CDirtyWord::OI_Dirty_Check(int iLevel, char *sMsg, int nFlag/*=0*/  )
{
	int iRet;
	if (pstDirtyCore==NULL||pstChnIndexTab==NULL||pstDirtyEngRec == NULL)
	{
		iRet=OI_Dirty_InitShm();  // 返回初始化共享内存检测码
		if(iRet<0) 
		{
			return -1;	
		}
	}

	if(!pstDirtyCore->iEnable) 
	{
		return -2;
	}

//	int tLevel = 1;
//	iRet = utf8DirtyCheck(1, sMsg, &tLevel );
//	if ( iRet == 0 && nFlag == 0 )
//	{
		iRet = OI_Dirty_Mix_Check(iLevel, (unsigned char *)sMsg);
//	}

	return iRet;
}

// 用于替换
int CDirtyWord::OI_Dirty_Replace(int iLevel,  char* sMsg,  int nFlag) 
{
	int iRet = 0; 	
//	int tLevel = 0; 
//	iRet = utf8DirtyReplace(1, sMsg, &tLevel); 

	unsigned char	cLoChar;  
	char *sReservBuff;
	int	 iCount=0;
	sReservBuff = sMsg; 
	if (sReservBuff==NULL||strlen((const char *)sReservBuff)==0)
	{
		return -1;	
	}
	iCount =0;
	const char* pNo1 = "|"; 
	const char* pNo2 = "\n"; 
	int tLength = strlen(sMsg);  
	for ( int i = 0;  i < tLength && iCount < tLength;  i++ )
	{
		cLoChar=sReservBuff[iCount]; 
		if(IS_ENGLISH_CHAR(cLoChar)||IS_DIGIT(cLoChar)||IS_ENGLISH_PUNCTUATION(cLoChar) )
		{
			if(strncasecmp((const char *)(sReservBuff+iCount), pNo1,  strlen(pNo1))  == 0 ||
			   strncasecmp((const char *)(sReservBuff+iCount), pNo2,  strlen(pNo2))  == 0 
			  )
			{
				char *ChangeArea=sReservBuff+iCount;
				*ChangeArea = '*'; 
			}
			iRet = -1; 
		}
		iCount++; 
		//		else if (IS_TWO_BYTES(cLoChar))	
		//		{
		//			iCount += 2; 
		//		}
		//		else if (IS_THREE_BYTES(cLoChar))
		//		{
		//			iCount += 3; 
		//		}
		//		else if (IS_FOUR_BYTES(cLoChar))
		//		{
		//			iCount  += 4;   
		//		}
		//		else if (IS_FIVE_BYTES(cLoChar))
		//		{
		//			iCount += 5; 
		//		}
		//		else if (IS_SIX_BYTES(cLoChar))
		//		{
		//			iCount += 6; 
		//		}
		//		else
		//		{
		//			iCount++; 
		//		}
	}

	return iRet;  
}

int CDirtyWord::OI_Dirty_Eng_GetFromFile(const char *szEngFile, Dirty_DB * pstDirtyDB)
{
	FILE *fpEngFile = NULL;
	char szLineBuf[1024];
	char *pNextToken;
	int iCount;
	Dirty_Eng_Item *pcur;

	if(!szEngFile || !pstDirtyDB)
	{
		return -1;
	}

	fpEngFile = fopen(szEngFile, "r");
	if(!fpEngFile)
	{
		return -2;
	}

	iCount = 0;
	for(;;)
	{
		fgets((char *)szLineBuf, sizeof(szLineBuf)-1, fpEngFile);
		if(feof(fpEngFile))
		{
			break;
		}

		TrimStr(szLineBuf);
		if(szLineBuf[0] == '#')
		{
			continue;
		}

		pNextToken = strtok(szLineBuf, " \t\r\n");
		if(!pNextToken)
		{
			continue;
		}

		pcur = &pstDirtyDB->stDirtyEngItems[iCount];
		memset(pcur,0,sizeof(Dirty_Chn_Item));
		pcur->iLevel = atoi(pNextToken);
		pNextToken = strtok(NULL, "\r\n");
		if(!pNextToken)
		{
			continue;
		}

		strncpy((char *)(pcur->sDirtyStr), (const char *)pNextToken, sizeof(pcur->sDirtyStr));
		pcur->sDirtyStr[sizeof(pcur->sDirtyStr)-1]=0;
		iCount++;

//#ifdef _DEBUG_
//		printf("Level : %d, Dirty String : \"%s\"\n", pcur->iLevel, pcur->sDirtyStr);
//#endif
	}

	fclose(fpEngFile);
	pstDirtyDB->iDirtyEngItemNum = iCount;

	return 0;
}

CDirtyWord::CDirtyWord()
{
	pstChnIndexTab = NULL ;
	pstDirtyCore = NULL;
	pstDirtyEngRec = NULL;
	DirtyShmKey = 0 ;  // dirty 配置的共享内存
	memset( DirtyGbTable, 0, sizeof( DirtyGbTable ) ); 
	memset( DirtyEngTable, 0, sizeof( DirtyEngTable ) );
}

CDirtyWord::~CDirtyWord()
{

}
