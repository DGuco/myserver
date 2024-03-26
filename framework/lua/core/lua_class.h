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
#ifndef __LUA_CLASS_H__
#define __LUA_CLASS_H__

#include "lua_vm.h"
#include <functional>
#include <stdexcept>
#include <string>
#include <cassert>
#include "lua_vm.h"
#include "security.h"
#include "type_traits.h"
#include "lua_functions.h"
#include "class_key.h"
#include "lua_exception.h"
#include "lua_helpers.h"
#include "constructor.h"

namespace luabridge
{
//============================================================================
/**
  Factored base to reduce template instantiations.
*/
class ClassBase
{
public:
    explicit ClassBase(const char *name, LuaVm *puaVm, bool shared)
        : className(name), m_pLuaVm(puaVm), m_bshared(shared)
    {
    }

protected:
    //--------------------------------------------------------------------------
    /**
      Create the const table.
    */
    void CreateConstTable(const char *name, bool trueConst = true)
    {
        lua_State *L = m_pLuaVm->LuaState();
        std::string type_name = std::string(trueConst ? "const_" : "") + name;

        // Stack: namespace table (ns) //栈状态lua_gettop(L) == n + 1:栈状态:ns
        lua_newtable(L); // Stack: ns, const table (co)   栈状态lua_gettop(L) == n + 2:ns=>co
        lua_pushvalue(L, -1); // Stack: ns, co, co 栈状态lua_gettop(L) == n + 3:ns=>co=>co
        lua_setmetatable(L, -2); // co.__metatable = co. 栈状态lua_gettop(L) == n + 2:ns=>co
        lua_pushstring(L, type_name.c_str()); // const table name 栈状态lua_gettop(L)== n + 3:ns=>co=>type_name
        lua_rawsetp(L, -2, GetTypeKey()); // co [typeKey] = type_name. 栈状态lua_gettop(L)== n + 2:ns=>co

        /**
         *https://zilongshanren.com/post/bind-a-simple-cpp-class-in-lua/
         *https://blog.csdn.net/qiuwen_521/article/details/107855867
         *用Lua里面的面向对象的方式来访问。
         *local s = cc.create()
         *s:setName("zilongshanren")
         *s:setAge(20)
         *s:print()
         *s:setName(xx)就等价于s.setName(s,xx)，此时我们只需要给s提供一个metatable,并且给这个metatable设置一个key为"__index"，
         *value等于它本身的metatable。最后，只需要把之前Student类的一些方法添加到这个metatable里面就可以了,或者key为"__index"，
         *value位一个function(t,k)类型的函数，函数中可以根据k获取对应的类的方法,这里的实现是后者
        **/
        lua_pushcfunction(L, &CFunc::IndexMetaMethod); //栈状态lua_gettop(L)== n + 3:ns=>co=>IndexMetaMethod
        //co.__index = &CFunc::IndexMetaMethod 栈状态lua_gettop(L)== n + 2:ns=>co
        LuaHelper::RawSetField(L, -2, "__index");

        lua_pushcfunction(L,
                          &CFunc::NewindexObjectMetaMethod); //栈状态lua_gettop(L)== n + 3:ns=>co=>NewindexObjectMetaMethod
        //co.__newindex = &CFunc::NewindexObjectMetaMethod 栈状态lua_gettop(L)== n + 2:ns=>co
        LuaHelper::RawSetField(L, -2, "__newindex");

        lua_newtable(L); //propget table(gt) 栈状态lua_gettop(L)== n + 3:ns=>co=>gt
        lua_rawsetp(L, -2, GetPropgetKey());//co[progetkey] = gt 栈状态lua_gettop(L)== n + 2:ns=>co

        if (Security::hideMetatables()) {
            lua_pushnil(L);  //栈状态lua_gettop(L)== n + 3:ns=>co=>nil
            LuaHelper::RawSetField(L, -2, "__metatable"); //co.__metatable = nil 栈状态lua_gettop(L)== n + 2:ns=>co
        }
        //now 栈状态lua_gettop(L)== n + 2:ns=>co
    }

