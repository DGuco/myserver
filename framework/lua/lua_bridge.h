/******************************************************************************
* Name: LuaBridge for C++
*
* Author: DGuco(杜国超)
* Date: 2019-12-07 17:15
* E-Mail: 1139140929@qq.com
*
* Copyright (C) 2019 DGuco(杜国超).  All rights reserved.
*
* License: The MIT License (http://www.opensource.org/licenses/mit-license.php)
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#ifndef  __LUA_BRIDGE_H__
#define  __LUA_BRIDGE_H__

#include <map>
#include <string>
#include <utility>
#include <memory>
#include "lua_file.h"

namespace luabridge
{
class LuaBridge
{
public:
    /**
     *
     */
    LuaBridge();
    /**
     * Construce
     * @param VM
     */
    LuaBridge(lua_State *VM);

    /**
     * destruct
     */
    ~LuaBridge();

    /**
     * load lua file
     * @param filePath
     * @return
     */
    bool LoadFile(const std::string &filePath);

    bool LoadFile(const char *filePath);

    /**
     * Call Lua function
     * @tparam R    返回类型
     * @tparam Args 函数参数列表
     * @param func  函数名
     * @param args  函数参数列表
     * @return R    Return type. (void, float, double, int, long, bool, const char*, std::string)
     * Sample:	double f = lua.Call<double>("test0", 1.0, 3, "param");
     */
    template<typename R, typename ...Args>
    R CallLuaFunc(const char *func, const Args... args);
    /**
     *
     * @param func 函数名
     * @param sig  函数签名
     * 格式如p*[:r*]或p*[>r*], 冒号或者大于号前面为参数，后面为返回值，每个字母为每一个参数的类型
     * 类型表示为 f 或 e 表示float； i 或 n 或 d 表示整数； b 表示bool； s 表示字元串S 表示char* 数组，前面是长度，后面是char*指针
     * @param ...
     * @return  返回 返回值如果为 NULL， 表示调用成功。否则返回错误信息
     */
    // 例1︰ double f; const char* error_msg = lua.CallLuaFunc(const char* scriptName, "test01", "nnnn:f", 1,2,3,4,&f);
    // 例2︰ const char* s; int len; const char* error_msg = lua.CallLuaFunc(const char* scriptName, "test01", "S:S", 11, "Hello\0World", &len, &s);
    const char *Call(const char *func, const char *sig, ...);

    /**
     * 把c++中的对象放入lua栈中
     * @tparam T
     * @param L
     * @param ptr
     */
    template<class T>
    static int PushSharedObjToLua(lua_State *L, std::shared_ptr<T> ptr);
    /**
     * @return _G TABLE
     */
    Namespace &GetGlobalNamespace();

    /**
     * @param name
     * @return
     */
    Namespace &BeginNameSpace(char *name);

    /**
     *
     * @return
     */
    Namespace &CurNameSpace();
    /**
     *Continue namespace registration in the parent.
     * Do not use this on the global namespace.
     */
    void EndNamespace();

    /**
     * lua_State
     * @return
     */
    lua_State *LuaState();
private:
    //InitLuaLibrary
    void InitLuaLibrary();

    //把参数压栈
    int PushToLua();

    template<typename T>
    int PushToLua(const T &t);

    template<typename First, typename... Rest>
    int PushToLua(const First &first, const Rest &...rest);

    inline void SafeBeginCall(const char *func);

    template<typename R, int __>
    inline R SafeEndCall(const char *func, int nArg);

    template<int __>
    inline void SafeEndCall(const char *func, int nArg);

private:
    LuaVm *m_pLuaVm;
    Namespace m_namespace;
    int m_iTopIndex;
};

LuaBridge::LuaBridge()
    : m_iTopIndex(0)
{
    lua_State *pState = luaL_newstate();
    if (pState == NULL) {
        throw std::runtime_error("LuaBridge constructor luaL_newstate() failed");
    }
    m_pLuaVm = new LuaVm(pState);
    // initialize lua standard library functions
    InitLuaLibrary();
    LuaException::EnableExceptions(m_pLuaVm->LuaState());
}

LuaBridge::LuaBridge(lua_State *VM)
    : m_iTopIndex(0)
{
    if (VM == NULL) {
        throw std::runtime_error("LuaBridge constructor failed");
    }
    m_pLuaVm = new LuaVm(VM);
    m_namespace.Reset();
    // initialize lua standard library functions
    InitLuaLibrary();
    LuaException::EnableExceptions(m_pLuaVm->LuaState());
}

LuaBridge::~LuaBridge()
{
    lua_State *L = m_pLuaVm->LuaState();
    if (NULL != L) {
        lua_close(L);
    }
}

bool LuaBridge::LoadFile(const std::string &filePath)
{
    lua_State *L = m_pLuaVm->LuaState();
    int ret = luaL_dofile(L, filePath.c_str());
    if (ret != 0) {
        throw std::runtime_error("Lua loadfile:" + filePath + " failed, error:" + lua_tostring(L, -1));
    }
    return 0;
}

