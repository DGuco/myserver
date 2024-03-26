//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(¶Å¹ú³¬)<1139140929@qq.com>.  All rights reserved.

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

#ifndef __LUA_VM_H__
#define __LUA_VM_H__

#include "lua_library.h"

namespace luabridge
{
/**
 * Base for class and namespace registration.
 * Maintains Lua stack in the proper state.
 * Once beginNamespace, BeginClass or DeriveClass is called the parent
 * object upon its destruction may no longer clear the Lua stack.
 * Then endNamespace or EndClass is called, a new parent is created
 * and the child transfers the responsibility for clearing stack to it.
 * So there can be maximum one "active" registrar object.
 */
class LuaVm
{
protected:
    lua_State *const L;
    int mutable m_stackSize;
public:
    LuaVm(lua_State *L)
        : L(L), m_stackSize(0)
    {
    }

    LuaVm(const LuaVm &rhs)
        : L(rhs.L), m_stackSize(rhs.m_stackSize)
    {
        rhs.m_stackSize = 0;
    }

    LuaVm &operator=(const LuaVm &rhs)
    {
        LuaVm tmp(rhs);
        std::swap(m_stackSize, tmp.m_stackSize);
        return *this;
    }

    lua_State *LuaState()
    {
        return L;
    }
    virtual ~LuaVm()
    {
        if (m_stackSize > 0) {
            assert (m_stackSize <= lua_gettop(L));
            lua_pop(L, m_stackSize);
        }
    }

    void AddStackSize(int value)
    { m_stackSize = m_stackSize + value; };
    int GetStackSize()
    { return m_stackSize; }
    void AssertIsActive() const
    {
        if (m_stackSize == 0) {
            throw std::runtime_error("Unable to continue registration");
        }
    }
};

}
#endif //__LUA_VM_H__
