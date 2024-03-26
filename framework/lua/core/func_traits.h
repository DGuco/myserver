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

#ifndef __FUNCTION_TRAITS_H__
#define __FUNCTION_TRAITS_H__

#include <functional>
#include "type_list.h"
#include "constructor.h"
#include "caller.h"

namespace luabridge
{
//==============================================================================
/**
    Traits for function pointers.

    There are three types of functions: global, non-const member, and const
    member. These templates determine the type of function, which class type it
    belongs to if it is a class member, the const-ness if it is a member
    function, and the type information for the return value and argument list.

    Expansions are provided for functions with up to 8 parameters. This can be
    manually extended, or expanded to an arbitrary amount using C++11 features.
*/
template<class MemFn, class D = MemFn>
struct FuncTraits
{
    enum
    {
        arity = 0
    };
};


/**
 * 普通c函数R(...)
 * @tparam R
 * @tparam ParamList
 */
template<class R, class... ParamList>
struct FuncTraits<R (*)(ParamList...)>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = false;
    using DeclType = R (*)(ParamList...);
    using ReturnType = R;

    template<class T>
    static T* callnew(lua_State*L,void* mem,int startParam)
    {
        return Constructor<arity,T, ParamList...>::call(L,mem, startParam);
    }

    template<class T>
    static T* callnew(lua_State*L,int startParam)
    {
        return Constructor<arity,T, ParamList...>::call(L, startParam);
    }

    static R call(lua_State*L,const DeclType &fp,int startParam)
    {
        return doCall<R,DeclType,ParamList...>(L,fp,startParam);
    }
};

/**
 * Non-const member function
 * @tparam R
 * @tparam ParamList
 */
template<class T, class R, class... ParamList>
struct FuncTraits<R (T::*)(ParamList...)>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = false;
    using DeclType = R (T::*)(ParamList...);
    using ClassType = T;
    using ReturnType = R;

    static R call(lua_State*L,ClassType *obj,const DeclType &fp,int startParam)
    {
        return doCall<R,T,DeclType,ParamList...>(L,obj, fp,startParam);
    }
};

/**
 * Const member function
 * @tparam R
 * @tparam ParamList
 */
template<class T, class R,class... ParamList>
struct FuncTraits<R (T::*)(ParamList...) const>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = true;
    using DeclType = R (T::*)(ParamList...) const;
    using ClassType = T;
    using ReturnType = R;

    static R call(lua_State*L,const ClassType *obj,const DeclType &fp,int startParam)
    {
        return doCall<R,T,DeclType,ParamList...>(L,obj, fp,startParam);
    }
};


/**
 * std::function
 * @tparam R
 * @tparam ParamList
 */
template<class R, class... ParamList>
struct FuncTraits<std::function<R(ParamList...)>>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = false;
    static bool const isConstMemberFunction = false;
    using DeclType = std::function<R(ParamList...)>;
    using ReturnType = R;

    static ReturnType call(lua_State*L,DeclType &fn,int startParam)
    {
        return doCall<ReturnType,DeclType,ParamList...>(L,fn,startParam);
    }
};

template<class ReturnType,int startParam>
struct Invoke
{
    template<class Fn>
    static int run(lua_State *L, Fn &fn)
    {
        if (LuaHelper::GetParamCount(L) != FuncTraits<Fn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncTraits<Fn>::call(L,fn,startParam));
            return 1;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        //参数个数:对象指针+成员函数参数个
        if (LuaHelper::GetParamCount(L) != FuncTraits<MemFn>::arity + 1) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity + 1,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncTraits<MemFn>::call(L,object, fn,startParam));
            return 1;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }
};

template<int startParam>
struct Invoke<void,startParam>
{
    template<class Fn>
    static int run(lua_State *L, Fn &fn)
    {
        if (LuaHelper::GetParamCount(L) != FuncTraits<Fn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            FuncTraits<Fn>::call(L,fn,startParam);
            return 0;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        //参数个数:对象指针+成员函数参数个
        if (LuaHelper::GetParamCount(L) != (FuncTraits<MemFn>::arity + 1)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity + 1,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            FuncTraits<MemFn>::call(L,object, fn,startParam);
            return 0;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }
};

} // namespace luabridge

#endif