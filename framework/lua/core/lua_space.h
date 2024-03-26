//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2019, Dmitry Tarakanov
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

#ifndef __LUA_NAMESPACE_H__
#define __LUA_NAMESPACE_H__

#include "lua_class.h"
#include "lua_helpers.h"

using namespace std;

namespace luabridge
{

/** Provides C++ to Lua registration capabilities.

    This class is not instantiated directly, call `getGlobalNamespace` to start
    the registration process.d
    lua 命名空间，对应lua中的一个table
*/
class Namespace
{
public:
    /**
     *
     */
    Namespace()
    {
        m_pLuaVm = NULL;
        m_name = "";
    }

    ~Namespace()
    {
        m_pLuaVm = NULL;
        m_name = "";
    }

    Namespace(const Namespace &other)
    {
        if (&other == this) {
            return;
        }
        this->m_pLuaVm = other.m_pLuaVm;
        this->m_name = other.m_name;
    }

    /**
     * Open the global namespace for registrations.
     * 默认命名空间，lua _G表，如果没有指定命名空间则所有的操作在_G表中
     * @param luaVm
     */
    explicit Namespace(LuaVm *luaVm)
        : m_pLuaVm(luaVm), m_name("_G")
    {
        lua_State *L = m_pLuaVm->LuaState();
        lua_getglobal(L, "_G");
        m_pLuaVm->AddStackSize(1);
    }

    /**
     * Open a namespace for registrations.
     * The namespace is created if it doesn't already exist.
     * The parent namespace is at the top of the Lua stack.
     * 打开新的命名空间，如果不存在则创建
     * @param name
     * @param luaVm
     */
    Namespace(char const *name, LuaVm *luaVm)
        : m_pLuaVm(luaVm), m_name(name)
    {
        lua_State *L = m_pLuaVm->LuaState();

        LUA_ASSERT_EX(L, lua_istable(L, -1), "lua_istable(L,-1)", false); // Stack: parent namespace (pns) 一般情况下是_G表

        LuaHelper::RawGetField(L, -1, name); // Stack: pns, namespace (ns) | nil

        //如果父命名空间中没有找到该表则创建
        if (lua_isnil(L, -1)) // Stack: pns, nil
        {
            //把nil 值弹出栈顶
            lua_pop(L, 1); // Stack: pns

            //创建一个新的table
            lua_newtable(L); // Stack: pns, ns
            //copy 创建的table 到栈顶
            lua_pushvalue(L, -1); // Stack: pns, ns, ns

            // ns.__metatable = ns
            lua_setmetatable(L, -2); // Stack: pns, ns

            lua_pushcfunction(L, &CFunc::IndexMetaMethod); // Stack: pns, ns, &CFunc::IndexMetaMethod
            // ns.__index = IndexMetaMethod
            LuaHelper::RawSetField(L, -2, "__index"); // Stack: pns, ns

            lua_pushcfunction(L, &CFunc::NewindexStaticMetaMethod); // Stack: pns, ns,&CFunc::NewindexStaticMetaMethod
            // ns.__newindex = NewindexMetaMethod
            LuaHelper::RawSetField(L, -2, "__newindex"); // Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propget table (pg)
            lua_rawsetp(L, -2, GetPropgetKey()); // ns[propgetKey] = pg. Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propset table (ps)
            lua_rawsetp(L, -2, GetPropsetKey()); // ns[propsetKey] = ps. Stack: pns, ns

            lua_pushvalue(L, -1); // Stack: pns, ns, ns
            // pns [name] = ns,通常情况下即_G[name] = ns
            LuaHelper::RawSetField(L, -3, name); // Stack: pns, ns
        }
        m_pLuaVm->AddStackSize(1);
    }

    /**
     * 打开新的命名空间，如果不存在则创建
     * @param name
     * @return
     */
    Namespace BeginNamespace(char const *name)
    {
        m_pLuaVm->AssertIsActive();
        return Namespace(name, m_pLuaVm);
    }

    /**
     *  Add or replace a variable.在命名空间中添加一个全局变量
     * @tparam T  variable type
     * @param name
     * @param pt   变量的地址
     * @param isWritable  是否可写
     */
    template<class T>
    void AddProperty(char const *name, T *pt, bool isWritable = true)
    {
        lua_State *L = m_pLuaVm->LuaState();
        if (m_pLuaVm->GetStackSize() == 1) {
            LUA_ASSERT_EX(L, false, "AddProperty () called on global namespace", false);
        }

        LUA_ASSERT_EX (L, lua_istable(L, -1), "lua_istable(L, -1)", false); // Stack: namespace table (ns)

        //注册getter方法
        lua_pushlightuserdata(L, pt); // Stack: ns, pointer
        lua_pushcclosure(L, &CFunc::GetVariable<T>, 1); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2); // Stack: ns

