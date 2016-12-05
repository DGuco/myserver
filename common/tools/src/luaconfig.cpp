#include "../inc/luaconfig.h"

CLuaconfig::CLuaconfig(void)
{
	L = lua_open();      //初始化lua
	luaL_openlibs(L);    //加载Lua基本库
}

CLuaconfig::~CLuaconfig(void)
{
	lua_close(L);        //清除Lua
}

//载入lua文件
bool CLuaconfig::LoadFile(const char* str)
{  
	if (luaL_dofile(L, str))
	{
		return false;
	}
	return true;
}

//读取Table表中的字符串
char* CLuaconfig::ReadStringFT(int tableindex)
{
	char *spTemp;
	lua_pushinteger(L, tableindex);
	lua_gettable(L, -2);
	if ( !lua_isstring(L, -1) )
	{
		printf("This is not string!\n");
		lua_error(L);
	}
	spTemp = (char*)lua_tostring(L, -1);
	lua_pop(L, 1);
	return spTemp;
}

//读取table表中的整形
int CLuaconfig::ReadIntegerFT(int tableindex)
{
	int mTemp;
	lua_pushinteger(L, tableindex);
	lua_gettable(L, -2);
	//if ( !lua_isnumber(L, -1) )
	if ( lua_type(L, -1) != LUA_TNUMBER )
	{
		printf("This is not number!\n");
    	lua_error(L);
	}
	mTemp = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return mTemp;
}

//加载table表
void  CLuaconfig::LoadTable(const char* str)
{
	lua_getglobal(L, str);
}

//读取lua文件中的int型数据
int CLuaconfig::ReadInt(int& pDesValue, const char* str, int nRunType)
{
	lua_getglobal(L,str);
	if ( !lua_isnumber(L, -1) )
	{
		printf("This is not number!\n");
		if ( 0 == nRunType )
		{
			// 启动服务器加载的时候 直接报错
			lua_error(L);
		}
		else
		{
			// 重新加载的时候 如果读错，直接返回
			return -1;
		}
	}
	pDesValue =  (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return 0;
}

//弹出栈顶
void CLuaconfig::Pop()
{
	lua_pop(L, 1);
}

//清除lua
void CLuaconfig::ClearLua()
{
	lua_close(L);
}

int CLuaconfig::ReadStringFT(char* pDesValue, int nDesLen, int nIndex, int nRow, const char*  pTemName, int nRunType)
{
	lua_pushinteger( L,  nIndex );
	lua_gettable( L,  -2 );
	
	//if ( !lua_isstring(L, -1) || lua_isnumber(L, -1) )
	if ( lua_type(L, -1) != LUA_TSTRING )
	{
		// line 后面表示行数， index后面表示列数， 此处错误 意思就是此行此列 应该是字符型， 策划填成了整形
		printf( "\nThe error occur at file %s, line %d, index %d, Data type is wrong, Params must be string. \n\n", 
				pTemName, nRow, nIndex );
		if ( 0 == nRunType )
		{
			// 启动服务器加载的时候 直接报错
			lua_error(L);
		}
		else
		{
			// 重新加载的时候 如果读错，直接返回
			return -1;
		}
	}
	char* spContent = (char*)lua_tostring(L, -1);
	lua_pop( L,  1 );

	strncpy( pDesValue, spContent, nDesLen-1 );

	return 0;
}

//读取table表中的整形
int CLuaconfig::ReadIntegerFT(long& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType)
{
	lua_pushinteger( L, nIndex );
	lua_gettable( L,  -2 );
	//if ( !lua_isnumber(L, -1) )
	if ( lua_type(L, -1) != LUA_TNUMBER )
	{
		// line 后面表示行数， index后面表示列数， 此处错误 意思就是此行此列 应该是整形， 策划填成了字符型
		printf("\nThe error occur at file %s, line %d, index %d, Data type is wrong, Params must be number. \n\n", 
				pTemName, nRow, nIndex );
		if ( 0 == nRunType )
		{
			// 启动服务器加载的时候 直接报错
			lua_error(L);
		}
		else
		{
			// 重新加载的时候 如果读错，直接返回
			return -1;
		}
	}

	pDesValue = (long)lua_tonumber(L, -1);
	lua_pop( L,  1 );

	return 0;
}

//读取table表中的整形
int CLuaconfig::ReadIntegerFT(int& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType)
{
	lua_pushinteger( L, nIndex );
	lua_gettable( L,  -2 );
	//if ( !lua_isnumber(L, -1) )
	if ( lua_type(L, -1) != LUA_TNUMBER )
	{
		// line 后面表示行数， index后面表示列数， 此处错误 意思就是此行此列 应该是整形， 策划填成了字符型
		printf("\nThe error occur at file %s, line %d, index %d, Data type is wrong, Params must be number. \n\n", 
				pTemName, nRow, nIndex );
		if ( 0 == nRunType )
		{
			// 启动服务器加载的时候 直接报错
			lua_error(L);
		}
		else
		{
			// 重新加载的时候 如果读错，直接返回
			return -1;
		}
	}

	pDesValue = (int)lua_tonumber(L, -1);
	lua_pop( L,  1 );

	return 0;
}

float CLuaconfig::ReadFloatFT(float& pDesValue, int nIndex, int nRow, const char*  pTemName,  int nRunType)
{
	lua_pushinteger( L, nIndex );
	lua_gettable( L,  -2 );
	//if ( !lua_isnumber(L, -1) )
	if ( lua_type(L, -1) != LUA_TNUMBER )
	{
		// line 后面表示行数， index后面表示列数， 此处错误 意思就是此行此列 应该是整形， 策划填成了字符型
		printf("\nThe error occur at file %s, line %d, index %d, Data type is wrong, Params must be number. \n\n",
				pTemName, nRow, nIndex );
		if ( 0 == nRunType )
		{
			// 启动服务器加载的时候 直接报错
			lua_error(L);
		}
		else
		{
			// 重新加载的时候 如果读错，直接返回
			return -1;
		}
	}

	pDesValue = (float)lua_tonumber(L, -1);
	lua_pop( L,  1 );

	return 0;
}
