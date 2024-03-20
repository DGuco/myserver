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

  This file incorporates work covered by the following copyright and
  permission notice:  

    The Loki Library
    Copyright (c) 2001 by Andrei Alexandrescu
    This code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
        Patterns Applied". Copyright (c) 2001. Addison-Wesley.
    Permission to use, copy, modify, distribute and sell this software for any 
        purpose is hereby granted without fee, provided that the above copyright 
        notice appear in all copies and that both that copyright notice and this 
        permission notice appear in supporting documentation.
    The author or Addison-Welsey Longman make no representations about the 
        suitability of this software for any purpose. It is provided "as is" 
        without express or implied warranty.
*/
//==============================================================================

#ifndef __TYPE_LIST_H__
#define __TYPE_LIST_H__

#include <string>
#include <typeinfo>
#include "lua_library.h"

namespace luabridge
{

/**
  None type means void parameters or return value.
*/
typedef void None;

//template<typename Head, typename Tail = None>
//struct TypeList
//{
//    typedef Tail TailType;
//};
//
//template<class List>
//struct TypeListSize
//{
//    static const size_t value = TypeListSize<typename List::TailType>::value + 1;
//};
//
//template<>
//struct TypeListSize<None>
//{
//    static const size_t value = 0;
//};
//
//template <class... Params>
//struct MakeTypeList;
//
//template <class Param, class... Params>
//struct MakeTypeList <Param, Params...>
//{
//  using Result = TypeList <Param, typename MakeTypeList <Params...>::Result>;
//};
//
//template <>
//struct MakeTypeList <>
//{
//  using Result = None;
//};
//
//
///**
//  A TypeList with actual values.
//*/
//template<typename List>
//struct TypeListValues
//{
//    static std::string const tostring(bool)
//    {
//        return "";
//    }
//};
//
///**
//  TypeListValues recursive template definition.
//*/
//template<typename Head, typename Tail>
//struct TypeListValues<TypeList<Head, Tail> >
//{
//    Head hd;
//    TypeListValues<Tail> tl;
//
//    TypeListValues(Head hd_, TypeListValues<Tail> const &tl_)
//        : hd(hd_), tl(tl_)
//    {
//    }
//
//    static std::string tostring(bool comma = false)
//    {
//        std::string s;
//
//        if (comma)
//            s = ", ";
//
//        s = s + typeid(Head).name();
//
//        return s + TypeListValues<Tail>::tostring(true);
//    }
//};
//
//// Specializations of type/value list for head types that are references and
//// const-references.  We need to handle these specially since we can't count
//// on the referenced object hanging around for the lifetime of the list.
//
//template<typename Head, typename Tail>
//struct TypeListValues<TypeList<Head &, Tail> >
//{
//    Head hd;
//    TypeListValues<Tail> tl;
//
//    TypeListValues(Head &hd_, TypeListValues<Tail> const &tl_)
//        : hd(hd_), tl(tl_)
//    {
//    }
//
//    static std::string const tostring(bool comma = false)
//    {
//        std::string s;
//
//        if (comma)
//            s = ", ";
//
//        s = s + typeid(Head).name() + "&";
//
//        return s + TypeListValues<Tail>::tostring(true);
//    }
//};
//
//template<typename Head, typename Tail>
//struct TypeListValues<TypeList<Head const &, Tail> >
//{
//    Head hd;
//    TypeListValues<Tail> tl;
//
//    TypeListValues(Head const &hd_, const TypeListValues<Tail> &tl_)
//        : hd(hd_), tl(tl_)
//    {
//    }
//
//    static std::string const tostring(bool comma = false)
//    {
//        std::string s;
//
//        if (comma)
//            s = ", ";
//
//        s = s + typeid(Head).name() + " const&";
//
//        return s + TypeListValues<Tail>::tostring(true);
//    }
//};
//
////==============================================================================
///**
//  *Subclass of a TypeListValues constructable from the Lua stack.
//  *cfunction 从lua stack获取参数列表
//*/
//
//template<typename List, int Start = 1>
//struct ArgList
//{
//};
//
//template<int Start>
//struct ArgList<None, Start>: public TypeListValues<None>
//{
//    ArgList(lua_State *)
//    {
//    }
//};
//
//template<typename Head, typename Tail, int Start>
//struct ArgList<TypeList<Head, Tail>, Start>
//    : public TypeListValues<TypeList<Head, Tail> >
//{
//    ArgList(lua_State *L)
//        : TypeListValues<TypeList<Head, Tail> >(Stack<Head>::get(L, Start),
//                                                ArgList<Tail, Start + 1>(L))
//    {
//    }
//};

//每个参数的类型
template<class... ParamList>
struct ArgTypeList
{
    enum
    {
        //参数个数
        arity = sizeof...(ParamList)
    };
    using ParamTypeElement = typename std::tuple<ParamList...>;
    //每个参数的类型
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, ParamTypeElement>::type;
    };
};

//获取指定函数参数指定位置的参数类型
#define  LUA_PARAM_TYPE(n) typename ArgTypeList<ParamList...>::template args<n>::type

} // namespace luabridge

#endif