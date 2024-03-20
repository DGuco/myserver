//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include "proxy_ctrl.h"
#include "signal_handler.h"

#include <stdio.h>
#include <functional>
#include "lua_bridge.h"

using namespace luabridge;

int Add(int a, int b)
{
	printf("lua call Add,a = %d,b = %d\n", a, b);
	return a + b;
}

int SpaceAdd(int a, int b)
{
	printf("lua call SpaceAdd,a = %d,b = %d\n", a, b);
	return a + b;
}

int Sub(int a, int b)
{
	printf("lua call Sub,a = %d,b = %d\n", a, b);
	return a - b;
}

void Say(const char* contex)
{
	printf("Global Say = %s\n", contex);
}

class Test
{
public:
	Test()
	{
		printf("Test()\n");
	}
	~Test()
	{
		printf("~Test()\n");
	}
};

int LuaFnAdd(lua_State* L)
{
	int a = Stack<int>::get(L, 1);
	int b = Stack<int>::get(L, 2);
	printf("LuaFnAdd call ,a = %d,b = %d\n", a, b);
	lua_pushinteger(L, a + b);
	return 1;
}

struct OuterClass
{
	OuterClass(int a)
	{
		printf("OuterClass,a = %d\n", a);
	}

	~OuterClass()
	{
		printf("~OuterClass\n");
	}

	void Say(char* world)
	{
		printf("*****OuterClass::Say: %s******\n", world);
	}
public:
	static int data;
};

struct OuterClass1
{
	OuterClass1(int a)
	{
		printf("OuterClass1,a = %d\n", a);
	}

	~OuterClass1()
	{
		printf("~OuterClass1\n");
	}

	void Say(char* world)
	{
		printf("*****OuterClass1::Say: %s******\n", world);
	}
public:
	static int data;
};

int OuterClass::data = 0;

std::function<int(int, int)> func = [](int a, int b) -> int
{
	printf("c++ Lambda func add a = %d,b = %d\n", a, b);
	return a + b;
};

std::shared_ptr<OuterClass1> g_pt = std::shared_ptr<OuterClass1>(new OuterClass1(100));

int LuaFnGetOurterClass(lua_State* L)
{
	return LuaBridge::PushSharedObjToLua<OuterClass1>(L, g_pt);
}

void TestLua()
{
	try {
		lua_State* L = luaL_newstate();
		LuaBridge luaBridge(L);
		luaBridge.LoadFile("../script/111111.lua");

		BEGIN_NAMESPACE(luaBridge, "space")
			BEGIN_CLASS(luaBridge, OuterClass)
			LuaHelper::DumpTable(L, -1, std::cout, 2);
		LuaHelper::DumpTable(L, -2, std::cout, 2);
		LuaHelper::DumpTable(L, -3, std::cout, 2);
		LuaHelper::DumpTable(L, -4, std::cout, 2);
		CLASS_ADD_CONSTRUCTOR(void(*)(int))
			CLASS_ADD_FUNC("Say", &OuterClass::Say)
			END_CLASS
			BEGIN_REGISTER_CFUNC(luaBridge)
			REGISTER_CFUNC("Add", SpaceAdd)
			END_REGISTER_CFUNC
			END_NAMESPACE

			BEGIN_CLASS(luaBridge, OuterClass)
			CLASS_ADD_CONSTRUCTOR(void(*)(int))
			CLASS_ADD_FUNC("Say", &OuterClass::Say)
			CLASS_ADD_STATIC_PROPERTY("data", &OuterClass::data)
			END_CLASS

			BEGIN_CLASS(luaBridge, OuterClass1)
			CLASS_ADD_CONSTRUCTOR(void(*)(int))
			CLASS_ADD_FUNC("Say", &OuterClass1::Say)
			END_CLASS

			BEGIN_SHARED_CLASS(luaBridge, OuterClass1)
			CLASS_ADD_CONSTRUCTOR(void(*)(int))
			CLASS_ADD_FUNC("Say", &OuterClass1::Say)
			END_CLASS

			BEGIN_REGISTER_CFUNC(luaBridge)
			REGISTER_CFUNC("Add", Add)
			REGISTER_CFUNC("LambdaAdd", func)
			REGISTER_CFUNC("Sub", Sub)
			REGISTER_CFUNC("Say", Say)
			REGISTER_CFUNC("LuaFnAdd", LuaFnAdd)
			REGISTER_CFUNC("LuaFnGetOurterClass", LuaFnGetOurterClass)
			END_REGISTER_CFUNC

			int ret = luaBridge.CallLuaFunc<int>("x11111_callfailedtest", 1, 2, 200, 100, "Hello lua");
		printf("ret = %d\n", ret);
		printf("-------------------\n");
		ret = luaBridge.CallLuaFunc<int>("x11111_test", 1, 2, BinaryStr("1111111111", 11));
		printf("ret = %d\n", ret);

	}
	catch (std::exception& e) {
		printf("run catch one execption,msg = %s\n", e.what());
	}
	g_pt->Say("Cpp hello OuterClass1");
	printf("main end\n");
	return;
}

using namespace std;
int main(int argc, char **argv)
{
	TestLua();
	//信号处理注册
	CSignalHandler::GetSingletonPtr()->RegisterHandler("proxyserver");
	
	if (!INIT_LOG("proxyserver"))
	{
		exit(0);
	}

	try
	{
		int iTmpRet = CProxyCtrl::GetSingletonPtr()->PrepareToRun();
		if (!iTmpRet)
		{
			DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,iRet = {}", iTmpRet);
			exit(0);
		}
	}
	catch (const std::exception& e)
	{
		DISK_LOG(ERROR_DISK, "CGateCtrl PrepareToRun failed,get exception = {}", e.what());
		exit(0);
	}

	CProxyCtrl::GetSingletonPtr()->Run();
	// 关闭日志
	SHUTDOWN_ALL_LOG();
}

