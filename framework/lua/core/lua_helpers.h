//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2007, Nathan Reed

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#ifndef __LUA_HELPERS_H__
#define __LUA_HELPERS_H__

#include "lua_library.h"
#include "lua_exception.h"
#include <cassert>
#include <cstring>
#include <iostream>

namespace luabridge
{

/**
 * 
 * */
struct BinaryStr
{
    BinaryStr(const char *pdata, int len)
    {
        m_pStr = pdata;
        m_iLen = len;
    }

    const char *m_pStr;
    int m_iLen;
};

class LuaHelper
{
public:
    /**
     * 检测lua 堆栈中的参数类型
     * @param Index
     * @param isLuaError 如果类型错误是否抛出lua异常
     * @return
     */
    static bool CheckLuaArg_Num(lua_State *L, int Index);
    static bool CheckLuaArg_Integer(lua_State *L, int Index);
    static bool CheckLuaArg_Str(lua_State *L, int Index);
    /**
     * print lua stack info 打印lua 堆栈信息
     * @param L
     */
    static void LuaStackInfo(lua_State *L);
    /**
     * Lua Asset,if asset failed  throw a lua lua error
     * @param condition
     * @param argindex
     * @param err_msg
     * @param luaerror 当在无保护环境下发生错误并且没有调用lua_atpanic(L, ThrowAtPanic)设置相应的异常回调lua会调用abort退出,
     * luaerror是否为true取决于调用代码是否在保护环境下运行(在lua_pcall执行逻辑中),如果你没有调用lua_atpanic拦截luaerror并且
     * 在非保护环境下抛出luaerror程序会主动退出
     * @return never return when assert failed
     */
    static int
    LuaAssert(lua_State *L, bool condition, const char *file, int line, const char *err_msg, bool luaerror = true);

    /**
     * param count
     * @return
     */
    static int GetParamCount(lua_State *L);
    /**
     * dbug lua error info 打印lua 错误日志message
     * @param FunName
     * @param Msg
     */
    static void DebugCallFuncErrorStack(lua_State *L, const char *FunName, const char *Msg);
    static void DefaultDebugLuaErrorInfo(const char *Msg);

    static int GetStackLen(lua_State *L, int idx);

    /** Get a table value, by passing metamethods.
    */
    static void RawGetField(lua_State *L, int index, char const *key);

    /** Set a table value, by passing metamethods.
    */
    static void RawSetField(lua_State *L, int index, char const *key);
    /** Returns true if the value is a full userdata (not light).
    */
    static bool IsFullUserData(lua_State *L, int index);

    /** Test lua_State objects for global equality.
        This can determine if two different lua_State objects really point
        to the same global state, such as when using coroutines.
        @note This is used for assertions.
    */
    static bool EqualStates(lua_State *L1, lua_State *L2);

