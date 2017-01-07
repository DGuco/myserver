#ifndef _LUACONFIG_H_
#define _LUACONFIG_H_

/**********************************************************
*说明:
*CLuaconfig是读取lua配置文件的类
*在使用配置文件的地方实例化一个CLuaconfig即可
************************************************************/

#include<stdio.h>
#include<string.h>
extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

class CLuaconfig
{
public:
	CLuaconfig(void);
	~CLuaconfig(void);
  	bool   LoadFile(const char* str);																	//载入lua文件
    char*  ReadStringFT(int tableindex);																//读取table中字符串
 	int    ReadIntegerFT(int tableindex);																//读取table中整型
    float  ReadFloatFT(int tableindex);																	//读取table浮点型
    void   LoadTable(const char* str);																	//读取table表名
    int    ReadInt(int& pDesValue, const char* str, int nRunType);																	//读取整形
    void   Pop();  
    void   ClearLua();

	// nIndex: 列号 nRow: 行数 pDsValue: 存放字符串的空间 nDesLen: 表示空间长度 pTemName: 文件名字 nRunType: 读取类型 
	int   ReadStringFT(  char* pDesValue, int nDesLen, int nIndex, int nRow, const char*  pTemName, int nRunType);					// 读取table中字符串
	// nIndex: 列号 nRow: 行数  pTemName: 文件名字   
	int   ReadIntegerFT( long& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType);							    // 读取table中整形
	int   ReadIntegerFT( int& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType);							    // 读取table中整形
	float ReadFloatFT(float& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType);
private:
	lua_State *L;																					    //指向Lua解释器指针
};
#endif