bool LuaBridge::LoadFile(const char *filePath)
{
    lua_State *L = m_pLuaVm->LuaState();
    int ret = luaL_dofile(L, filePath);
    if (ret != 0) {
        throw std::runtime_error("Lua loadfile:" + std::string(filePath) + " failed, error:" + lua_tostring(L, -1));
    }
    return 0;
}

void LuaBridge::InitLuaLibrary()
{
    lua_State *L = m_pLuaVm->LuaState();
    // initialize lua standard library functions
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_debug(L);
    luaopen_utf8(L);
    luaopen_package(L);
}

int LuaBridge::PushToLua()
{
    return 0;
}

template<typename T>
int LuaBridge::PushToLua(const T &t)
{
    lua_State *L = m_pLuaVm->LuaState();
    Stack<T>::push(L, t);
    return 1;
}

template<typename First, typename... Rest>
int LuaBridge::PushToLua(const First &first, const Rest &...rest)
{
    lua_State *L = m_pLuaVm->LuaState();
    Stack<First>::push(L, first);
    return PushToLua(rest...);
}

void LuaBridge::SafeBeginCall(const char *func)
{
    lua_State *L = m_pLuaVm->LuaState();
    //记录调用前的堆栈索引
    m_iTopIndex = lua_gettop(L);
    lua_getglobal(L, func);
}

template<typename R, int __>
R LuaBridge::SafeEndCall(const char *func, int nArg)
{
    lua_State *L = m_pLuaVm->LuaState();
    if (lua_pcall(L, nArg, 1, 0) != LUA_OK) {
        LuaHelper::DebugCallFuncErrorStack(L, func, lua_tostring(L, -1));
        //恢复调用前的堆栈索引
        lua_settop(L, m_iTopIndex);
        return 0;
    }
    else {
        try {
            R r = Stack<R>::get(L, -1, false);
            //恢复调用前的堆栈索引
            lua_settop(L, m_iTopIndex);
            return r;
        }
        catch (std::exception &e) {
            //恢复调用前的堆栈索引
            lua_settop(L, m_iTopIndex);
            LuaHelper::DebugCallFuncErrorStack(L, func, e.what());
            return 0;
        }
    }
}

template<int __>
void LuaBridge::SafeEndCall(const char *func, int nArg)
{
    lua_State *L = m_pLuaVm->LuaState();
    if (lua_pcall(L, nArg, 0, 0) != 0) {
        LuaHelper::DebugCallFuncErrorStack(L, func, lua_tostring(L, -1));
    }
    lua_settop(L, m_iTopIndex);
}

template<typename R, typename ...Args>
R LuaBridge::CallLuaFunc(const char *func, const Args... args)
{
    SafeBeginCall(func);
    PushToLua(args...);
    return SafeEndCall<R, 0>(func, sizeof...(args));
}

const char *LuaBridge::Call(const char *func, const char *sig, ...)
{
    lua_State *L = m_pLuaVm->LuaState();
    va_list vl;
    va_start(vl, sig);

    lua_getglobal(L, func);

    /* 壓入調用參數 */
    int narg = 0;
    while (*sig) {  /* push arguments */
        switch (*sig++) {
        case 'f':    /* 浮點數 */
        case 'e':    /* 浮點數 */
            lua_pushnumber(L, va_arg(vl, double));
            break;

        case 'i':    /* 整數 */
        case 'n':    /* 整數 */
        case 'd':    /* 整數 */
            lua_pushnumber(L, va_arg(vl, int));
            break;

        case 'b':    /* 布爾值 */
            lua_pushboolean(L, va_arg(vl, int));
            break;

        case 's':    /* 字元串 */
            lua_pushstring(L, va_arg(vl, char *));
            break;

        case 'S':    /* 字元串 */
        {
            int len = va_arg(vl, int);
            lua_pushlstring(L, va_arg(vl, char *), len);
        }
            break;

        case '>':
        case ':':goto L_LuaCall;

        default:
            //assert(("Lua call option is invalid!", false));
            //error(L, "invalid option (%c)", *(sig - 1));
            lua_pushnumber(L, 0);
        }
        narg++;
        luaL_checkstack(L, 1, "too many arguments");
    }

    L_LuaCall:
    int nres = static_cast<int>(strlen(sig));
    const char *sresult = NULL;
    if (lua_pcall(L, narg, nres, 0) != 0) {
        sresult = lua_tostring(L, -1);
        nres = 1;
    }
    else {
        // 取得返回值
        int index = -nres;
        while (*sig) {
            switch (*sig++) {
            case 'f':    /* 浮点数 float*/
            case 'e':    /* 浮点数 float*/
                *va_arg(vl, double *) = lua_tonumber(L, index);
                break;

            case 'i':    /* 整数 */
            case 'n':    /* 整数 */
            case 'd':    /* 整数 */
                *va_arg(vl, int *) = static_cast<int>(lua_tonumber(L, index));
                break;

            case 'b':    /* bool */
                *va_arg(vl, int *) = static_cast<int>(lua_toboolean(L, index));
                break;

            case 's':    /* string */
                *va_arg(vl, const char **) = lua_tostring(L, index);
                break;

            case 'S':    /* string */
            {
                size_t len;
                const char *str = lua_tolstring(L, index, &len);
                *va_arg(vl, int *) = static_cast<int>(len);
                *va_arg(vl, const char **) = str;
            }
                break;

            default:break;
            }
            index++;
        }
    }
    va_end(vl);

    lua_pop(L, nres);
    return sresult;
}