        //注册setter方法
        if (isWritable) {
            //isWritable is true  注册setter方法
            lua_pushlightuserdata(L, pt); // Stack: ns, pointer
            lua_pushcclosure(L, &CFunc::SetVariable<T>, 1); // Stack: ns, setter
        }
        else {
            //isWritable is false  调用setter 方法时抛出lua error
            lua_pushstring(L, name); // Stack: ns, ps, namedd
            lua_pushcclosure(L, &CFunc::ReadOnlyError, 1); // Stack: ns, error_fn
        }
        CFunc::AddSetter(L, name, -2); // Stack: ns
    }

    /**
     * Add or replace a variable.在命名空间中添加一个全局变量
     * @tparam TG  variable type
     * @tparam TS  variable type
     * @param name
     * @param get   get func
     * @param set   set func
     * 
     * */
    template<class TG, class TS = TG>
    void AddProperty(char const *name, TG (*get)(), void (*set)(TS) = 0)
    {
        if (m_pLuaVm->GetStackSize() == 1) {
            throw std::logic_error("AddProperty () called on global namespace");
        }
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: ns, function ptr
        lua_pushcclosure(L, &CFunc::Call<TG (*)()>::f, 1); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2);

        if (set != 0) {
            lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: ns, function ptr
            lua_pushcclosure(L, &CFunc::Call<void (*)(TS)>::f, 1);
        }
        else {
            lua_pushstring(L, name);
            lua_pushcclosure(L, &CFunc::ReadOnlyError, 1);
        }
        CFunc::AddSetter(L, name, -2);
    }

    /**
     * Add or replace a variable.在命名空间中添加一个全局变量
     * @param name
     * @param get   get func
     * @param set   set func
     * 
     * */
    void AddProperty(char const *name, int (*get)(lua_State *), int (*set)(lua_State *) = 0)
    {
        if (m_pLuaVm->GetStackSize() == 1) {
            throw std::logic_error("AddProperty () called on global namespace");
        }
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)
        lua_pushcfunction(L, get); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2); // Stack: ns
        if (set != 0) {
            lua_pushcfunction(L, set); // Stack: ns, setter
            CFunc::AddSetter(L, name, -2); // Stack: ns
        }
        else {
            lua_pushstring(L, name); // Stack: ns, name
            lua_pushcclosure(L, &CFunc::ReadOnlyError, 1); // Stack: ns, name, ReadOnlyError
            CFunc::AddSetter(L, name, -2); // Stack: ns
        }
    }

    /**
     * register cfunction
     * @param func func name 函数名
     * @param f
     **/
    void AddCFunction(const char *func, lua_CFunction fp)
    {
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushcfunction(L, fp); // Stack: ns, function
        LuaHelper::RawSetField(L, -2, func); // Stack: ns
    }

    /**
     * register cfunction
     * @param L  lua_State
     * @tparam   Func func type
     * @param fp func name 函数名
     * @param f
     **/
    template<class Func>
    void AddCFunction(const char *func, Func const fp)
    {
        AddCFunction(func, LuaCFunctionWrap<__COUNTER__>(fp));
    }

    /**
     * register cfunction
     * @param L    lua_State
     * @param func func name 函数名
     * @param f
     **/
    static void AddGlobalCFunc(lua_State *L, const char *func, lua_CFunction f)
    {
        lua_register(L, func, f);
    }

    /**
     * register cfunction
     * @param L  lua_State
     * @tparam   Func func type
     * @param fp func name 函数名
     * @param f
     **/
    template<class Func>
    static void AddGlobalCFunc(lua_State *L, const char *func, Func const fp)
    {
        AddGlobalCFunc(L, func, LuaCFunctionWrap<__COUNTER__>(fp));
    }

    //----------------------------------------------------------------------------
    /**
        Open a new or existing class for registrations.
    */
    template<class T>
    Class<T>* BeginClass(char const *name, bool shared)
    {
        m_pLuaVm->AssertIsActive();
        return new Class<T>(name, m_pLuaVm, shared);
    }

    //----------------------------------------------------------------------------
    /**
        Derive a new class for registrations.

        To continue registrations for the class later, use BeginClass ().
        Do not call DeriveClass () again.
    */
    template<class Derived, class Base>
    Class<Derived>* DeriveClass(char const *name)
    {
        m_pLuaVm->AssertIsActive();
        return new Class<Derived>(name, m_pLuaVm, ClassInfo<Base>::GetStaticKey());
    }

    void Reset()
    {
        m_pLuaVm = NULL;
        m_name = "";
    }

    bool IsValid()
    {
        return m_pLuaVm != NULL && m_name != "";
    }

    const std::string &SpaceName() const
    {
        return m_name;
    }

private:
    LuaVm *m_pLuaVm;
    std::string m_name;
};
} // namespace luabridge

#endif