    /**
        Wrapper for lua_pcall that throws.
    */
    static void Pcall(lua_State *L, int nargs = 0, int nresults = 0, int msgh = 0);
    /**
     * 输出lua table 内容
     * @param L
     * @param index
     * @param stream
     * @param level
     */
    static void
    DumpTable(lua_State *L, int index, std::ostream &stream, unsigned int depth = 1, unsigned int level = 0);
private:
    static void PutIndent(std::ostream &stream, unsigned int level);
    static void DumpState(lua_State *L, std::ostream &stream, unsigned int depth, unsigned int level);
    static void DumpValue(lua_State *L, int index, std::ostream &stream, unsigned int depth, unsigned int level);
};

void LuaHelper::LuaStackInfo(lua_State *L)
{
    LuaHelper::DefaultDebugLuaErrorInfo(
        "==========================Lua stack info start=====================================");
    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    char Msg[512] = {0};
    for (int i = 0;; i++) {
        if (lua_getstack(L, i, &trouble_info) == 0)
            break;
        lua_getinfo(L, "Snl", &trouble_info);
        snprintf(Msg, 512 - 1, "name[%s] what[%s] short_src[%s] linedefined[%d] currentline[%d]",
                 trouble_info.name,
                 trouble_info.what,
                 trouble_info.short_src,
                 trouble_info.linedefined,
                 trouble_info.currentline);
        LuaHelper::DefaultDebugLuaErrorInfo(Msg);
    }
    LuaHelper::DefaultDebugLuaErrorInfo(
        "========================== Lua stack info end =====================================");

}

bool LuaHelper::CheckLuaArg_Num(lua_State *L, int Index)
{
    if (lua_isnumber(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (NULL == trouble_info.name) {
        trouble_info.name = "?";
    }

    char Msg[128] = {0};
    if (lua_isnil(L, Index)) {
        snprintf(Msg, 128, "Lua function[%s], arg[%d] is null", trouble_info.name, Index);
    }
    else {
        snprintf(Msg, 128, "Lua function[%s], arg[%d] type error not number", trouble_info.name, Index);
    }
    LuaHelper::DefaultDebugLuaErrorInfo(Msg);
    LuaStackInfo(L);
    return false;
}

bool LuaHelper::CheckLuaArg_Integer(lua_State *L, int Index)
{
    if (lua_isinteger(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (NULL == trouble_info.name) {
        trouble_info.name = "?";
    }

    char Msg[128] = {0};
    if (lua_isnil(L, Index)) {
        snprintf(Msg, 128, "Lua function[%s], arg[%d] is null", trouble_info.name, Index);
    }
    else {
        snprintf(Msg, 128, "Lua function[%s], arg[%d] type error not integer", trouble_info.name, Index);
    }
    LuaHelper::DefaultDebugLuaErrorInfo(Msg);

    LuaStackInfo(L);
    return false;
}

bool LuaHelper::CheckLuaArg_Str(lua_State *L, int Index)
{
    if (lua_isstring(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    if (lua_getstack(L, 1, &trouble_info) || lua_getstack(L, 0, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (NULL == trouble_info.name) {
        trouble_info.name = "?";
    }

    char Msg[512] = {0};
    if (lua_isnil(L, Index)) {
        snprintf(Msg, 512 - 1, "Lua function[%s], arg[%d] is null \n", trouble_info.name, Index);
    }
    else {
        snprintf(Msg, 512 - 1, "Lua function[%s], arg[%d] type error not str \n", trouble_info.name, Index);
    }
    LuaHelper::DefaultDebugLuaErrorInfo(Msg);
    LuaStackInfo(L);
    return false;
}

int LuaHelper::LuaAssert(lua_State *L, bool condition, const char *file, int line, const char *err_msg, bool luaerror)
{
    if (!condition) {
        lua_Debug ar;
        memset(&ar, 0, sizeof(lua_Debug));
        if (lua_getstack(L, 0, &ar)) {
            lua_getinfo(L, "n", &ar);
        }
        if (NULL == ar.name) {
            ar.name = "?";
        }
        if (NULL == ar.namewhat) {
            ar.namewhat = "?";
        }
        /*
         * https://blog.csdn.net/weixin_33682719/article/details/92861195
         *@param check:
         * lua_toxxx和luaL_checkxxx的区别，这两个函数都是从lua栈上获取一个值，但是在检查到类型不符时候，lua_toxxx只是返回null或者默认值；
         * 而luaL_check则是会抛出一个异常，下面的代码不会再继续执行；这里就需要注意了，lua里面使用的异常并不是c++的异常，只是使用了c的setjump和longjump来实现到恢复点的跳转，
         * 所以并不会有C++所期望的栈的展开操作，所以在C++里面看来是异常安全的代码，此时也是“不安全”的，也不能保证异常安全，比如
         * Function1(lua_state state)
         * {
             TestClass tmp();
             luaL_checkstring(state,1);
         * }
         * 当上面的luaL_checkstring出现异常时候，TestClass的析构函数并不会被调用，假如你需要在析构函数里面释放一些资源，可能会导致资源泄露、锁忘记释放等问题。
         * 所以在使用luaL_checkxxx时候，需要很小心，在luaL_checkxxx之前尽量不要申请一些需要之后释放的资源，尤其是加锁函数,智能指针和auto锁也不能正常工作。
         * 如果用g++重新编译lua源码不会有问题
         **/
#ifdef COMPILE_LUA_WITH_CXX
        if (luaerror) {
            std::string filename(file);
            if (NULL != file) {
                return luaL_error(L,
                                  "(%s:%d assert fail) %s `%s' (%s)",
                                  file,
                                  line,
                                  ar.namewhat,
                                  ar.name,
                                  err_msg);
            }
            else {
                return luaL_error(L, "(%s:%d assert fail) %s `%s' (%s)", " ? ", line, ar.namewhat, ar.name, err_msg);
            }
        }
        else {
            char Msg[512] = {'\0'};
            if (NULL != file) {
                snprintf(Msg,
                         512 - 1,
                         "(%s:%d) assert fail: %s `%s' (%s)",
                         file,
                         line,
                         ar.namewhat,
                         ar.name,
                         err_msg);
            }
            else {
                snprintf(Msg, 512 - 1, "(%s:%d) assert fail: %s `%s' (%s)", "?", line, ar.namewhat, ar.name, err_msg);
            }
            throw std::logic_error(Msg);
        }
#else
        char Msg[512] = {0};
        if (NULL != file)
        {
            snprintf(Msg,512 - 1,"(%s:%d) assert fail: %s `%s' (%s)",file,line,ar.namewhat, ar.name, err_msg);
        }else
        {
            snprintf(Msg,512 - 1,"(%s:%d) assert fail: %s `%s' (%s)","?",line,ar.namewhat, ar.name, err_msg);
        }
        throw std::logic_error(Msg);
#endif
    }
    else {
        return 1;
    }
}

int LuaHelper::GetParamCount(lua_State *L)
{
    return lua_gettop(L);
}

void LuaHelper::DebugCallFuncErrorStack(lua_State *L, const char *FunName, const char *Msg)
{
    LuaStackInfo(L);
    printf("CallLuaFunc fun:[%s] failed,msg:[%s]\n", FunName, Msg);
}

void LuaHelper::DefaultDebugLuaErrorInfo(const char *Msg)
{
    printf("%s\n", Msg);
}

int LuaHelper::GetStackLen(lua_State *L, int idx)
{
    lua_len(L, idx);
    int len = int(luaL_checknumber(L, -1));
    lua_pop (L, 1);
    return len;
}

void LuaHelper::RawGetField(lua_State *L, int index, char const *key)
{
    LuaHelper::LuaAssert(L, lua_istable(L, index), __FILE__, __LINE__, "lua_istable(L, index)", false);
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_rawget(L, index);
}

void LuaHelper::RawSetField(lua_State *L, int index, char const *key)
{
    LuaHelper::LuaAssert(L, lua_istable(L, index), __FILE__, __LINE__, "lua_istable(L, index)", false);
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

bool LuaHelper::IsFullUserData(lua_State *L, int index)
{
    return lua_isuserdata(L, index) && !lua_islightuserdata (L, index);
}

bool LuaHelper::EqualStates(lua_State *L1, lua_State *L2)
{
    return lua_topointer(L1, LUA_REGISTRYINDEX) ==
        lua_topointer(L2, LUA_REGISTRYINDEX);
}

void LuaHelper::Pcall(lua_State *L, int nargs, int nresults, int msgh)
{
    int code = lua_pcall (L, nargs, nresults, msgh);
    if (code != LUA_OK)
        throw (LuaException(L, code));
}

void LuaHelper::PutIndent(std::ostream &stream, unsigned level)
{
    for (unsigned i = 0; i < level; ++i) {
        stream << "  ";
    }
}

void LuaHelper::DumpValue(lua_State *L, int index, std::ostream &stream, unsigned int depth, unsigned level)
{
    const int type = lua_type(L, index);
    switch (type) {
    case LUA_TNIL:stream << "nil";
        break;

    case LUA_TBOOLEAN:stream << (lua_toboolean(L, index) ? "true" : "false");
        break;

    case LUA_TNUMBER:stream << lua_tonumber(L, index);
        break;

    case LUA_TSTRING:stream << '"' << lua_tostring(L, index) << '"';
        break;

    case LUA_TFUNCTION:
        if (lua_iscfunction(L, index)) {
            stream << "cfunction@" << lua_topointer(L, index);
        }
        else {
            stream << "function@" << lua_topointer(L, index);
        }
        break;

    case LUA_TTHREAD:stream << "thread@" << lua_tothread(L, index);
        break;

    case LUA_TLIGHTUSERDATA:stream << "lightuserdata@" << lua_touserdata(L, index);
        break;

    case LUA_TTABLE:DumpTable(L, index, stream, depth, level);
        break;

    case LUA_TUSERDATA:stream << "userdata@" << lua_touserdata(L, index);
        break;

    default:stream << lua_typename(L, type);;
        break;
    }
}

void LuaHelper::DumpTable(lua_State *L, int index, std::ostream &stream, unsigned depth, unsigned int level)
{
    stream << "table@" << lua_topointer(L, index);
    if (level >= depth) {
        return;
    }

    index = lua_absindex(L, index);
    stream << " {";
    lua_pushnil(L); // Initial key
    while (lua_next(L, index)) {
        stream << "\n";
        PutIndent(stream, level + 1);
        DumpValue(L, -2, stream, depth, level + 1); // Key
        stream << ": ";
        DumpValue(L, -1, stream, depth, level + 1); // Value
        lua_pop(L, 1); // Value
    }
    PutIndent(stream, level);
    if (level == 0) {
        stream << "\n}\n";
    }
    else {
        stream << "\n";
        PutIndent(stream, level);
        stream << "}";
    }
}

void LuaHelper::DumpState(lua_State *L, std::ostream &stream, unsigned int depth, unsigned int level)
{
    int top = lua_gettop(L);
    for (int i = 1; i <= top; ++i) {
        stream << "stack #" << i << ": ";
        DumpValue(L, i, stream, depth, level);
        stream << "\n";
    }
}

#define  LUA_ASSERT(L, con, msg) LuaHelper::LuaAssert(L,con,__FILE__,__LINE__,msg);
#define  LUA_ASSERT_EX(L, con, msg, luaerror) LuaHelper::LuaAssert(L,con,__FILE__,__LINE__,msg,luaerror);

} // namespace luabridge

#endif