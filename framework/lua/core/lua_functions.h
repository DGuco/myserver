//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2019, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright (C) 2004 Yong Lin.  All rights reserved.

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

#ifndef __LUA_FUNCTION_H__
#define __LUA_FUNCTION_H__

#include <string>
#include "func_traits.h"
#include "lua_library.h"

namespace luabridge
{

// We use a structure so we can define everything in the header.
//
struct CFunc
{
    static void AddGetter(lua_State *L, const char *name, int tableIndex)
    {
        assert (lua_istable(L, tableIndex));
        assert (lua_iscfunction(L, -1)); // Stack: getter

        lua_rawgetp(L, tableIndex, GetPropgetKey()); // Stack: getter, propget table (pg)
        lua_pushvalue(L, -2); // Stack: getter, pg, getter
        LuaHelper::RawSetField(L, -2, name); // Stack: getter, pg
        lua_pop (L, 2); // Stack: -
    }

    static void AddSetter(lua_State *L, const char *name, int tableIndex)
    {
        assert (lua_istable(L, tableIndex));
        assert (lua_iscfunction(L, -1)); // Stack: setter

        lua_rawgetp(L, tableIndex, GetPropsetKey()); // Stack: setter, propset table (ps)
        lua_pushvalue(L, -2); // Stack: setter, ps, setter
        LuaHelper::RawSetField(L, -2, name); // Stack: setter, ps
        lua_pop (L, 2); // Stack: -
    }

    //----------------------------------------------------------------------------
    /**
        __index metamethod for a namespace or class static and non-static members.
        Retrieves functions from metatables and properties from propget tables.
        Looks through the class hierarchy if inheritance is present.
        __index = function(t, k) {}
    */
    static int IndexMetaMethod(lua_State *L)
    {
        //栈状态lua_gettop(L) == 2:tu(t)=>field name(k)
        //check第一个元素是否是一个表或者userdata
        LUA_ASSERT(L, lua_istable(L, 1) || lua_isuserdata(L, 1), "lua_istable(L, 1) || lua_isuserdata(L, 1)");

        //将其元表压栈
        LUA_ASSERT (L, lua_getmetatable(L, 1) == 1, "lua_getmetatable(L, 1) == 1");
        //mt = tu.__metatable 栈状态lua_gettop(L) == 3:tu=>field name=>mt
        LUA_ASSERT (L, lua_istable(L, -1), "lua_istable(L, 1)");

        for (;;) {
            lua_pushvalue(L, 2); // 栈状态lua_gettop(L)==4:tu=>field name =>mt =>field name
            lua_rawget(L, -2);  //func = mt[field name] 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>func

            //找到了name对应的函数,return 1
            if (lua_iscfunction(L, -1)) //栈状态lua_gettop(L) == 4:tu=>field name=>mt=>func
            {
                lua_remove(L, -2);  //栈状态lua_gettop(L) == 3:tu=>field name=>func
                return 1;
            }

            //没有找到了name对应的函数
            // 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>nil
            LUA_ASSERT (L, lua_isnil(L, -1), "lua_istable(L, 1)");
            lua_pop (L, 1); // 栈状态lua_gettop(L) == 3:tu=>field name=>mt

            lua_rawgetp(L, -1, GetPropgetKey()); //pg=mg['getkey']  栈状态lua_gettop(L) == 4:tu=>field name=>mt=>pg
            LUA_ASSERT (L, lua_istable(L, -1), "lua_istable(L, 1)");

            lua_pushvalue(L, 2); //栈状态lua_gettop(L) == 5:tu=>field name=>mt=>pg=>field name
            lua_rawget(L, -2);  //getter = pg[field name]栈状态lua_gettop(L) == 5:tu=>field name=>mt=>pg=>getter|nil
            lua_remove(L, -2); // 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>getter|nil

            //找到了name对应getter的函数
            if (lua_iscfunction(L, -1)) // 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>getter
            {
                lua_remove(L, -2);  //栈状态lua_gettop(L) == 3:tu=>field name=>getter
                lua_pushvalue(L, 1); //栈状态lua_gettop(L) == 4:tu=>field name=>getter=>tu
                /**
                 *lua_call
                 *调用一个函数。要调用一个函数请遵循以下协议： 首先，要调用的函数应该被压入栈； 接着，把需要传递给这个函数的参数按正序压栈； 这是指第一个参数首先压栈。
                 *最后调用一下 lua_call； nargs 是你压入栈的参数个数。 当函数调用完毕后，所有的参数以及函数本身都会出栈。 而函数的返回值这时则被压栈。 返回值的个
                 *数将被调整为 nresults 个， 除非 nresults 被设置成 LUA_MULTRET。 在这种情况下，所有的返回值都被压入堆栈中。 Lua 会保证返回值都放入栈空间中。
                 *函数返回值将按正序压栈（第一个返回值首先压栈）， 因此在调用结束后，最后一个返回值将被放在栈顶。
                 */
                lua_call(L, 1, 1); // 栈状态lua_gettop(L) == 3:tu=>field name=>value
                return 1;
            }

            //没有找到了name对应getter的函数
            // 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>nil
            LUA_ASSERT (L, lua_isnil(L, -1), "lua_isnil(L, -1)");
            lua_pop (L, 1);            // 栈状态lua_gettop(L) == 3:tu=>field name=>mt

            // It may mean that the field may be in const table and it's constness violation.
            // Don't check that, just return nil

            // Repeat the lookup in the parent metafield,
            // or return nil if the field doesn't exist.
            //尝试获取父类的metatable
            lua_rawgetp(L,
                        -1,
                        GetParentKey()); //pmt = mt['parentkey'] 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>pmt|nil

            //没有找到父类的metatable
            if (lua_isnil (L, -1)) // 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>nil
            {
                lua_remove(L, -2); // 栈状态lua_gettop(L) == 3:tu=>field name=>nil
                return 1; //返回nil
            }

            //找到父类的metatable 栈状态lua_gettop(L) == 4:tu=>field name=>mt=>pmt
            // Removethe  metatable and repeat the search in the parent one.
            LUA_ASSERT (L, lua_istable(L, -1), "lua_istable(L, -1)");
            lua_remove(L, -2);
            //now 栈状态lua_gettop(L) == 3:tu=>field name=>pmt 回到开头在父类的metatable再找一遍
        }
    }

