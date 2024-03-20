/******************************************************************************
* https://github.com/DGuco/luabridge
*
* Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.

* Copyright 2019, Dmitry Tarakanov
* Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
* Copyright 2007, Nathan Reed
* Copyright (C) 2004 Yong Lin.  All rights reserved.
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

#ifndef  __LUA_STACK_H__
#define  __LUA_STACK_H__

#include <cstring>
#include "lua_library.h"
#include "lua_helpers.h"
#include "user_data.h"
#include <string>

namespace luabridge
{
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Stack;

template<>
struct Stack<void>
{
    static void push(lua_State *L)
    {
    }
};

//------------------------------------------------------------------------------
/**
    Receive the lua_State* as an argument.
*/
template<>
struct Stack<lua_State *>
{
    static lua_State *get(lua_State *L, int, bool luaerror = true)
    {
        return L;
    }
};

//------------------------------------------------------------------------------
/**
    Push a lua_CFunction.
*/
template<>
struct Stack<lua_CFunction>
{
    static void push(lua_State *L, lua_CFunction f)
    {
        lua_pushcfunction(L, f);
    }

    static lua_CFunction get(lua_State *L, int index, bool luaerror = true)
    {
        return lua_tocfunction(L, index);
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `int`.
*/
template<>
struct Stack<int>
{
    static void push(lua_State *L, int value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    static int get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast<int>(lua_tointeger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned int`.
*/
template<>
struct Stack<unsigned int>
{
    static void push(lua_State *L, unsigned int value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static unsigned int get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast<unsigned int>(lua_tointeger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned char`.
*/
template<>
struct Stack<unsigned char>
{
    static void push(lua_State *L, unsigned char value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static unsigned char get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <unsigned char> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `short`.
*/
template<>
struct Stack<short>
{
    static void push(lua_State *L, short value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static short get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <short> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned short`.
*/
template<>
struct Stack<unsigned short>
{
    static void push(lua_State *L, unsigned short value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static unsigned short get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <unsigned short> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `long`.
*/
template<>
struct Stack<long>
{
    static void push(lua_State *L, long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static long get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <long> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned long`.
*/
template<>
struct Stack<unsigned long>
{
    static void push(lua_State *L, unsigned long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static unsigned long get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <unsigned long> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `long long`.
 */
template<>
struct Stack<long long>
{
    static void push(lua_State *L, long long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static long long get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <long long> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `unsigned long long`.
 */
template<>
struct Stack<unsigned long long>
{
    static void push(lua_State *L, unsigned long long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }
    static unsigned long long get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Integer(L, index), "CheckLuaArg_Integer failed", luaerror);
        return static_cast <unsigned long long> (luaL_checkinteger(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `float`.
*/
template<>
struct Stack<float>
{
    static void push(lua_State *L, float value)
    {
        lua_pushnumber(L, static_cast <lua_Number> (value));
    }

    static float get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Num(L, index), "CheckLuaArg_Num failed", luaerror);
        return static_cast<float>(lua_tonumber(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `double`.
*/
template<>
struct Stack<double>
{
    static void push(lua_State *L, double value)
    {
        lua_pushnumber(L, static_cast <lua_Number> (value));
    }

    static double get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Num(L, index), "CheckLuaArg_Num failed", luaerror);
        return static_cast<double>(lua_tonumber(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `bool`.
*/
template<>
struct Stack<bool>
{
    static void push(lua_State *L, bool value)
    {
        lua_pushboolean(L, value ? 1 : 0);
    }

    static bool get(lua_State *L, int index, bool luaerror = true)
    {
        return lua_toboolean(L, index) ? true : false;
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `char`.
*/
template<>
struct Stack<char>
{
    static void push(lua_State *L, char value)
    {
        lua_pushlstring(L, &value, 1);
    }

    static char get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Str(L, index), "CheckLuaArg_Str failed", luaerror);
        if (lua_isnil(L, index)) {
            return ' ';
        }
        return lua_tostring(L, index)[0];
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `const char*`.
*/
template<>
struct Stack<const char *>
{
    static void push(lua_State *L, char const *str)
    {
        if (str != 0)
            lua_pushstring(L, str);
        else
            lua_pushnil(L);
    }

    static const char *get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Str(L, index), "CheckLuaArg_Str failed", luaerror);
        if (lua_isnil(L, index)) {
            return "";
        }
        return lua_tostring(L, index);
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `const char*`.
*/
template<>
struct Stack<char *>
{
    static void push(lua_State *L, char const *str)
    {
        if (str != NULL)
            lua_pushstring(L, str);
        else
            lua_pushnil(L);
    }

    static char *get(lua_State *L, int index, bool luaerror = true)
    {
        //抛出c++异常
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Str(L, index), "CheckLuaArg_Str failed", luaerror);
        if (lua_isnil(L, index)) {
            return const_cast<char *>("");
        }
        return const_cast<char *>(lua_tostring(L, index));
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `BinaryStr`.
*/
template<>
struct Stack<BinaryStr>
{
    static void push(lua_State *L, BinaryStr pStr)
    {
        if (pStr.m_pStr != NULL && pStr.m_iLen > 0) {
            lua_pushlstring(L, pStr.m_pStr, pStr.m_iLen);
        }
    }

    BinaryStr get(lua_State *L, int index, bool luaerror = true)
    {
        //抛出c++异常
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Str(L, index), "CheckLuaArg_Str failed", luaerror);
        size_t len;
        const char *str = lua_tolstring(L, index, &len);
        return BinaryStr(str, len);
    }
};
//------------------------------------------------------------------------------
/**
    Stack specialization for `std::string`.
*/
template<>
struct Stack<std::string>
{
    static void push(lua_State *L, std::string const &str)
    {
        lua_pushlstring(L, str.data(), str.size());
    }

    static std::string get(lua_State *L, int index, bool luaerror = true)
    {
        LUA_ASSERT_EX(L, LuaHelper::CheckLuaArg_Str(L, index), "CheckLuaArg_Str failed", luaerror);
        if (lua_isnil(L, index)) {
            return "";
        }
        size_t len;
        const char *str = lua_tolstring(L, index, &len);
        return std::string(str, len);

//        size_t len;
//        if (lua_type(L, index) == LUA_TSTRING) {
//            const char *str = lua_tolstring(L, index, &len);
//            return std::string(str, len);
//        }
//
//        // Lua reference manual:
//        // If the value is a number, then lua_tolstring also changes the actual value in the stack to a string.
//        // (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
//        lua_pushvalue(L, index);
//        const char *str = lua_tolstring(L, -1, &len);
//        std::string string(str, len);
//        lua_pop(L, 1); // Pop the temporary string
//        return string;
    }
};

template<class T>
struct StackOpSelector<T &, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, T &value)
    {
        Stack<T>::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<const T &, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, const T &value)
    {
        Stack<T>::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<T *, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, T *value)
    {
        Stack<T>::push(L, *value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<const T *, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, const T *value)
    {
        Stack<T>::push(L, *value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<T &>
{
    typedef StackOpSelector<T &, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, T &value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<const T &>
{
    typedef StackOpSelector<const T &, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, const T &value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<T *>
{
    typedef StackOpSelector<T *, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, T *value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<const T *>
{
    typedef StackOpSelector<const T *, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, const T *value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = true)
    {
        return Helper::get(L, index, luaerror);
    }
};

} // namespace luabridge

#endif  //__LUA_STACK_H__