template<typename T>
int LuaBridge::PushSharedObjToLua(lua_State *L, std::shared_ptr<T> ptr)
{
    new(lua_newuserdata(L, sizeof(UserdataShared<std::shared_ptr<T>>))) UserdataShared<std::shared_ptr<T>>(ptr);
    lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetClassKey());
    LUA_ASSERT(L, lua_istable(L, -1), "UserdataSharedHelper::push<T*> lua_istable failed");
    lua_setmetatable(L, -2);
    return 1;
}

Namespace &LuaBridge::BeginNameSpace(char *name)
{
    m_namespace = GetGlobalNamespace().BeginNamespace(name);;
    return m_namespace;
}

void LuaBridge::EndNamespace()
{
    m_namespace.Reset();
    if (m_pLuaVm->GetStackSize() == 1) {
        throw std::logic_error("endNamespace () called on global namespace");
    }

    assert (m_pLuaVm->GetStackSize() > 1);
    m_pLuaVm->AddStackSize(-1);
    lua_State *L = m_pLuaVm->LuaState();
    lua_pop(L, 1);
}

Namespace &LuaBridge::GetGlobalNamespace()
{
    static Namespace g_namespace(m_pLuaVm);
    return g_namespace;
}

lua_State *LuaBridge::LuaState()
{
    return m_pLuaVm->LuaState();
}

Namespace &LuaBridge::CurNameSpace()
{
    return m_namespace;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE(luabridge, name)                                                \
    {                                                                                   \
        LuaBridge& _luabridge_ = luabridge;                                             \
        _luabridge_.BeginNameSpace(name);

#define END_NAMESPACE                                                                   \
        _luabridge_.EndNamespace();                                                     \
    }

#define BEGIN_CLASS_SHARED_OR_NOT(luabridge, ClassT, shared)                             \
    {                                                                                   \
        Class<ClassT> *pclasst = NULL;                                                  \
        if(!luabridge.CurNameSpace().IsValid())                                         \
        {                                                                               \
            Namespace& nameSpace = luabridge.GetGlobalNamespace();                      \
            pclasst = nameSpace.BeginClass<ClassT>(#ClassT,shared);        \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            Namespace& nameSpace = luabridge.CurNameSpace();                            \
            pclasst = nameSpace.BeginClass<ClassT>(#ClassT,shared);                     \
        }

/*
 * 通过BEGIN_CLASS注册的class只能在lua里创建类的对象obj,并且该obj只能在lua里访问,不能在c++中访问
 * (注册在lua中的c++函数除外),因为随时有可能被lua虚拟机gc掉
 * */
#define BEGIN_CLASS(luabridge, ClassT) BEGIN_CLASS_SHARED_OR_NOT(luabridge, ClassT,false)

/*
 * 通过BEGIN_SHARED_CLASS注册的class可以在c++中创建对象obj然后调用PushSharedObjToLua传入lua中,
 * 该obj在c++(注册在lua中的c++函数除外)和lua中共享,无需担心obj会被lua虚拟机gc掉,也可以在lua中创建
 * obj，注意在lua中创建的obj只在lua中访问
 * */
#define BEGIN_SHARED_CLASS(luabridge, ClassT) BEGIN_CLASS_SHARED_OR_NOT(luabridge, ClassT,true)

#define CLASS_ADD_CONSTRUCTOR(FT)                                                       \
        pclasst->AddConstructor<FT>();

#define CLASS_ADD_FUNC(name, func)                                                      \
        pclasst->AddFunction(name, func);

#define CLASS_ADD_STATIC_PROPERTY(name, data)                                           \
        pclasst->AddStaticProperty(name, data,true);

#define END_CLASS                                                                       \
        pclasst->EndClass();                                                            \
        delete pclasst;                                                                 \
        pclasst = 0;                                                                    \
    }

#define BEGIN_REGISTER_CFUNC(luabridge)                                                 \
    {                                                                                   \
        LuaBridge& _luabridge_ = luabridge;

#define REGISTER_CFUNC(name, func)                                                      \
        if(!_luabridge_.CurNameSpace().IsValid())                                       \
        {                                                                               \
            Namespace::AddGlobalCFunc(_luabridge_.LuaState(),name,func);                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
             _luabridge_.CurNameSpace().AddCFunction(name,func);                        \
        }

#define END_REGISTER_CFUNC                                                              \
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////
} //namespace luabridge

#endif //__LUA_BRIDGE_H__