    //----------------------------------------------------------------------------
    /**
        __newindex metamethod for namespace or class static members.
        Retrieves properties from propset tables.
    */
    static int NewindexStaticMetaMethod(lua_State *L)
    {
        return NewindexMetaMethod(L, false);
    }

    //----------------------------------------------------------------------------
    /**
        __newindex metamethod for non-static members.
        Retrieves properties from propset tables.
    */
    static int NewindexObjectMetaMethod(lua_State *L)
    {
        return NewindexMetaMethod(L, true);
    }

    static int NewindexMetaMethod(lua_State *L, bool pushSelf)
    {
        assert (
            lua_istable(L, 1) || lua_isuserdata(L, 1)); // Stack (further not shown): table | userdata, name, new value

        lua_getmetatable(L, 1); // Stack: metatable (mt)
        assert (lua_istable(L, -1));

        for (;;) {
            lua_rawgetp(L, -1, GetPropsetKey()); // Stack: mt, propset table (ps) | nil

            if (lua_isnil (L, -1)) // Stack: mt, nil
            {
                lua_pop (L, 2); // Stack: -
                return luaL_error(L, "No member named '%s'", lua_tostring (L, 2));
            }

            assert (lua_istable(L, -1));

            lua_pushvalue(L, 2); // Stack: mt, ps, field name
            lua_rawget(L, -2); // Stack: mt, ps, setter | nil
            lua_remove(L, -2); // Stack: mt, setter | nil

            if (lua_iscfunction(L, -1)) // Stack: mt, setter
            {
                lua_remove(L, -2); // Stack: setter
                if (pushSelf) {
                    lua_pushvalue(L, 1); // Stack: setter, table | userdata
                }
                lua_pushvalue(L, 3); // Stack: setter, table | userdata, new value
                lua_call(L, pushSelf ? 2 : 1, 0); // Stack: -
                return 0;
            }

            assert (lua_isnil(L, -1)); // Stack: mt, nil
            lua_pop (L, 1); // Stack: mt

            lua_rawgetp(L, -1, GetParentKey()); // Stack: mt, parent mt | nil

            if (lua_isnil (L, -1)) // Stack: mt, nil
            {
                lua_pop (L, 1); // Stack: -
                return luaL_error(L, "No writable member '%s'", lua_tostring (L, 2));
            }

            assert (lua_istable(L, -1)); // Stack: mt, parent mt
            lua_remove(L, -2); // Stack: parent mt
            // Repeat the search in the parent
        }

        // no return
    }