    //--------------------------------------------------------------------------
    /**
      Create the class table.

      The Lua stack should have the const table on top.
    */
    void CreateClassTable(char const *name)
    {
        lua_State *L = m_pLuaVm->LuaState();
        // Stack: namespace table (ns), const table (co)
        // Stack 栈状态lua_gettop(L) == n + 2:ns=>co

        // Class table is the same as const table except the propset table
        CreateConstTable(name, false); // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl

        lua_newtable(L); // propset table (ps)  Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>ps
        lua_rawsetp(L, -2, GetPropsetKey()); // cl [propsetKey] = ps. Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl

        lua_pushvalue(L, -2); // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl=>co
        lua_rawsetp(L, -2, GetConstKey()); // cl [constKey] = co. Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl=>co

        lua_pushvalue(L, -1); // Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>cl
        lua_rawsetp(L, -3, GetClassKey()); // co [classKey] = cl.Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
        //now 栈状态lua_gettop(L)== n + 3:ns=>co=>cl
    }

    //--------------------------------------------------------------------------
    /**
      Create the static table.
    */
    void CreateStaticTable(char const *name)
    {
        lua_State *L = m_pLuaVm->LuaState();
        // Stack: namespace table (ns), const table (co), class table (cl)
        // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
        lua_newtable(L); //visible static table (vst) Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        std::string type_name = std::string("static_") + name;
        lua_pushstring(L, type_name.c_str()); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>type_name
        lua_rawsetp(L, -2, GetTypeKey()); //st.typekey = type_name Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_pushcfunction(L, &CFunc::IndexMetaMethod); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>IndexMetaMethod
        //st.__index = IndexMetaMethod,Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        LuaHelper::RawSetField(L, -2, "__index");

        lua_pushcfunction(L,
                          &CFunc::NewindexStaticMetaMethod); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>NewindexStaticMetaMethod
        //st.__newindex = NewindexStaticMetaMethod,Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        LuaHelper::RawSetField(L, -2, "__newindex");

        lua_newtable(L); // proget table (pg) Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>pg
        lua_rawsetp(L, -2, GetPropgetKey()); // st [propgetKey] = pg. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_newtable(L); // Stack: ns, co, cl, st, propset table (ps) Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>ps
        lua_rawsetp(L, -2, GetPropsetKey()); // st [propsetKey] = pg. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_pushvalue(L, -2); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
        lua_rawsetp(L, -2, GetClassKey()); // st [classKey] = cl.  Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

        if (Security::hideMetatables()) {
            lua_pushnil(L); // Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>nil
            LuaHelper::RawSetField(L,
                                   -2,
                                   "__metatable"); //st.__metatable = nil   Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        }
    }

    //==========================================================================
    /**
      lua_CFunction to construct a class object wrapped in a container.
    */
    template<class MemFn, class C>
    static int CtorContainerProxy(lua_State *L)
    {
        typedef typename ContainerTraits<C>::Type T;
        T *const p = FuncTraits<MemFn>::template callnew<T>(L, 2);
        UserdataSharedHelper<C, false>::push(L, p);
        return 1;
    }

    //--------------------------------------------------------------------------
    /**
      lua_CFunction to construct a class object in-Place in the userdata.
    */
    template<class MemFn, class T>
    static int CtorPlacementProxy(lua_State *L)
    {
        UserdataValue<T> *value = UserdataValue<T>::Place(L);
        FuncTraits<MemFn>::template callnew<T>(L, value->getObject(), 2);
        value->commit();
        return 1;
    }

    //注册类的成员前检查lua栈上的table是否合法
    void AssertStackState(bool luaerror = false) const
    {
        lua_State *L = m_pLuaVm->LuaState();
        // Stack: const table (co), class table (cl), static table (st)
        lua_rawgetp(L, -1, GetTypeKey()); // Stack: rt, registry type

        LUA_ASSERT_EX(L, lua_isstring(L, -1), "AssertStackState lua_isstring(L,-1)", luaerror);
        std::string name = std::string(lua_tostring(L, -1));
        lua_pop(L, 1);
        std::string rightName = std::string("static_") + className;
        LUA_ASSERT_EX(L,
                      name == rightName,
                      (std::string("table type name wrong,rightName = ") + rightName + std::string(",curname = ")
                          + name).c_str(),
                      luaerror);

        LUA_ASSERT_EX(L, lua_istable(L, -2), "AssertStackState lua_istable(L,-1)", luaerror);
        lua_rawgetp(L, -2, GetTypeKey()); // Stack: rt, registry type

        LUA_ASSERT_EX(L, lua_isstring(L, -1), "AssertStackState lua_isstring(L,-1)", luaerror);
        name = std::string(lua_tostring(L, -1));
        lua_pop(L, 1);
        rightName = className;
        LUA_ASSERT_EX(L,
                      name == rightName,
                      (std::string("table type name wrong,rightName = ") + rightName + std::string(",curname = ")
                          + name).c_str(),
                      luaerror);

        LUA_ASSERT_EX(L, lua_istable(L, -3), "AssertStackState lua_istable(L,-3)", luaerror);
        lua_rawgetp(L, -3, GetTypeKey()); // Stack: rt, registry type
        name = std::string(lua_tostring(L, -1));
        lua_pop(L, 1);
        rightName = std::string("const_") + className;
        LUA_ASSERT_EX(L,
                      name == rightName,
                      (std::string("table type name wrong,rightName = ") + rightName + std::string(",curname = ")
                          + name).c_str(),
                      luaerror);
    }
protected:
    std::string className;
    LuaVm *m_pLuaVm;
    bool m_bshared;
};

//============================================================================
//
// Class
//
//============================================================================
/**
  Provides a class registration in a lua_State.

  After construction the Lua stack holds these objects:
    -1 static table
    -2 class table
    -3 const table
    -4 enclosing namespace table
*/
template<class T>
class Class: public ClassBase
{
public:
    //==========================================================================
    /**
      Register a new class or add to an existing class registration.
      构造函数完成后
      vst(visible static table) = {
          __metatable = st,
      }
      co(const table) = {
          __metatable = co,
          typekey = const_name,
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __gc = &CFunc::GCMetaMethod<T>,
          propgetKey = {table}(通过addProperty注册普通成员变量的get方法会注册在这里),
          classKey = cl,
          func1_name = func1,(普通成员函数1 会被注册在这个表里),
          func2_name = func12,(普通成员函数2 会被注册在这个表里),
      }

      cl(class table) = {
          __metatable = cl,
          typekey = name,
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __gc = &CFunc::GCMetaMethod<T>,
          propgetKey = {}(table)(通过addProperty注册普通成员变量的get方法也会注册在这里),
          propsetKey = {}(table)(通过addProperty注册普通成员变量的set方法也会注册在这里),,
          constKey = co,
          func1_name = func1,(普通成员函数1 也会被注册在这个表里),
          func2_name = func12,(普通成员函数2 也会被注册在这个表里),
      }

      st(static table) = {
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __call = class Constructor(class Constructor 会被注册在这个表里),
          propgetKey = {}(table)(通过addStaticProperty注册静态成员变量的get方法会注册在这里),
          propsetKey = {}(table)(通过addStaticProperty注册静态成员变量的set方法会注册在这里),
          classKey = cl,
          static_func1_name = func1,(static成员函数1 也会被注册在这个表里),
          static_func2_name = func12,(static成员函数2 也会被注册在这个表里),
      }

      _G = {
          name = vst;
      }

      registry = {
          ClassInfo<T>_static_key = st,
          ClassInfo<T>_class_key = cl,
          ClassInfo<T>_const_key = co,
      }
     **/
    Class(char const *name, LuaVm *luaVm, bool shared)
        : ClassBase(name, luaVm, shared)
    {
        lua_State *L = m_pLuaVm->LuaState();
        //栈顶是否是表(_G)
        LUA_ASSERT_EX(L, lua_istable(L, -1), "lua_istable(L, -1)", false); //栈状态lua_gettop(L)== n + 1:ns
        //尝试find类的metadata表_G[name]
        LuaHelper::RawGetField(L, -1, name); // st = _G[name] 栈状态lua_gettop(L)== n + 2:ns=>st|nil

        //表不存在create it
        if (lua_isnil(L, -1)) // Stack: ns, nil 栈状态:ns=>nil
        {
            //弹出nil值
            lua_pop(L, 1); // Stack: ns 栈状态ua_gettop(L)== n + 1:ns
            //create类的const metadata表(co),and set co.__metatable = co
            CreateConstTable(name);
            //now 栈状态lua_gettop(L)== n + 2:ns=>co
            //注册gc元方法
            lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); // 栈状态lua_gettop(L)== n + 3:ns=>co=>gcfun
            //co.__gc = gcfun 即co.__metatable.__gc = gcfun 栈状态lua_gettop(L)== n + 2:ns=>co
            LuaHelper::RawSetField(L, -2, "__gc");
            m_pLuaVm->AddStackSize(1);

            //create类的非const metadata表 and set cl.__metatable = cl
            CreateClassTable(name); //class table (cl) stack栈状态lua_gettop(L)== n + 3:ns=>co=>cl
            //now 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
            //注册gc元方法
            lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); //gcfun stack栈状态lua_gettop(L)== n + 4:ns=>co=>cl=>gcfun
            //cl.__gc = gcfun 即 cl.__metatable.__gc = gcfun stack栈状态lua_gettop(L)== n + 3:ns=>co=>cl
            LuaHelper::RawSetField(L, -2, "__gc");
            m_pLuaVm->AddStackSize(1);

