//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(�Ź���)<1139140929@qq.com>.  All rights reserved.

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

        // Stack: namespace table (ns) //ջ״̬lua_gettop(L) == n + 1:ջ״̬:ns
        lua_newtable(L); // Stack: ns, const table (co)   ջ״̬lua_gettop(L) == n + 2:ns=>co
        lua_pushvalue(L, -1); // Stack: ns, co, co ջ״̬lua_gettop(L) == n + 3:ns=>co=>co
        lua_setmetatable(L, -2); // co.__metatable = co. ջ״̬lua_gettop(L) == n + 2:ns=>co
        lua_pushstring(L, type_name.c_str()); // const table name ջ״̬lua_gettop(L)== n + 3:ns=>co=>type_name
        lua_rawsetp(L, -2, GetTypeKey()); // co [typeKey] = type_name. ջ״̬lua_gettop(L)== n + 2:ns=>co

        /**
         *https://zilongshanren.com/post/bind-a-simple-cpp-class-in-lua/
         *https://blog.csdn.net/qiuwen_521/article/details/107855867
         *��Lua������������ķ�ʽ�����ʡ�
         *local s = cc.create()
         *s:setName("zilongshanren")
         *s:setAge(20)
         *s:print()
         *s:setName(xx)�͵ȼ���s.setName(s,xx)����ʱ����ֻ��Ҫ��s�ṩһ��metatable,���Ҹ����metatable����һ��keyΪ"__index"��
         *value�����������metatable�����ֻ��Ҫ��֮ǰStudent���һЩ������ӵ����metatable����Ϳ�����,����keyΪ"__index"��
         *valueλһ��function(t,k)���͵ĺ����������п��Ը���k��ȡ��Ӧ����ķ���,�����ʵ���Ǻ���
        **/
        lua_pushcfunction(L, &CFunc::IndexMetaMethod); //ջ״̬lua_gettop(L)== n + 3:ns=>co=>IndexMetaMethod
        //co.__index = &CFunc::IndexMetaMethod ջ״̬lua_gettop(L)== n + 2:ns=>co
        LuaHelper::RawSetField(L, -2, "__index");

        lua_pushcfunction(L,
                          &CFunc::NewindexObjectMetaMethod); //ջ״̬lua_gettop(L)== n + 3:ns=>co=>NewindexObjectMetaMethod
        //co.__newindex = &CFunc::NewindexObjectMetaMethod ջ״̬lua_gettop(L)== n + 2:ns=>co
        LuaHelper::RawSetField(L, -2, "__newindex");

        lua_newtable(L); //propget table(gt) ջ״̬lua_gettop(L)== n + 3:ns=>co=>gt
        lua_rawsetp(L, -2, GetPropgetKey());//co[progetkey] = gt ջ״̬lua_gettop(L)== n + 2:ns=>co

        if (Security::hideMetatables()) {
            lua_pushnil(L);  //ջ״̬lua_gettop(L)== n + 3:ns=>co=>nil
            LuaHelper::RawSetField(L, -2, "__metatable"); //co.__metatable = nil ջ״̬lua_gettop(L)== n + 2:ns=>co
        }
        //now ջ״̬lua_gettop(L)== n + 2:ns=>co
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
        // Stack ջ״̬lua_gettop(L) == n + 2:ns=>co

        // Class table is the same as const table except the propset table
        CreateConstTable(name, false); // Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl

        lua_newtable(L); // propset table (ps)  Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>ps
        lua_rawsetp(L, -2, GetPropsetKey()); // cl [propsetKey] = ps. Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl

        lua_pushvalue(L, -2); // Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl=>co
        lua_rawsetp(L, -2, GetConstKey()); // cl [constKey] = co. Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl=>co

        lua_pushvalue(L, -1); // Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>cl
        lua_rawsetp(L, -3, GetClassKey()); // co [classKey] = cl.Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl
        //now ջ״̬lua_gettop(L)== n + 3:ns=>co=>cl
    }

    //--------------------------------------------------------------------------
    /**
      Create the static table.
    */
    void CreateStaticTable(char const *name)
    {
        lua_State *L = m_pLuaVm->LuaState();
        // Stack: namespace table (ns), const table (co), class table (cl)
        // Stack ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl
        lua_newtable(L); //visible static table (vst) Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
        std::string type_name = std::string("static_") + name;
        lua_pushstring(L, type_name.c_str()); //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>type_name
        lua_rawsetp(L, -2, GetTypeKey()); //st.typekey = type_name Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_pushcfunction(L, &CFunc::IndexMetaMethod); //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>IndexMetaMethod
        //st.__index = IndexMetaMethod,Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
        LuaHelper::RawSetField(L, -2, "__index");

        lua_pushcfunction(L,
                          &CFunc::NewindexStaticMetaMethod); //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>NewindexStaticMetaMethod
        //st.__newindex = NewindexStaticMetaMethod,Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
        LuaHelper::RawSetField(L, -2, "__newindex");

        lua_newtable(L); // proget table (pg) Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>pg
        lua_rawsetp(L, -2, GetPropgetKey()); // st [propgetKey] = pg. Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_newtable(L); // Stack: ns, co, cl, st, propset table (ps) Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>ps
        lua_rawsetp(L, -2, GetPropsetKey()); // st [propsetKey] = pg. Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

        lua_pushvalue(L, -2); //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
        lua_rawsetp(L, -2, GetClassKey()); // st [classKey] = cl.  Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

        if (Security::hideMetatables()) {
            lua_pushnil(L); // Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>nil
            LuaHelper::RawSetField(L,
                                   -2,
                                   "__metatable"); //st.__metatable = nil   Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
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

    //ע����ĳ�Աǰ���luaջ�ϵ�table�Ƿ�Ϸ�
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
      ���캯����ɺ�
      vst(visible static table) = {
          __metatable = st,
      }
      co(const table) = {
          __metatable = co,
          typekey = const_name,
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __gc = &CFunc::GCMetaMethod<T>,
          propgetKey = {table}(ͨ��addPropertyע����ͨ��Ա������get������ע��������),
          classKey = cl,
          func1_name = func1,(��ͨ��Ա����1 �ᱻע�����������),
          func2_name = func12,(��ͨ��Ա����2 �ᱻע�����������),
      }

      cl(class table) = {
          __metatable = cl,
          typekey = name,
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __gc = &CFunc::GCMetaMethod<T>,
          propgetKey = {}(table)(ͨ��addPropertyע����ͨ��Ա������get����Ҳ��ע��������),
          propsetKey = {}(table)(ͨ��addPropertyע����ͨ��Ա������set����Ҳ��ע��������),,
          constKey = co,
          func1_name = func1,(��ͨ��Ա����1 Ҳ�ᱻע�����������),
          func2_name = func12,(��ͨ��Ա����2 Ҳ�ᱻע�����������),
      }

      st(static table) = {
          __index = &CFunc::IndexMetaMethod,
          __newindex = &CFunc::NewindexStaticMetaMethod,
          __call = class Constructor(class Constructor �ᱻע�����������),
          propgetKey = {}(table)(ͨ��addStaticPropertyע�ᾲ̬��Ա������get������ע��������),
          propsetKey = {}(table)(ͨ��addStaticPropertyע�ᾲ̬��Ա������set������ע��������),
          classKey = cl,
          static_func1_name = func1,(static��Ա����1 Ҳ�ᱻע�����������),
          static_func2_name = func12,(static��Ա����2 Ҳ�ᱻע�����������),
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
        //ջ���Ƿ��Ǳ�(_G)
        LUA_ASSERT_EX(L, lua_istable(L, -1), "lua_istable(L, -1)", false); //ջ״̬lua_gettop(L)== n + 1:ns
        //����find���metadata��_G[name]
        LuaHelper::RawGetField(L, -1, name); // st = _G[name] ջ״̬lua_gettop(L)== n + 2:ns=>st|nil

        //������create it
        if (lua_isnil(L, -1)) // Stack: ns, nil ջ״̬:ns=>nil
        {
            //����nilֵ
            lua_pop(L, 1); // Stack: ns ջ״̬ua_gettop(L)== n + 1:ns
            //create���const metadata��(co),and set co.__metatable = co
            CreateConstTable(name);
            //now ջ״̬lua_gettop(L)== n + 2:ns=>co
            //ע��gcԪ����
            lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); // ջ״̬lua_gettop(L)== n + 3:ns=>co=>gcfun
            //co.__gc = gcfun ��co.__metatable.__gc = gcfun ջ״̬lua_gettop(L)== n + 2:ns=>co
            LuaHelper::RawSetField(L, -2, "__gc");
            m_pLuaVm->AddStackSize(1);

            //create��ķ�const metadata�� and set cl.__metatable = cl
            CreateClassTable(name); //class table (cl) stackջ״̬lua_gettop(L)== n + 3:ns=>co=>cl
            //now ջ״̬lua_gettop(L) == n + 3:ns=>co=>cl
            //ע��gcԪ����
            lua_pushcfunction(L, &CFunc::GCMetaMethod<T>); //gcfun stackջ״̬lua_gettop(L)== n + 4:ns=>co=>cl=>gcfun
            //cl.__gc = gcfun �� cl.__metatable.__gc = gcfun stackջ״̬lua_gettop(L)== n + 3:ns=>co=>cl
            LuaHelper::RawSetField(L, -2, "__gc");
            m_pLuaVm->AddStackSize(1);

            //create���static metadata�� and set st.__metatable = st
            CreateStaticTable(name); // Stack: ns, co, cl, st ջ״̬:ns=>co=>cl=>st
            //now ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
            m_pLuaVm->AddStackSize(1);

            //Map T back to its tables.
            //��stԪ����һ������ѹջ��
            lua_pushvalue(L, -1); // ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>st
            //��static metadata�����registry��
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetStaticKey()); //stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

            //��clԪ����һ������ѹջ��
            lua_pushvalue(L, -2); // stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
            //��metadata�����registry��
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetClassKey()); //stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

            //��coԪ����һ������ѹջ��
            lua_pushvalue(L, -3); // ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>co
            //��const metadata�����registry��
            lua_rawsetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetConstKey()); //stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st

            lua_newtable(L);             //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
            lua_pushstring(L, name);      //Stack ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>name
            LuaHelper::RawSetField(L,
                                   -2,
                                   "classname"); // vst [classname] = name Stack ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst
            lua_pushvalue(L, -2); // ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>st
            lua_setmetatable(L, -2);  // vst.__metatable = st. ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
            LuaHelper::RawSetField(L, -5, name); // ns [name] = vst. Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
            //now stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
        }
        else {
            //�����Stack: ns ջ״̬ua_gettop(L)== n + 2:ns=>vst
            LUA_ASSERT_EX (L, lua_istable(L, -1), "lua_istable(L, -1)", false);
            LuaHelper::RawGetField(L, -1, "classname");
            LUA_ASSERT_EX (L, lua_isstring(L, -1), "lua_isstring(L, -1)", false);
            std::string classname = std::string(lua_tostring(L, -1));
            LUA_ASSERT_EX(L,
                          name == classname,
                          (std::string("class wrong,rightclass = ") + name + std::string(",curname = ") + classname)
                              .c_str(),
                          false);
            lua_pop(L, 1); //name��ջ  Stack: ns ջ״̬ua_gettop(L)== n + 2:ns=>vst
//                printf("========================metatable==========================\n");
//                int top = lua_gettop(L);
//                if(lua_getmetatable(L,-1))
//                {
//                    top = lua_gettop(L);
//                    LuaHelper::DumpTable(L,-1,std::cout,2);
//                    top = lua_gettop(L);
//                }
//                printf("========================metatable==========================\n");
//                lua_pop(L, 1); // Stack: ns ջ״̬ua_gettop(L)== n + 1:ns
//                lua_pop(L, 1); // Stack: ns ջ״̬ua_gettop(L)== n + 1:ns
//                top = lua_gettop(L);

            lua_pop(L, 1); //vst��ջ  Stack: ns ջ״̬ua_gettop(L)== n + 1:ns

            lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetStaticKey()); // Stack ջ״̬ua_gettop(L)== n + 3:ns=>st
            m_pLuaVm->AddStackSize(1);

            // Map T back from its stored tables
            lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::GetConstKey()); // Stack ջ״̬ua_gettop(L)== n + 3:ns=>st=>co
            //����co��λ��
            lua_insert(L, -2); // Stack ջ״̬ua_gettop(L)== n + 3:ns=>co=>st
            m_pLuaVm->AddStackSize(1);

            lua_rawgetp(L,
                        LUA_REGISTRYINDEX,
                        ClassInfo<T>::GetClassKey()); // Stack ջ״̬ua_gettop(L)== n + 4:ns=>co=>st=>cl
            //��ջ��Ԫ���ƶ���ָ������Ч�������� �����ƶ��������֮�ϵ�Ԫ��,����cl��λ��
            lua_insert(L, -2); // Stack ջ״̬ua_gettop(L)== n + 4:ns=>co=>cl=>st
            m_pLuaVm->AddStackSize(1);

            //now stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
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

        lua_newtable(L);             //Stack ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
        lua_pushstring(L, name);      //Stack ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>name
        LuaHelper::RawSetField(L,
                               -2,
                               "classname"); // vst [classname] = name Stack ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst
        lua_pushvalue(L, -2); // ջ״̬lua_gettop(L) == n + 6:ns=>co=>cl=>st=>vst=>st
        lua_setmetatable(L, -2);  // vst.__metatable = st. ջ״̬lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
        LuaHelper::RawSetField(L, -5, name); // ns [name] = vst. Stack ջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
        //now stackջ״̬lua_gettop(L) == n + 4:ns=>co=>cl=>st
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