    //----------------------------------------------------------------------------
    /**
        lua_CFunction to report an error writing to a read-only value.

        The name of the variable is in the first upvalue.
    */
    static int ReadOnlyError(lua_State *L)
    {
        std::string s;

        s = s + "'" + lua_tostring (L, lua_upvalueindex(1)) + "' is read-only";

        return luaL_error(L, s.c_str());
    }

    //----------------------------------------------------------------------------
    /**
        lua_CFunction to get a variable.

        This is used for global variables or class static data members.

        The pointer to the data is in the first upvalue.
    */
    template<class T>
    static int GetVariable(lua_State *L)
    {
        assert (lua_islightuserdata(L, lua_upvalueindex(1)));
        T const *ptr = static_cast <T const *> (lua_touserdata(L, lua_upvalueindex (1)));
        assert (ptr != 0);
        Stack<T>::push(L, *ptr);
        return 1;
    }

    //----------------------------------------------------------------------------
    /**
        lua_CFunction to set a variable.

        This is used for global variables or class static data members.

        The pointer to the data is in the first upvalue.
    */
    template<class T>
    static int SetVariable(lua_State *L)
    {
        assert (lua_islightuserdata(L, lua_upvalueindex(1)));
        T *ptr = static_cast <T *> (lua_touserdata(L, lua_upvalueindex (1)));
        assert (ptr != 0);
        *ptr = Stack<T>::get(L, 1);
        return 0;
    }

    //----------------------------------------------------------------------------
    /**
        lua_CFunction to call a function with a return value.

        This is used for global functions, global properties, class static methods,
        and class static properties.

        The function pointer (lightuserdata) in the first upvalue.
    */
    template<class FnPtr>
    struct Call
    {
        typedef typename FuncTraits<FnPtr>::ReturnType ReturnType;
        static int f(lua_State *L)
        {
            assert (lua_islightuserdata(L, lua_upvalueindex(1)));
            FnPtr fnptr = reinterpret_cast <FnPtr> (lua_touserdata(L, lua_upvalueindex (1)));
            assert (fnptr != 0);
            return Invoke<ReturnType, 1>::run(L, fnptr);
        }
    };

    //----------------------------------------------------------------------------
    /**
        lua_CFunction to call a function with a return value.

        This is used for global functions, global properties, class static methods,
        and class static properties.

        The function pointer (lightuserdata) in the first upvalue.
    */
    template<class FnPtr>
    struct CFCall
    {
        typedef typename FuncTraits<FnPtr>::ReturnType ReturnType;

        static int f(lua_State *L, FnPtr fnptr)
        {
            return Invoke<ReturnType, 1>::run(L, fnptr);
        }
    };
    //----------------------------------------------------------------------------
    /**
        lua_CFunction to call a class member function with a return value.

        The member function pointer is in the first upvalue.
        The class userdata object is at the top of the Lua stack.
    */
    template<class MemFnPtr>
    struct CallMember
    {
        typedef typename FuncTraits<MemFnPtr>::ClassType T;
        typedef typename FuncTraits<MemFnPtr>::ReturnType ReturnType;