            //create类的static metadata表 and set st.__metatable = st
            CreateStaticTable(name); // Stack: ns, co, cl, st 栈状态:ns=>co=>cl=>st
            //now 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            m_pLuaVm->AddStackSize(1);

            //Map T back to its tables.
            //把st元表作一个副本压栈。
            lua_pushvalue(L, -1); // 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>st
            //把static metadata表插入registry表
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetStaticKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            //把cl元表作一个副本压栈。
            lua_pushvalue(L, -2); // stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
            //把metadata表插入registry表
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetClassKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            //把co元表作一个副本压栈。
            lua_pushvalue(L, -3); // 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>co
            //把const metadata表插入registry表
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetConstKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            lua_newtable(L);             //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
            lua_pushstring(L, name);      //Stack 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>name
            LuaHelper::RawSetField(L,
                                   -2,
                                   "classname"); // vst [classname] = name Stack 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst
            lua_pushvalue(L, -2); // 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>st
            lua_setmetatable(L, -2);  // vst.__metatable = st. 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
            LuaHelper::RawSetField(L, -5, name); // ns [name] = vst. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            //now stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        }
        else {
            //表存在Stack: ns 栈状态ua_gettop(L)== n + 2:ns=>vst
            LUA_ASSERT_EX (L, lua_istable(L, -1), "lua_istable(L, -1)", false);
            LuaHelper::RawGetField(L, -1, "classname");
            LUA_ASSERT_EX (L, lua_isstring(L, -1), "lua_isstring(L, -1)", false);
            std::string classname = std::string(lua_tostring(L, -1));
            LUA_ASSERT_EX(L,
                          name == classname,
                          (std::string("class wrong,rightclass = ") + name + std::string(",curname = ") + classname)
                              .c_str(),
                          false);
            lua_pop(L, 1); //name出栈  Stack: ns 栈状态ua_gettop(L)== n + 2:ns=>vst
//                printf("========================metatable==========================\n");
//                int top = lua_gettop(L);
//                if(lua_getmetatable(L,-1))
//                {
//                    top = lua_gettop(L);
//                    LuaHelper::DumpTable(L,-1,std::cout,2);
//                    top = lua_gettop(L);
//                }
//                printf("========================metatable==========================\n");
//                lua_pop(L, 1); // Stack: ns 栈状态ua_gettop(L)== n + 1:ns
//                lua_pop(L, 1); // Stack: ns 栈状态ua_gettop(L)== n + 1:ns
//                top = lua_gettop(L);

            lua_pop(L, 1); //vst出栈  Stack: ns 栈状态ua_gettop(L)== n + 1:ns

            lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetStaticKey()); // Stack 栈状态ua_gettop(L)== n + 3:ns=>st
            m_pLuaVm->AddStackSize(1);

