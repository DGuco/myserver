//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2008, Nigel Atkinson <suprapilot+LuaCode@gmail.com>

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

#ifndef __LUA_EXCEPTION_H__
#define __LUA_EXCEPTION_H__

#include <exception>
#include <string>

namespace luabridge
{

class LuaException: public std::exception
{
private:
    lua_State *m_L;
    std::string m_what;

public:
    //----------------------------------------------------------------------------
    /**
        Construct a LuaException after a lua_pcall().
    */
    LuaException(lua_State *L, int /*code*/)
        : m_L(L)
    {
        WhatFromStack();
    }

    //----------------------------------------------------------------------------

    LuaException(lua_State *L,
                 char const *,
                 char const *,
                 long)
        : m_L(L)
    {
        WhatFromStack();
    }

    //----------------------------------------------------------------------------

    ~LuaException() throw()
    {
    }

    //----------------------------------------------------------------------------

    char const *what() const throw()
    {
        return m_what.c_str();
    }

    //============================================================================
    /**
        Throw an exception.

        This centralizes all the exceptions thrown, so that we can set
        breakpoints before the stack is unwound, or otherwise customize the
        behavior.
    */
    template<class Exception>
    static void Throw(Exception e)
    {
        throw e;
    }

    //----------------------------------------------------------------------------
    /**
        Initializes error handling. Subsequent Lua errors are translated to C++ exceptions.
    */
    static void EnableExceptions(lua_State *L)
    {
        lua_atpanic(L, ThrowAtPanic);
    }

protected:
    void WhatFromStack()
    {
        if (lua_gettop(m_L) > 0) {
            char const *s = lua_tostring (m_L, -1);
            m_what = s ? s : "";
        }
        else {
            // stack is empty
            m_what = "missing error";
        }
    }

private:
    static int ThrowAtPanic(lua_State *L)
    {
        throw LuaException(L, -1);
    }
};

//----------------------------------------------------------------------------
/**
    Initializes error handling. Subsequent Lua errors are translated to C++ exceptions.
*/
static void EnableExceptions(lua_State *L)
{
    LuaException::EnableExceptions(L);
}

} // namespace luabridge

#endif