        static int f(lua_State *L)
        {
            LUA_ASSERT(L, LuaHelper::IsFullUserData(L, lua_upvalueindex(1)), "CallMember::f IsFullUserData");
            T *const t = Userdata::get<T>(L, 1, false);
            MemFnPtr const &fnptr = *static_cast <MemFnPtr const *> (lua_touserdata(L, lua_upvalueindex (1)));
            LUA_ASSERT(L, fnptr != 0, "CallMember::f fnptr != 0 ");
            return Invoke<ReturnType, 2>::run(L, t, fnptr);
        }
    };

    template<class MemFnPtr>
    struct CallConstMember
    {
        typedef typename FuncTraits<MemFnPtr>::ClassType T;
        typedef typename FuncTraits<MemFnPtr>::ReturnType ReturnType;

        static int f(lua_State *L)
        {
            assert (LuaHelper::IsFullUserData(L, lua_upvalueindex(1)));
            T const *const t = Userdata::get<T>(L, 1, true);
            MemFnPtr const &fnptr = *static_cast <MemFnPtr const *> (lua_touserdata(L, lua_upvalueindex (1)));
            assert (fnptr != 0);
            return Invoke<ReturnType, 2>::run(L, t, fnptr);
        }
    };

    //--------------------------------------------------------------------------
    /**
        lua_CFunction to call a class member lua_CFunction.

        The member function pointer is in the first upvalue.
        The object userdata ('this') value is at top ot the Lua stack.
    */
    template<class T>
    struct CallMemberCFunction
    {
        static int f(lua_State *L)
        {
            assert (LuaHelper::IsFullUserData(L, lua_upvalueindex(1)));
            typedef int (T::*MFP)(lua_State *L);
            T *const t = Userdata::get<T>(L, 1, false);
            MFP const &fnptr = *static_cast <MFP const *> (lua_touserdata(L, lua_upvalueindex (1)));
            assert (fnptr != 0);
            return (t->*fnptr)(L);
        }
    };

    template<class T>
    struct CallConstMemberCFunction
    {
        static int f(lua_State *L)
        {
            assert (LuaHelper::IsFullUserData(L, lua_upvalueindex(1)));
            typedef int (T::*MFP)(lua_State *L);
            T const *const t = Userdata::get<T>(L, 1, true);
            MFP const &fnptr = *static_cast <MFP const *> (lua_touserdata(L, lua_upvalueindex (1)));
            assert (fnptr != 0);
            return (t->*fnptr)(L);
        }
    };

    /**
        lua_CFunction to call on a object.

        The proxy function pointer (lightuserdata) is in the first upvalue.
        The class userdata object is at the top of the Lua stack.
    */
    template<class FnPtr>
    struct CallProxyFunction
    {
        using ReturnType = typename FuncTraits<FnPtr>::ReturnType;

        static int f(lua_State *L)
        {
            assert (lua_islightuserdata(L, lua_upvalueindex(1)));
            auto fnptr = reinterpret_cast <FnPtr> (lua_touserdata(L, lua_upvalueindex (1)));
            assert (fnptr != 0);
            return Invoke<ReturnType, 1>::run(L, fnptr);
        }
    };

    template<class Functor>
    struct CallProxyFunctor
    {
        using ReturnType = typename FuncTraits<Functor>::ReturnType;

        static int f(lua_State *L)
        {
            assert (LuaHelper::IsFullUserData(L, lua_upvalueindex(1)));
            Functor &fn = *static_cast <Functor *> (lua_touserdata(L, lua_upvalueindex (1)));
            return Invoke<ReturnType, 1>::run(L, fn);
        }
    };

    // SFINAE Helpers
    template<class MemFnPtr, bool isConst>
    struct CallMemberFunctionHelper
    {
        static void add(lua_State *L, char const *name, MemFnPtr mf)
        {
            new(lua_newuserdata(L, sizeof(MemFnPtr))) MemFnPtr(mf);
            lua_pushcclosure(L, &CallConstMember<MemFnPtr>::f, 1);
            lua_pushvalue(L, -1);
            LuaHelper::RawSetField(L, -5, name); // const table
            LuaHelper::RawSetField(L, -3, name); // class table
        }
    };

