//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.

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

#ifndef __LUA_CALLER_H__
#define __LUA_CALLER_H__

#include <functional>
#include "type_list.h"
#include "constructor.h"

namespace luabridge
{

template<size_t NUM_PARAMS/*参数个数*/, class ReturnType/*返回类型*/, class... ParamList/*参数列表*/>
struct Caller;

/**
 * 无参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<0, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn();
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)();
    }
};

/**
 * 1参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<1, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0));
    }
};

/**
 * 2参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<2, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1));
    }
};

/**
 * 3参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<3, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2));
    }
};

/**
 * 4参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<4, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3));
    }
};

/**
 * 5参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<5, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4));
    }
};

/**
 * 6参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<6, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5));
    }
};

/**
 * 7参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<7, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6));
    }
};

/**
 * 8参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<8, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7));
    }
};

/**
 * 9参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<9, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8));
    }
};

/**
 * 10参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<10, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9));
    }
};

/**
 * 11参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<11, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10));
    }
};

/**
 * 12参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<12, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11));
    }
};

/**
 * 13参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<13, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12));
    }
};

/**
 * 14参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<14, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13));
    }
};

/**
 * 15参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<15, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14));
    }
};

/**
 * 16参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<16, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                  Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                          Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15));
    }
};

/**
 * 17参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<17, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                  Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                  Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                          Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                          Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16));
    }
};

/**
 * 18参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<18, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                  Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                  Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                  Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                          Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                          Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                          Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17));
    }
};

/**
 * 19参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<19, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                  Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                  Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                  Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17),
                  Stack<LUA_PARAM_TYPE(18)>::get(L, startParam + 18));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                          Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                          Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                          Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17),
                          Stack<LUA_PARAM_TYPE(18)>::get(L, startParam + 18));
    }
};

/**
 * 20参数
 * @tparam ReturnType
 * @tparam ParamList
 */
template<class ReturnType, class... ParamList>
struct Caller<20, ReturnType, ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State *L, Fn &fn, int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                  Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                  Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                  Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17),
                  Stack<LUA_PARAM_TYPE(18)>::get(L, startParam + 18),
                  Stack<LUA_PARAM_TYPE(19)>::get(L, startParam + 19));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State *L, T *obj, MemFn &fn, int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L, startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L, startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L, startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L, startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L, startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L, startParam + 14),
                          Stack<LUA_PARAM_TYPE(15)>::get(L, startParam + 15),
                          Stack<LUA_PARAM_TYPE(16)>::get(L, startParam + 16),
                          Stack<LUA_PARAM_TYPE(17)>::get(L, startParam + 17),
                          Stack<LUA_PARAM_TYPE(18)>::get(L, startParam + 18),
                          Stack<LUA_PARAM_TYPE(19)>::get(L, startParam + 19));
    }
};

/**
 * call cfunction
 * @tparam ReturnType
 * @tparam Fn
 * @tparam ParamList
 * @param L
 * @param fn
 * @param startParam
 * @return
 */
template<class ReturnType, class Fn, class... ParamList>
ReturnType doCall(lua_State *L, const Fn &fn, int startParam)
{
    return Caller<ArgTypeList<ParamList...>::arity, ReturnType, ParamList...>::f(L, fn, startParam);
}

/**
 * call class memfunc
 * @tparam ReturnType
 * @tparam T
 * @tparam MemFn
 * @tparam ParamList
 * @param L
 * @param obj
 * @param fn
 * @param startParam
 * @return
 */
template<class ReturnType, class T, class MemFn, class... ParamList>
static ReturnType doCall(lua_State *L, T *obj, const MemFn &fn, int startParam)
{
    return Caller<ArgTypeList<ParamList...>::arity, ReturnType, ParamList...>::f(L, obj, fn, startParam);
}

}
#endif //__LUA_CALLER_H__
