#ifndef _OI_DIRTY_CHECK_H_
#define _OI_DIRTY_CHECK_H_

#include "servertool.h"
#include "base.h"

#define  Def_DirtyShmKey	17008
#define  Def_DirtyGbTable	 "dirty_gb"
#define  Def_DirtyEngTable	"dirty_eng"

#define  C_MAX_WORD_LEN   256
#define  C_MAX_TABLE_LEN  (62*64*64)+(64*64)+64+1
#define  C_MAX_WORD_NUM   6000
#define  E_MAX_WORD_NUM   500

#define  LEVEL_SW			16
#define  MAX_X				64
#define	 MAX_Y				64
#define	 MAX_Z				64

/*
#define  IS_DOUBLE_CHAR(ch) (ch>=0xA0&&ch<0xFF)  
#define  IS_ENGLISH_CHAR(ch) (ch>='a'&&ch<='z'||ch>='A'&&ch<='Z') 
#define  IS_CHINESE_CHAR(lo,hi) (lo>=0xB0&&lo<=0xF7&&hi>=0xA1&&hi<=0xFE) 
#define  IS_ENGLISH_DOUBLE(lo,hi) (lo==0xA3&&(hi<=0xDA&&hi>=0xC1||hi<=0xFA&&hi>=0xE1))
#define  CONVERT_DOUBLE_TO_SINGLE(ch,lo,hi) ch=(hi>=0xC1&&hi<=0xDA)?(hi-0x80):(hi-0xA0); 
#define  CONVERT_CAPITAL_CHAR(ch,lo) ch=(lo>='a'&&lo<='z')?lo-0x20:lo;
#define  EQUAL_ENGLISH_CHAR(lo,hi) (lo==hi||lo==(hi-0x20))
*/
#define	 IS_MULTI_FIRST_BYTE(ch)	( ch>=0xC0&&ch<=0xFD )

#define  IS_ONE_BYTES(ch)			( ch >= 0x00 && ch <= 0x7F )
#define	 IS_TWO_BYTES(ch)			( ch >= 0xC0 && ch <= 0xDF )
#define  IS_THREE_BYTES(ch)			( ch >= 0xE0 && ch <= 0xEF )
#define  IS_FOUR_BYTES(ch)			( ch >= 0xF0 && ch <= 0xF7 )
#define	 IS_FIVE_BYTES(ch)			( ch >= 0xF8 && ch <= 0xFB )
#define	 IS_SIX_BYTES(ch)			( ch >= 0xFC && ch <= 0xFD )
#define	 IS_MULTI_SUB_BYTE(ch)		( ch>=0x80&&ch<=0xBF )		

#define  IS_DOUBLE_CHAR(ch) 		( ch>=0xA0&&ch<0xFF)
#define  IS_ENGLISH_CHAR(ch)		( (ch>='a'&&ch<='z') || (ch>='A'&&ch<='Z') )
//IS_CHINESE_CHAR这个集合包含了全角英文和全角数字
#define  IS_CHINESE_CHAR(lo,hi)		( (lo>0x80 && lo<0xff) && ((hi>0x3f && hi<0x7f)||(hi>0x7f && hi<0xff)))
//// 中文标点符号
#define  IS_CHINESE_PUNC(lo,hi)		( (lo>0xa0 && lo<0xb0) && (hi>0x3f && hi<0xff))
#define  IS_ENGLISH_DOUBLE(lo,hi)	( (lo==0xA3) &&( (hi<=0xDA && hi>=0xC1) || (hi<=0xFA && hi>=0xE1) ))
#define  IS_DIGIT_DOUBLE(lo,hi) 	( (lo==0xA3) &&(hi>=0xB0 && hi<=0xB9) )
#define  IS_DIGIT(ch)  (isdigit(ch))

// 标点符号
#define	 IS_ENGLISH_PUNCTUATION(ch)		( ch>=0x01&&ch<=0x80 )		

//全角??-->大写的
#define  CONVERT_DOUBLE_TO_SINGLE(ch,lo,hi)	ch=(hi>=0xC1&&hi<=0xDA)?(hi-0x80):(hi-0xA0)
#define  CONVERT_DOUBLE_DIGIT_TO_SINGLE(ch,lo,hi)  ch=hi-0x80
#define  CONVERT_CAPITAL_CHAR(ch,lo)			 ch=(lo>='a'&&lo<='z')?lo-0x20:lo
#define  EQUAL_ENGLISH_CHAR(lo,hi)  ( lo==hi || lo==(hi-0x20))
#define CHAR_KEYWORD_DELI '|'
//#define CAL_INDEX_OFFSET(offset,hi,lo)	offset=(hi-0x80)*256+lo;
#define CAL_INDEX_OFFSET2(offset,hi,lo)	offset=(hi-0x80)*256+lo;
#define	CAL_INDEX_OFFSET(offset,hi,mid,lo) offset=(hi-0xC0)*64*64+(mid-0x80)*64+lo;
//#define	CAL_INDEX_OFFSET(offset,mid,lo) offset=(mid-0x80)*64+lo;

