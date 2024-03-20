//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>

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

#ifndef __CLASS_KEY_H__
#define __CLASS_KEY_H__

namespace luabridge
{

/**
 * A unique key for a type name in a metatable.
 */
inline const void *GetTypeKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0x71);
#endif
}

/**
 * The key of a const table in another metatable.
 */
inline const void *GetConstKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0xc07);
#endif
}

/**
 * The key of a class table in another metatable.
 */
inline const void *GetClassKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0xc1a);
#endif
}

/**
 * The key of a propget table in another metatable.
 */
inline const void *GetPropgetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0x6e7);
#endif
}

/**
 * The key of a propset table in another metatable.
 */
inline const void *GetPropsetKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0x5e7);
#endif
}

/**
 * The key of a static table in another metatable.
 */
inline const void *GetStaticKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0x57a);
#endif
}

/**
 * The key of a parent table in another metatable.
 */
inline const void *GetParentKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast <void *> (0xdad);
#endif
}

/** Unique Lua registry keys for a class.

    Each registered class inserts three keys into the registry, whose
    values are the corresponding static, class, and const metatables. This
    allows a quick and reliable lookup for a metatable from a template type.
    利用静态函数，静态局部变量的特性，取每个类的模板类ClassInfo<T>的三个函数的局部变量的地址给每个类的不同的三个table生成唯一的key，全局唯一
*/
template<class T>
class ClassInfo
{
public:
    /** Get the key for the static table.

        The static table holds the static data members, static properties, and
        static member functions for a class.
    */
    static void const *GetStaticKey()
    {
        static char value;
        return &value;
    }

    /** Get the key for the class table.

        The class table holds the data members, properties, and member functions
        of a class. Read-only data and properties, and const member functions are
        also placed here (to save a lookup in the const table).
    */
    static void const *GetClassKey()
    {
        static char value;
        return &value;
    }

    /** Get the key for the const table.

        The const table holds read-only data members and properties, and const
        member functions of a class.
    */
    static void const *GetConstKey()
    {
        static char value;
        return &value;
    }
};
} // namespace luabridge

#endif
