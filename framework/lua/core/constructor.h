//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(¶Å¹ú³¬)<1139140929@qq.com>.  All rights reserved.
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

#ifndef __CONSTRUCTOR_H__
#define __CONSTRUCTOR_H__

#include "type_list.h"
#include "lua_stack.h"

namespace luabridge
{

/*
* Constructor generators.  These templates allow you to call operator new and
* pass the contents of a type/value list to the Constructor.  Like the
* function pointer containers, these are only defined up to 8 parameters.
*/

/** Constructor generators.

    These templates call operator new with the contents of a type/value
    list passed to the Constructor with up to 8 parameters. Two versions
    of call() are provided. One performs a regular new, the other performs
    a placement new.
*/
template<size_t NUM_PARAMS, class T, typename... ParamList>
struct Constructor
{
};

template<class T>
struct Constructor<0, T>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T;
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T;
    }
};

template<class T, class ... ParamList>
struct Constructor<1, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0));
    }
};

template<class T, class ... ParamList>
struct Constructor<2, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1));
    }
};

template<class T, class ... ParamList>
struct Constructor<3, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2));
    }
};

template<class T, class ... ParamList>
struct Constructor<4, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3));
    }
};

template<class T, class ... ParamList>
struct Constructor<5, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                     Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4));
    }
};

template<class T, class ... ParamList>
struct Constructor<6, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                     Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                     Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5));
    }
};

template<class T, class ... ParamList>
struct Constructor<7, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                     Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                     Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                     Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6));
    }
};

template<class T, class ... ParamList>
struct Constructor<8, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                     Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                     Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                     Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                     Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7));
    }
};

template<class T, class ... ParamList>
struct Constructor<9, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
                     Stack<LUA_PARAM_TYPE(1)>::get(L, startParam + 1),
                     Stack<LUA_PARAM_TYPE(2)>::get(L, startParam + 2),
                     Stack<LUA_PARAM_TYPE(3)>::get(L, startParam + 3),
                     Stack<LUA_PARAM_TYPE(4)>::get(L, startParam + 4),
                     Stack<LUA_PARAM_TYPE(5)>::get(L, startParam + 5),
                     Stack<LUA_PARAM_TYPE(6)>::get(L, startParam + 6),
                     Stack<LUA_PARAM_TYPE(7)>::get(L, startParam + 7),
                     Stack<LUA_PARAM_TYPE(8)>::get(L, startParam + 8));
    }
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<10, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<11, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<12, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<13, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<14, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<15, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<16, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<17, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<18, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<19, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

template<class T, class ... ParamList>
struct Constructor<20, T, ParamList...>
{
    static T *call(lua_State *L, int startParam)
    {
        return new T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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
    static T *call(lua_State *L, void *mem, int startParam)
    {
        return new(mem) T(Stack<LUA_PARAM_TYPE(0)>::get(L, startParam + 0),
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

} // namespace luabridge

#endif