typedef struct _OI_DIRTY_CHN_RECORD
{
	unsigned char 	sDirtyStr[C_MAX_WORD_LEN+1]; /* 脏话短语 */
	unsigned char 	sKeyWord[4];				 /* 关键字眼 */
	short			iKeyOffset;					  /* 相对于关键字在sDirtystr中的偏移 */
	short			iNextKey;					  /* 下一个有相同关键字的脏话短语 */ 
	int				iLevel;
	char sBase[20];
}OI_DIRTY_CHN_RECORD; //内存中中文

typedef struct _OI_DIRTY_ENG_RECORD 
{
	unsigned char sDirtyStr[C_MAX_WORD_LEN+1];
	int	 iLevel;
	char sBase[20];
}OI_DIRTY_ENG_RECORD;//内存中英文

typedef struct _OI_DIRTY_SINGLE_WORD_RECORD 
{
	unsigned char sDirtyWord[4];
}OI_DIRTY_SW_RECORD;

typedef struct _OI_DIRTY_CHN_INDEX 
{
	short   iDirtyIndexTable[MAX_X][MAX_Y][MAX_Z];
} OI_DIRTY_CHN_INDEX;

typedef struct _OI_DIRTY_CORE
{
	int						iEnable ;			//是否有效  1 有效
	time_t					lChnUpdateTime;		//文件的时间
	time_t					lEngUpdateTime;
	time_t					lSwUpdateTime;
	OI_DIRTY_CHN_INDEX		stChnDirtyIndex;	//索引
	int						aiChnWordCount;		//过滤个数
	int						aiEngWordCount;
	int						aiSigWordCount;
	OI_DIRTY_CHN_RECORD		astChnDirtyRec[C_MAX_WORD_NUM];//中文脏话纪录
	OI_DIRTY_ENG_RECORD		astEngDirtyRec[E_MAX_WORD_NUM];//英文脏话纪录
	short   				iDirtySwIndex[C_MAX_TABLE_LEN];
	OI_DIRTY_SW_RECORD      astSwDirtyRec[E_MAX_WORD_NUM];
	unsigned long			ulChnVer;						//中英文的版本号
	unsigned long			ulEngVer;
	char					sReserve[112];
}OI_DIRTY_CORE; //内存结构

typedef struct
{
	unsigned char 	sDirtyStr[C_MAX_WORD_LEN+1];  /* 脏话短语 */
	unsigned char 	sKeyWord[4];				  /* 关键字眼 */
	int	iLevel;
}Dirty_Chn_Item; // db中结构

typedef struct
{
	unsigned char sDirtyStr[C_MAX_WORD_LEN+1];
	int iLevel;
}Dirty_Eng_Item; // db中结构

typedef struct
{
	Dirty_Chn_Item stDirtyChnItems[C_MAX_WORD_NUM];
	int iDirtyChnItemNum;
	unsigned long ulChnVer;//版本
	Dirty_Eng_Item stDirtyEngItems[E_MAX_WORD_NUM];
	int iDirtyEngItemNum;
	unsigned long ulEngVer;
}Dirty_DB; // 从db中获取得机构

class CDirtyWord : public CSingleton< CDirtyWord >
{
private:
	OI_DIRTY_CHN_INDEX *pstChnIndexTab ;
	OI_DIRTY_CORE	   *pstDirtyCore ;
	OI_DIRTY_ENG_RECORD *pstDirtyEngRec;
	int DirtyShmKey ;						// dirty配置的共享内存
	char DirtyGbTable[255]; 
	char DirtyEngTable[255];

private:
	int OI_Dirty_InitShm();
	//将pstDirtyDb中中文信息加载到shm 
	int OI_Dirty_Chn_LoadToShm(Dirty_DB *pstDirtyDB);
	int OI_Dirty_Eng_LoadToShm(Dirty_DB *pstDirtyDB);
	//从文件中获取中文dirty信息到结构pstDirtyDB
	int OI_Dirty_Chn_GetFromFile(const char *szChnFile, Dirty_DB * pstDirtyDB);
	int OI_Dirty_Eng_GetFromFile(const char *szEngFile, Dirty_DB * pstDirtyDB);	
	int OI_Dirty_Mix_Check(int iLevel,unsigned char *sCheckStr);   
	int ReadDirtyWordDB();
	int OI_Drity_Chn_Add_Item (unsigned char * sDirtyWord, unsigned char *sKeyWord,int iCount, short iLevel );
public:   
	int OI_Dirty_start();
	int OI_Dirty_Check(int iLevel,char *sMsg, int nFlag = 0);	
	int OI_Dirty_Replace(int iLevel,  char* sMsg,  int nFlag = 0); 
	CDirtyWord();
	~CDirtyWord();
};
#endif 