            // Map T back from its stored tables
            lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetConstKey()); // Stack 栈状态ua_gettop(L)== n + 3:ns=>st=>co
            //调整co的位置
            lua_insert(L, -2); // Stack 栈状态ua_gettop(L)== n + 3:ns=>co=>st
            m_pLuaVm->AddStackSize(1);

            lua_rawgetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetClassKey()); // Stack 栈状态ua_gettop(L)== n + 4:ns=>co=>st=>cl
            //把栈顶元素移动到指定的有效索引处， 依次移动这个索引之上的元素,调整cl的位置
            lua_insert(L, -2); // Stack 栈状态ua_gettop(L)== n + 4:ns=>co=>cl=>st
            m_pLuaVm->AddStackSize(1);

            //now stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        }
    }

    //==========================================================================
    /**
      Derive a new class.
    */
    Class(char const *name, LuaVm *luaVm, void const *const staticKey, bool shared)
        : ClassBase(name, luaVm, shared)
    {
        lua_State *L = m_pLuaVm->LuaState();
        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        CreateConstTable(name); // Stack: ns, const table (co)
        lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); // Stack: ns, co, function
        LuaHelper::RawSetField(L, -2, "__gc"); // Stack: ns, co
        m_pLuaVm->AddStackSize(1);


        CreateClassTable(name); // Stack: ns, co, class table (cl)
        lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); // Stack: ns, co, cl, function
        LuaHelper::RawSetField(L, -2, "__gc"); // Stack: ns, co, cl
        m_pLuaVm->AddStackSize(1);


        CreateStaticTable(name); // Stack: ns, co, cl, st
        m_pLuaVm->AddStackSize(1);

        lua_rawgetp(L, LUA_REGISTRYINDEX, staticKey); // Stack: ns, co, cl, st, parent st (pst) | nil
        if (lua_isnil(L, -1)) // Stack: ns, co, cl, st, nil
        {
            m_pLuaVm->AddStackSize(1);
            throw std::runtime_error("Base class is not registered");
        }

        assert (lua_istable(L, -1)); // Stack: ns, co, cl, st, pst

        lua_rawgetp(L, -1, GetClassKey()); // Stack: ns, co, cl, st, pst, parent cl (pcl)
        assert (lua_istable(L, -1));

        lua_rawgetp(L, -1, GetConstKey()); // Stack: ns, co, cl, st, pst, pcl, parent co (pco)
        assert (lua_istable(L, -1));

        lua_rawsetp(L, -6, GetParentKey()); // co [parentKey] = pco. Stack: ns, co, cl, st, pst, pcl
        lua_rawsetp(L, -4, GetParentKey()); // cl [parentKey] = pcl. Stack: ns, co, cl, st, pst
        lua_rawsetp(L, -2, GetParentKey()); // st [parentKey] = pst. Stack: ns, co, cl, st

        lua_pushvalue(L, -1); // Stack: ns, co, cl, st, st
        lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetStaticKey()); // Stack: ns, co, cl, st
        lua_pushvalue(L, -2); // Stack: ns, co, cl, st, cl
        lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetClassKey()); // Stack: ns, co, cl, st
        lua_pushvalue(L, -3); // Stack: ns, co, cl, st, co
        lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetConstKey()); // Stack: ns, co, cl, st

        lua_newtable(L);             //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
        lua_pushstring(L, name);      //Stack 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>name
        LuaHelper::RawSetField(L,
                               -2,
                               "classname"); // vst [classname] = name Stack 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst
        lua_pushvalue(L, -2); // 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>st
        lua_setmetatable(L, -2);  // vst.__metatable = st. 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
        LuaHelper::RawSetField(L, -5, name); // ns [name] = vst. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
        //now stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
    }

    //--------------------------------------------------------------------------
    /**
      Continue registration in the enclosing namespace.
    */
    void EndClass()
    {
        assert (m_pLuaVm->GetStackSize() > 3);
        m_pLuaVm->AddStackSize(-3);
        lua_State *L = m_pLuaVm->LuaState();
        lua_pop(L, 3);
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static data member.
    */
    template<class U>
    Class<T> &AddStaticProperty(char const *name, U *pu, bool isWritable = true)
    {
        return AddStaticData(name, pu, isWritable);
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static data member.
    */
    template<class U>
    Class<T> &AddStaticData(char const *name, U *pu, bool isWritable = true)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushlightuserdata(L, pu); // Stack: co, cl, st, pointer
        lua_pushcclosure(L, &CFunc::GetVariable<U>, 1); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -2); // Stack: co, cl, st

        if (isWritable) {
            lua_pushlightuserdata(L, pu); // Stack: co, cl, st, ps, pointer
            lua_pushcclosure(L, &CFunc::SetVariable<U>, 1); // Stack: co, cl, st, ps, setter
        }
        else {
            lua_pushstring(L, name); // Stack: co, cl, st, name
            lua_pushcclosure(L, &CFunc::ReadOnlyError, 1); // Stack: co, cl, st, error_fn
        }
        CFunc::AddSetter(L, name, -2); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static property member.

      If the set function is null, the property is read-only.
    */
    template<class U>
    Class<T> &AddStaticProperty(char const *name, U (*get)(), void (*set)(U) = 0)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::Call<U (*)()>::f, 1); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -2); // Stack: co, cl, st

        if (set != 0) {
            lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::Call<void (*)(U)>::f, 1); // Stack: co, cl, st, setter
        }
        else {
            lua_pushstring(L, name); // Stack: co, cl, st, ps, name
            lua_pushcclosure(L, &CFunc::ReadOnlyError, 1); // Stack: co, cl, st, error_fn
        }
        CFunc::AddSetter(L, name, -2); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a static member function.
    */
    template<class FP>
    Class<T> &AddStaticFunction(char const *name, FP const fp)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushlightuserdata(L, reinterpret_cast <void *> (fp)); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::Call<FP>::f, 1); // co, cl, st, function
        LuaHelper::RawSetField(L, -2, name); // co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a lua_CFunction.
    */
    Class<T> &AddStaticFunction(char const *name, int (*const fp)(lua_State *))
    {
        return AddStaticCFunction(name, fp);
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a lua_CFunction.
    */
    Class<T> &AddStaticCFunction(char const *name, int (*const fp)(lua_State *))
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushcfunction(L, fp); // co, cl, st, function
        LuaHelper::RawSetField(L, -2, name); // co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a data member.
    */
    template<class U>
    Class<T> &AddProperty(char const *name, U T::* mp, bool isWritable = true)
    {
        return AddData(name, mp, isWritable);
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a data member.
    */
    template<class U>
    Class<T> &AddData(char const *name, U T::* mp, bool isWritable = true)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        typedef const U T::*mp_t;
        new(lua_newuserdata(L, sizeof(mp_t))) mp_t(mp); // Stack: co, cl, st, field ptr
        lua_pushcclosure(L, &CFunc::getProperty<T, U>, 1); // Stack: co, cl, st, getter
        lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
        CFunc::AddGetter(L, name, -5); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -3); // Stack: co, cl, st

        if (isWritable) {
            new(lua_newuserdata(L, sizeof(mp_t))) mp_t(mp); // Stack: co, cl, st, field ptr
            lua_pushcclosure(L, &CFunc::setProperty<T, U>, 1); // Stack: co, cl, st, setter
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st
        }

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member.
    */
    template<class TG, class TS = TG>
    Class<T> &AddProperty(char const *name, TG (T::* get)() const, void (T::* set)(TS) = 0)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        typedef TG (T::*get_t)() const;
        new(lua_newuserdata(L, sizeof(get_t))) get_t(get); // Stack: co, cl, st, funcion ptr
        lua_pushcclosure(L, &CFunc::CallConstMember<get_t>::f, 1); // Stack: co, cl, st, getter
        lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
        CFunc::AddGetter(L, name, -5); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -3); // Stack: co, cl, st

        if (set != 0) {
            typedef void (T::* set_t)(TS);
            new(lua_newuserdata(L, sizeof(set_t))) set_t(set); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::CallMember<set_t>::f, 1); // Stack: co, cl, st, setter
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st
        }

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member.
    */
    template<class TG, class TS = TG>
    Class<T> &AddProperty(char const *name, TG (T::* get)(lua_State *) const, void (T::* set)(TS, lua_State *) = 0)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        typedef TG (T::*get_t)(lua_State *) const;
        new(lua_newuserdata(L, sizeof(get_t))) get_t(get); // Stack: co, cl, st, funcion ptr
        lua_pushcclosure(L, &CFunc::CallConstMember<get_t>::f, 1); // Stack: co, cl, st, getter
        lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
        CFunc::AddGetter(L, name, -5); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -3); // Stack: co, cl, st

        if (set != 0) {
            typedef void (T::* set_t)(TS, lua_State *);
            new(lua_newuserdata(L, sizeof(set_t))) set_t(set); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::CallMember<set_t>::f, 1); // Stack: co, cl, st, setter
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st
        }

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member, by proxy.

      When a class is closed for modification and does not provide (or cannot
      provide) the function signatures necessary to implement get or set for
      a property, this will allow non-member functions act as proxies.

      Both the get and the set functions require a T const* and T* in the first
      argument respectively.
    */
    template<class TG, class TS = TG>
    Class<T> &AddProperty(char const *name, TG (*get)(T const *), void (*set)(T *, TS) = 0)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::Call<TG (*)(const T *)>::f, 1); // Stack: co, cl, st, getter
        lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
        CFunc::AddGetter(L, name, -5); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -3); // Stack: co, cl, st

        if (set != 0) {
            lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::Call<void (*)(T *, TS)>::f, 1); // Stack: co, cl, st, setter
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st
        }

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a property member, by proxy C-function.

      When a class is closed for modification and does not provide (or cannot
      provide) the function signatures necessary to implement get or set for
      a property, this will allow non-member functions act as proxies.

      The object userdata ('this') value is at the index 1.
      The new value for set function is at the index 2.
    */
    Class<T> &AddProperty(char const *name, int (*get)(lua_State *), int (*set)(lua_State *) = 0)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        lua_pushcfunction(L, get);
        lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
        CFunc::AddGetter(L, name, -5); // Stack: co, cl, st,, getter
        CFunc::AddGetter(L, name, -3); // Stack: co, cl, st,

        if (set != 0) {
            lua_pushcfunction(L, set);
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st,
        }

        return *this;
    }

    template<class TG, class TS = TG>
    Class<T> &
    AddProperty(char const *name, std::function<TG(const T *)> get, std::function<void(T *, TS)> set = nullptr)
    {
        lua_State *L = m_pLuaVm->LuaState();

        using GetType = decltype(get);
        new(lua_newuserdata(L, sizeof(get))) GetType(std::move(get)); // Stack: co, cl, st, function userdata (ud)
        lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
        lua_pushcfunction (L, &CFunc::GCMetaMethodAny<GetType>); // Stack: co, cl, st, ud, mt, gc function
        LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
        lua_setmetatable(L, -2); // Stack: co, cl, st, ud
        lua_pushcclosure(L, &CFunc::CallProxyFunctor<GetType>::f, 1); // Stack: co, cl, st, getter
        lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
        CFunc::AddGetter(L, name, -4); // Stack: co, cl, st, getter
        CFunc::AddGetter(L, name, -4); // Stack: co, cl, st

        if (set != nullptr) {
            using SetType = decltype(set);
            new(lua_newuserdata(L,
                                sizeof(set))) SetType(std::move(set)); // Stack: co, cl, st, function userdata (ud)
            lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
            lua_pushcfunction (L, &CFunc::GCMetaMethodAny<SetType>); // Stack: co, cl, st, ud, mt, gc function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
            lua_setmetatable(L, -2); // Stack: co, cl, st, ud
            lua_pushcclosure(L, &CFunc::CallProxyFunctor<SetType>::f, 1); // Stack: co, cl, st, setter
            CFunc::AddSetter(L, name, -3); // Stack: co, cl, st
        }

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member function by std::function.
    */
    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, std::function<ReturnType(T *, Params...)> function)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        using FnType = decltype(function);
        new(lua_newuserdata(L,
                            sizeof(function))) FnType(std::move(function)); // Stack: co, cl, st, function userdata (ud)
        lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
        lua_pushcfunction (L, &CFunc::GCMetaMethodAny<FnType>); // Stack: co, cl, st, ud, mt, gc function
        LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
        lua_setmetatable(L, -2); // Stack: co, cl, st, ud
        lua_pushcclosure(L, &CFunc::CallProxyFunctor<FnType>::f, 1); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a const member function by std::function.
    */
    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, std::function<ReturnType(const T *, Params...)> function)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        using FnType = decltype(function);
        new(lua_newuserdata(L,
                            sizeof(function))) FnType(std::move(function)); // Stack: co, cl, st, function userdata (ud)
        lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
        lua_pushcfunction (L, &CFunc::GCMetaMethodAny<FnType>); // Stack: co, cl, st, ud, mt, gc function
        LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
        lua_setmetatable(L, -2); // Stack: co, cl, st, ud
        lua_pushcclosure(L, &CFunc::CallProxyFunctor<FnType>::f, 1); // Stack: co, cl, st, function
        lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member function.
    */
    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, ReturnType (T::* mf)(Params...))
    {
        using MemFn = ReturnType (T::*)(Params...);

        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        static const std::string GC = "__gc";
        if (name == GC) {
            throw std::logic_error(GC + " metamethod registration is forbidden");
        }
        CFunc::CallMemberFunctionHelper<MemFn, false>::add(L, name, mf);
        return *this;
    }

    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, ReturnType (T::* mf)(Params...) const)
    {
        using MemFn = ReturnType (T::*)(Params...) const;

        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        static const std::string GC = "__gc";
        if (name == GC) {
            throw std::logic_error(GC + " metamethod registration is forbidden");
        }
        CFunc::CallMemberFunctionHelper<MemFn, true>::add(L, name, mf);
        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a proxy function.
    */
    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, ReturnType (*proxyFn)(T *object, Params...))
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        static const std::string GC = "__gc";
        if (name == GC) {
            throw std::logic_error(GC + " metamethod registration is forbidden");
        }
        using FnType = decltype(proxyFn);
        lua_pushlightuserdata(L, reinterpret_cast <void *> (proxyFn)); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::CallProxyFunction<FnType>::f, 1); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st
        return *this;
    }

    template<class ReturnType, class... Params>
    Class<T> &AddFunction(char const *name, ReturnType (*proxyFn)(const T *object, Params...))
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        static const std::string GC = "__gc";
        if (name == GC) {
            throw std::logic_error(GC + " metamethod registration is forbidden");
        }
        using FnType = decltype(proxyFn);
        lua_pushlightuserdata(L, reinterpret_cast <void *> (proxyFn)); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::CallProxyFunction<FnType>::f, 1); // Stack: co, cl, st, function
        lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st
        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member lua_CFunction.
    */
    Class<T> &AddFunction(char const *name, int (T::*mfp)(lua_State *))
    {
        return AddCFunction(name, mfp);
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a member lua_CFunction.
    */
    Class<T> &AddCFunction(char const *name, int (T::*mfp)(lua_State *))
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        typedef int (T::*MFP)(lua_State *);
        new(lua_newuserdata(L, sizeof(mfp))) MFP(mfp); // Stack: co, cl, st, function ptr
        lua_pushcclosure(L, &CFunc::CallMemberCFunction<T>::f, 1); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a const member lua_CFunction.
    */
    Class<T> &AddFunction(char const *name, int (T::*mfp)(lua_State *) const)
    {
        return AddCFunction(name, mfp);
    }

    //--------------------------------------------------------------------------
    /**
        Add or replace a const member lua_CFunction.
    */
    Class<T> &AddCFunction(char const *name, int (T::*mfp)(lua_State *) const)
    {
        AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
        lua_State *L = m_pLuaVm->LuaState();

        typedef int (T::*MFP)(lua_State *) const;
        new(lua_newuserdata(L, sizeof(mfp))) MFP(mfp);
        lua_pushcclosure(L, &CFunc::CallConstMemberCFunction<T>::f, 1);
        lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
        LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st

        return *this;
    }

    //--------------------------------------------------------------------------
    /**
      Add or replace a primary Constructor.

      The primary Constructor is invoked when calling the class type table
      like a function.

      The template parameter should be a function pointer type that matches
      the desired Constructor (since you can't take the address of a Constructor
      and pass it as an argument).
    */
    template<class MemFn>
    Class<T> &AddConstructor()
    {
        if (m_bshared) {
            AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
            lua_State *L = m_pLuaVm->LuaState();

            lua_pushcclosure(L, &CtorContainerProxy < MemFn, std::shared_ptr<T>> , 0);
            LuaHelper::RawSetField(L, -2, "__call");

            return *this;
        }
        else {
            AssertStackState(); // Stack: const table (co), class table (cl), static table (st)
            lua_State *L = m_pLuaVm->LuaState();

            lua_pushcclosure(L, &CtorPlacementProxy < MemFn, T > , 0);
            LuaHelper::RawSetField(L, -2, "__call");

            return *this;
        }
    }
};
} // namespace luabridge

#endif //__LUA_CLASS_H__