    template<class MemFnPtr>
    struct CallMemberFunctionHelper<MemFnPtr, false>
    {
        static void add(lua_State *L, char const *name, MemFnPtr mf)
        {
            new(lua_newuserdata(L, sizeof(MemFnPtr))) MemFnPtr(mf);
            lua_pushcclosure(L, &CallMember<MemFnPtr>::f, 1);
            LuaHelper::RawSetField(L, -3, name); // class table
        }
    };

    //--------------------------------------------------------------------------
    /**
        __gc metamethod for a class.
    */
    template<class C>
    static int GCMetaMethod(lua_State *L)
    {
        Userdata *const ud = Userdata::getExact<C>(L, 1);
        ud->~Userdata();
        return 0;
    }

    /**
        __gc metamethod for an arbitrary class.
    */
    template<class T>
    static int GCMetaMethodAny(lua_State *L)
    {
        assert (LuaHelper::IsFullUserData(L, 1));
        T *t = static_cast <T *> (lua_touserdata(L, 1));
        t->~T();
        return 0;
    }

    //--------------------------------------------------------------------------
    /**
        lua_CFunction to get a class data member.

        The pointer-to-member is in the first upvalue.
        The class userdata object is at the top of the Lua stack.
    */
    template<class C, typename T>
    static int getProperty(lua_State *L)
    {
        C *const c = Userdata::get<C>(L, 1, true);
        T C::* *mp = static_cast <T C::* *> (lua_touserdata(L, lua_upvalueindex (1)));
        try {
            Stack<T &>::push(L, c->**mp);
        }
        catch (const std::exception &e) {
            luaL_error(L, e.what());
        }
        return 1;
    }

    //--------------------------------------------------------------------------
    /**
        lua_CFunction to set a class data member.

        The pointer-to-member is in the first upvalue.
        The class userdata object is at the top of the Lua stack.
    */
    template<class C, typename T>
    static int setProperty(lua_State *L)
    {
        C *const c = Userdata::get<C>(L, 1, false);
        T C::* *mp = static_cast <T C::* *> (lua_touserdata(L, lua_upvalueindex (1)));
        try {
            c->**mp = Stack<T>::get(L, 2);
        }
        catch (const std::exception &e) {
            luaL_error(L, e.what());
        }
        return 0;
    }
};

template<typename Func, int FUNCID>
struct lua_function
{
    static Func fn;
    static int Call(lua_State *L)
    {
        return CFunc::CFCall<Func>::f(L, fn);
    }
};

template<typename Func, int FUNCID>
Func lua_function<Func, FUNCID>::fn;

template<int FUNCID, class FT>
struct LuaCFunctionWrapI
{

};

template<int FUNCID, class R, class... ParamList>
struct LuaCFunctionWrapI<FUNCID, R (*)(ParamList...)>
{
    using DeclType = R (*)(ParamList...);
    typedef typename ArgTypeList<ParamList...>::template args<0>::type ParType;
    inline lua_CFunction operator()(DeclType f)
    {
        lua_function<DeclType, FUNCID>::fn = f;
        return &lua_function<DeclType, FUNCID>::Call;
    }
};

template<int FUNCID, class R, class... ParamList>
struct LuaCFunctionWrapI<FUNCID, std::function<R(ParamList...)>>
{
    using DeclType = std::function<R(ParamList...)>;

    typedef typename ArgTypeList<ParamList...>::template args<0>::type ParType;
    inline lua_CFunction operator()(DeclType f)
    {
        lua_function<DeclType, FUNCID>::fn = f;
        return &lua_function<DeclType, FUNCID>::Call;
    }
};

template<int FUNCID, typename Func>
inline lua_CFunction LuaCFunctionWrap(Func f)
{
    return LuaCFunctionWrapI<FUNCID, Func>()(f);
}

} // namespace luabridge

#endif