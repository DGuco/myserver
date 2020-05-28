/*
    AGM::LibReflection is a C++ reflection library.
    Version 0.8.
    Copyright (C) 2004 Achilleas Margaritis

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef AGM_REFLECTION_HPP
#define AGM_REFLECTION_HPP


#pragma warning (disable: 4786)
#pragma warning (disable: 4003)

#include <stddef.h>
#include <map>
#include <list>
#include <string>
#include <typeinfo>
#include <iostream>
#include <stdexcept>


//repeaters
#define __REPEAT0(M, C, S)         
#define __REPEAT1(M, C, S)         S M(1)
#define __REPEAT2(M, C, S)         __REPEAT1(M, C, S)  C M(2)
#define __REPEAT3(M, C, S)         __REPEAT2(M, C, S)  C M(3)
#define __REPEAT4(M, C, S)         __REPEAT3(M, C, S)  C M(4)
#define __REPEAT5(M, C, S)         __REPEAT4(M, C, S)  C M(5)
#define __REPEAT6(M, C, S)         __REPEAT5(M, C, S)  C M(6)
#define __REPEAT7(M, C, S)         __REPEAT6(M, C, S)  C M(7)
#define __REPEAT8(M, C, S)         __REPEAT7(M, C, S)  C M(8)
#define __REPEAT9(M, C, S)         __REPEAT8(M, C, S)  C M(9)
#define __REPEAT10(M, C, S)        __REPEAT9(M, C, S)  C M(10)
#define __REPEAT11(M, C, S)        __REPEAT10(M, C, S) C M(11)
#define __REPEAT12(M, C, S)        __REPEAT11(M, C, S) C M(12)
#define __REPEAT13(M, C, S)        __REPEAT12(M, C, S) C M(13)
#define __REPEAT14(M, C, S)        __REPEAT13(M, C, S) C M(14)
#define __REPEAT15(M, C, S)        __REPEAT14(M, C, S) C M(15)
#define __REPEAT16(M, C, S)        __REPEAT15(M, C, S) C M(16)
#define __REPEAT17(M, C, S)        __REPEAT16(M, C, S) C M(17)
#define __REPEAT18(M, C, S)        __REPEAT17(M, C, S) C M(18)
#define __REPEAT19(M, C, S)        __REPEAT18(M, C, S) C M(19)
#define __REPEAT20(M, C, S)        __REPEAT19(M, C, S) C M(20)
#define __REPEAT(N, M, C, S)       __REPEAT##N(M, C, S)


//various defs needed for parameters
#define __MAX_PARAMS__       20
#define __NOTHING__          
#define __COMMA__            ,
#define __TEMPLATE_ARG__(N)  class T##N
#define __TYPE_ARG__(N)      T##N 
#define __ARG__(N)           T##N t##N
#define __PARAM__(N)         t##N
#define __NOT_VIRTUAL__


//calculates the offset of a field
#define __OFFSET__(C, M) offsetof(C,M)

namespace agm { namespace reflection {


//root of all callables
struct __callable__ {
    virtual ~__callable__() {
    }
};


//callable class macro with return type
#define __CALLABLE__(N)\
template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable##N##__ : public __callable__ {\
    typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
    MethodType method;\
    __callable##N##__(MethodType m) : method(m) {\
    }\
    R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//callable class macro with return type and const type
#define __CALLABLE_CONST__(N)\
template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_const##N##__ : public __callable__ {\
    typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const;\
    MethodType method;\
    __callable_const##N##__(MethodType m) : method(m) {\
    }\
    R invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        return (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//callable class macro with void return type
#define __CALLABLE_VOID__(N)\
template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_void##N##__ : public __callable__ {\
    typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
    MethodType method;\
    __callable_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//callable class macro with void return type and const type
#define __CALLABLE_CONST_VOID__(N)\
template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __callable_const_void##N##__ : public __callable__ {\
    typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const;\
    MethodType method;\
    __callable_const_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        (object->*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//static callable class macro with return type
#define __STATIC_CALLABLE__(N)\
template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> struct __static_callable##N##__ : public __callable__ {\
    typedef R (*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
    MethodType method;\
    __static_callable##N##__(MethodType m) : method(m) {\
    }\
    R invoke(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__)) const {\
        return (*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//void version for 0 params
struct __static_callable_void0__ : public __callable__ {
    typedef void (*MethodType)();
    MethodType method;
    __static_callable_void0__(MethodType m) : method(m) {
    }
    void invoke() const {
        (*method)();
    }
};


//void version
#define __STATIC_CALLABLE_VOID__(N)\
template <__REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __NOTHING__)> struct __static_callable_void##N##__ : public __callable__ {\
    typedef void (*MethodType)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__));\
    MethodType method;\
    __static_callable_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__)) const {\
        (*method)(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
    }\
};


//define callables
__CALLABLE__(0);
__CALLABLE__(1);
__CALLABLE__(2);
__CALLABLE__(3);
__CALLABLE__(4);
__CALLABLE__(5);
__CALLABLE__(6);
__CALLABLE__(7);
__CALLABLE__(8);
__CALLABLE__(9);
__CALLABLE__(10);
__CALLABLE__(11);
__CALLABLE__(12);
__CALLABLE__(13);
__CALLABLE__(14);
__CALLABLE__(15);
__CALLABLE__(16);
__CALLABLE__(17);
__CALLABLE__(18);
__CALLABLE__(19);
__CALLABLE__(20);


//define const callables
__CALLABLE_CONST__(0);
__CALLABLE_CONST__(1);
__CALLABLE_CONST__(2);
__CALLABLE_CONST__(3);
__CALLABLE_CONST__(4);
__CALLABLE_CONST__(5);
__CALLABLE_CONST__(6);
__CALLABLE_CONST__(7);
__CALLABLE_CONST__(8);
__CALLABLE_CONST__(9);
__CALLABLE_CONST__(10);
__CALLABLE_CONST__(11);
__CALLABLE_CONST__(12);
__CALLABLE_CONST__(13);
__CALLABLE_CONST__(14);
__CALLABLE_CONST__(15);
__CALLABLE_CONST__(16);
__CALLABLE_CONST__(17);
__CALLABLE_CONST__(18);
__CALLABLE_CONST__(19);
__CALLABLE_CONST__(20);


//define void callables
__CALLABLE_VOID__(0);
__CALLABLE_VOID__(1);
__CALLABLE_VOID__(2);
__CALLABLE_VOID__(3);
__CALLABLE_VOID__(4);
__CALLABLE_VOID__(5);
__CALLABLE_VOID__(6);
__CALLABLE_VOID__(7);
__CALLABLE_VOID__(8);
__CALLABLE_VOID__(9);
__CALLABLE_VOID__(10);
__CALLABLE_VOID__(11);
__CALLABLE_VOID__(12);
__CALLABLE_VOID__(13);
__CALLABLE_VOID__(14);
__CALLABLE_VOID__(15);
__CALLABLE_VOID__(16);
__CALLABLE_VOID__(17);
__CALLABLE_VOID__(18);
__CALLABLE_VOID__(19);
__CALLABLE_VOID__(20);


//define const void callables
__CALLABLE_CONST_VOID__(0);
__CALLABLE_CONST_VOID__(1);
__CALLABLE_CONST_VOID__(2);
__CALLABLE_CONST_VOID__(3);
__CALLABLE_CONST_VOID__(4);
__CALLABLE_CONST_VOID__(5);
__CALLABLE_CONST_VOID__(6);
__CALLABLE_CONST_VOID__(7);
__CALLABLE_CONST_VOID__(8);
__CALLABLE_CONST_VOID__(9);
__CALLABLE_CONST_VOID__(10);
__CALLABLE_CONST_VOID__(11);
__CALLABLE_CONST_VOID__(12);
__CALLABLE_CONST_VOID__(13);
__CALLABLE_CONST_VOID__(14);
__CALLABLE_CONST_VOID__(15);
__CALLABLE_CONST_VOID__(16);
__CALLABLE_CONST_VOID__(17);
__CALLABLE_CONST_VOID__(18);
__CALLABLE_CONST_VOID__(19);
__CALLABLE_CONST_VOID__(20);


//static callable classes
__STATIC_CALLABLE__(0)
__STATIC_CALLABLE__(1)
__STATIC_CALLABLE__(2)
__STATIC_CALLABLE__(3)
__STATIC_CALLABLE__(4)
__STATIC_CALLABLE__(5)
__STATIC_CALLABLE__(6)
__STATIC_CALLABLE__(7)
__STATIC_CALLABLE__(8)
__STATIC_CALLABLE__(9)
__STATIC_CALLABLE__(10)
__STATIC_CALLABLE__(11)
__STATIC_CALLABLE__(12)
__STATIC_CALLABLE__(13)
__STATIC_CALLABLE__(14)
__STATIC_CALLABLE__(15)
__STATIC_CALLABLE__(16)
__STATIC_CALLABLE__(17)
__STATIC_CALLABLE__(18)
__STATIC_CALLABLE__(19)
__STATIC_CALLABLE__(20)
__STATIC_CALLABLE_VOID__(1)
__STATIC_CALLABLE_VOID__(2)
__STATIC_CALLABLE_VOID__(3)
__STATIC_CALLABLE_VOID__(4)
__STATIC_CALLABLE_VOID__(5)
__STATIC_CALLABLE_VOID__(6)
__STATIC_CALLABLE_VOID__(7)
__STATIC_CALLABLE_VOID__(8)
__STATIC_CALLABLE_VOID__(9)
__STATIC_CALLABLE_VOID__(10)
__STATIC_CALLABLE_VOID__(11)
__STATIC_CALLABLE_VOID__(12)
__STATIC_CALLABLE_VOID__(13)
__STATIC_CALLABLE_VOID__(14)
__STATIC_CALLABLE_VOID__(15)
__STATIC_CALLABLE_VOID__(16)
__STATIC_CALLABLE_VOID__(17)
__STATIC_CALLABLE_VOID__(18)
__STATIC_CALLABLE_VOID__(19)
__STATIC_CALLABLE_VOID__(20)


//macro of a inline method that accepts a method pointer and creates a callable for it
#define __CREATE_CALLABLE__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> static inline __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return new __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)>(method);\
    }


//macro of a inline method that accepts a method pointer and creates a const callable for it
#define __CREATE_CALLABLE_CONST__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> static inline __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const) {\
        return new __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)>(method);\
    }


//void version
#define __CREATE_CALLABLE_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> static inline __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return new __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)>(method);\
    }


//const void version
#define __CREATE_CALLABLE_CONST_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> static inline __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const) {\
        return new __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)>(method);\
    }


//macro to create a static callable
#define __CREATE_STATIC_CALLABLE__(N)\
    template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> static inline __static_callable##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> *create(R (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return new __static_callable##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)>(method);\
    }


//void version for 0 params
#define __CREATE_STATIC_CALLABLE_VOID__0\
    static inline __static_callable_void0__ *create(void (*method)()) {\
        return new __static_callable_void0__(method);\
    }


//void version
#define __CREATE_STATIC_CALLABLE_VOID__(N)\
    template <__REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __NOTHING__)> static inline __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)> *create(void (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return new __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)>(method);\
    }


//factory method for non-voids
template <class R> struct __callable_factory__ {
    __CREATE_CALLABLE__(0)
    __CREATE_CALLABLE__(1)
    __CREATE_CALLABLE__(2)
    __CREATE_CALLABLE__(3)
    __CREATE_CALLABLE__(4)
    __CREATE_CALLABLE__(5)
    __CREATE_CALLABLE__(6)
    __CREATE_CALLABLE__(7)
    __CREATE_CALLABLE__(8)
    __CREATE_CALLABLE__(9)
    __CREATE_CALLABLE__(10)
    __CREATE_CALLABLE__(11)
    __CREATE_CALLABLE__(12)
    __CREATE_CALLABLE__(13)
    __CREATE_CALLABLE__(14)
    __CREATE_CALLABLE__(15)
    __CREATE_CALLABLE__(16)
    __CREATE_CALLABLE__(17)
    __CREATE_CALLABLE__(18)
    __CREATE_CALLABLE__(19)
    __CREATE_CALLABLE__(20)
    __CREATE_CALLABLE_CONST__(0)
    __CREATE_CALLABLE_CONST__(1)
    __CREATE_CALLABLE_CONST__(2)
    __CREATE_CALLABLE_CONST__(3)
    __CREATE_CALLABLE_CONST__(4)
    __CREATE_CALLABLE_CONST__(5)
    __CREATE_CALLABLE_CONST__(6)
    __CREATE_CALLABLE_CONST__(7)
    __CREATE_CALLABLE_CONST__(8)
    __CREATE_CALLABLE_CONST__(9)
    __CREATE_CALLABLE_CONST__(10)
    __CREATE_CALLABLE_CONST__(11)
    __CREATE_CALLABLE_CONST__(12)
    __CREATE_CALLABLE_CONST__(13)
    __CREATE_CALLABLE_CONST__(14)
    __CREATE_CALLABLE_CONST__(15)
    __CREATE_CALLABLE_CONST__(16)
    __CREATE_CALLABLE_CONST__(17)
    __CREATE_CALLABLE_CONST__(18)
    __CREATE_CALLABLE_CONST__(19)
    __CREATE_CALLABLE_CONST__(20)
};


//factory method for non-voids
template <> struct __callable_factory__<void> {
    __CREATE_CALLABLE_VOID__(0)
    __CREATE_CALLABLE_VOID__(1)
    __CREATE_CALLABLE_VOID__(2)
    __CREATE_CALLABLE_VOID__(3)
    __CREATE_CALLABLE_VOID__(4)
    __CREATE_CALLABLE_VOID__(5)
    __CREATE_CALLABLE_VOID__(6)
    __CREATE_CALLABLE_VOID__(7)
    __CREATE_CALLABLE_VOID__(8)
    __CREATE_CALLABLE_VOID__(9)
    __CREATE_CALLABLE_VOID__(10)
    __CREATE_CALLABLE_VOID__(11)
    __CREATE_CALLABLE_VOID__(12)
    __CREATE_CALLABLE_VOID__(13)
    __CREATE_CALLABLE_VOID__(14)
    __CREATE_CALLABLE_VOID__(15)
    __CREATE_CALLABLE_VOID__(16)
    __CREATE_CALLABLE_VOID__(17)
    __CREATE_CALLABLE_VOID__(18)
    __CREATE_CALLABLE_VOID__(19)
    __CREATE_CALLABLE_VOID__(20)
    __CREATE_CALLABLE_CONST_VOID__(0)
    __CREATE_CALLABLE_CONST_VOID__(1)
    __CREATE_CALLABLE_CONST_VOID__(2)
    __CREATE_CALLABLE_CONST_VOID__(3)
    __CREATE_CALLABLE_CONST_VOID__(4)
    __CREATE_CALLABLE_CONST_VOID__(5)
    __CREATE_CALLABLE_CONST_VOID__(6)
    __CREATE_CALLABLE_CONST_VOID__(7)
    __CREATE_CALLABLE_CONST_VOID__(8)
    __CREATE_CALLABLE_CONST_VOID__(9)
    __CREATE_CALLABLE_CONST_VOID__(10)
    __CREATE_CALLABLE_CONST_VOID__(11)
    __CREATE_CALLABLE_CONST_VOID__(12)
    __CREATE_CALLABLE_CONST_VOID__(13)
    __CREATE_CALLABLE_CONST_VOID__(14)
    __CREATE_CALLABLE_CONST_VOID__(15)
    __CREATE_CALLABLE_CONST_VOID__(16)
    __CREATE_CALLABLE_CONST_VOID__(17)
    __CREATE_CALLABLE_CONST_VOID__(18)
    __CREATE_CALLABLE_CONST_VOID__(19)
    __CREATE_CALLABLE_CONST_VOID__(20)
};


//factory method for static non-voids
template <class R> struct __static_callable_factory__ {
    __CREATE_STATIC_CALLABLE__(0)
    __CREATE_STATIC_CALLABLE__(1)
    __CREATE_STATIC_CALLABLE__(2)
    __CREATE_STATIC_CALLABLE__(3)
    __CREATE_STATIC_CALLABLE__(4)
    __CREATE_STATIC_CALLABLE__(5)
    __CREATE_STATIC_CALLABLE__(6)
    __CREATE_STATIC_CALLABLE__(7)
    __CREATE_STATIC_CALLABLE__(8)
    __CREATE_STATIC_CALLABLE__(9)
    __CREATE_STATIC_CALLABLE__(10)
    __CREATE_STATIC_CALLABLE__(11)
    __CREATE_STATIC_CALLABLE__(12)
    __CREATE_STATIC_CALLABLE__(13)
    __CREATE_STATIC_CALLABLE__(14)
    __CREATE_STATIC_CALLABLE__(15)
    __CREATE_STATIC_CALLABLE__(16)
    __CREATE_STATIC_CALLABLE__(17)
    __CREATE_STATIC_CALLABLE__(18)
    __CREATE_STATIC_CALLABLE__(19)
    __CREATE_STATIC_CALLABLE__(20)
};


//factory method for static voids
template <> struct __static_callable_factory__<void> {
    __CREATE_STATIC_CALLABLE_VOID__0
    __CREATE_STATIC_CALLABLE_VOID__(1)
    __CREATE_STATIC_CALLABLE_VOID__(2)
    __CREATE_STATIC_CALLABLE_VOID__(3)
    __CREATE_STATIC_CALLABLE_VOID__(4)
    __CREATE_STATIC_CALLABLE_VOID__(5)
    __CREATE_STATIC_CALLABLE_VOID__(6)
    __CREATE_STATIC_CALLABLE_VOID__(7)
    __CREATE_STATIC_CALLABLE_VOID__(8)
    __CREATE_STATIC_CALLABLE_VOID__(9)
    __CREATE_STATIC_CALLABLE_VOID__(10)
    __CREATE_STATIC_CALLABLE_VOID__(11)
    __CREATE_STATIC_CALLABLE_VOID__(12)
    __CREATE_STATIC_CALLABLE_VOID__(13)
    __CREATE_STATIC_CALLABLE_VOID__(14)
    __CREATE_STATIC_CALLABLE_VOID__(15)
    __CREATE_STATIC_CALLABLE_VOID__(16)
    __CREATE_STATIC_CALLABLE_VOID__(17)
    __CREATE_STATIC_CALLABLE_VOID__(18)
    __CREATE_STATIC_CALLABLE_VOID__(19)
    __CREATE_STATIC_CALLABLE_VOID__(20)
};


//callable generator
#define __CALLABLE_GENERATOR__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return __callable_factory__<R>::create(method);\
    }


//const callable generator
#define __CALLABLE_GENERATOR_CONST__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)) const) {\
        return __callable_factory__<R>::create(method);\
    }


//static callable generator
#define __STATIC_CALLABLE_GENERATOR__(N)\
    template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> inline __callable__ *__create_static_callable__(R (*method)(__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__))) {\
        return __static_callable_factory__<R>::create(method);\
    }


//generators
__CALLABLE_GENERATOR__(0)
__CALLABLE_GENERATOR__(1)
__CALLABLE_GENERATOR__(2)
__CALLABLE_GENERATOR__(3)
__CALLABLE_GENERATOR__(4)
__CALLABLE_GENERATOR__(5)
__CALLABLE_GENERATOR__(6)
__CALLABLE_GENERATOR__(7)
__CALLABLE_GENERATOR__(8)
__CALLABLE_GENERATOR__(9)
__CALLABLE_GENERATOR__(10)
__CALLABLE_GENERATOR__(11)
__CALLABLE_GENERATOR__(12)
__CALLABLE_GENERATOR__(13)
__CALLABLE_GENERATOR__(14)
__CALLABLE_GENERATOR__(15)
__CALLABLE_GENERATOR__(16)
__CALLABLE_GENERATOR__(17)
__CALLABLE_GENERATOR__(18)
__CALLABLE_GENERATOR__(19)
__CALLABLE_GENERATOR__(20)
__CALLABLE_GENERATOR_CONST__(0)
__CALLABLE_GENERATOR_CONST__(1)
__CALLABLE_GENERATOR_CONST__(2)
__CALLABLE_GENERATOR_CONST__(3)
__CALLABLE_GENERATOR_CONST__(4)
__CALLABLE_GENERATOR_CONST__(5)
__CALLABLE_GENERATOR_CONST__(6)
__CALLABLE_GENERATOR_CONST__(7)
__CALLABLE_GENERATOR_CONST__(8)
__CALLABLE_GENERATOR_CONST__(9)
__CALLABLE_GENERATOR_CONST__(10)
__CALLABLE_GENERATOR_CONST__(11)
__CALLABLE_GENERATOR_CONST__(12)
__CALLABLE_GENERATOR_CONST__(13)
__CALLABLE_GENERATOR_CONST__(14)
__CALLABLE_GENERATOR_CONST__(15)
__CALLABLE_GENERATOR_CONST__(16)
__CALLABLE_GENERATOR_CONST__(17)
__CALLABLE_GENERATOR_CONST__(18)
__CALLABLE_GENERATOR_CONST__(19)
__CALLABLE_GENERATOR_CONST__(20)
__STATIC_CALLABLE_GENERATOR__(0)
__STATIC_CALLABLE_GENERATOR__(1)
__STATIC_CALLABLE_GENERATOR__(2)
__STATIC_CALLABLE_GENERATOR__(3)
__STATIC_CALLABLE_GENERATOR__(4)
__STATIC_CALLABLE_GENERATOR__(5)
__STATIC_CALLABLE_GENERATOR__(6)
__STATIC_CALLABLE_GENERATOR__(7)
__STATIC_CALLABLE_GENERATOR__(8)
__STATIC_CALLABLE_GENERATOR__(9)
__STATIC_CALLABLE_GENERATOR__(10)
__STATIC_CALLABLE_GENERATOR__(11)
__STATIC_CALLABLE_GENERATOR__(12)
__STATIC_CALLABLE_GENERATOR__(13)
__STATIC_CALLABLE_GENERATOR__(14)
__STATIC_CALLABLE_GENERATOR__(15)
__STATIC_CALLABLE_GENERATOR__(16)
__STATIC_CALLABLE_GENERATOR__(17)
__STATIC_CALLABLE_GENERATOR__(18)
__STATIC_CALLABLE_GENERATOR__(19)
__STATIC_CALLABLE_GENERATOR__(20)


//property handler base
struct __property_base__ {
    virtual ~__property_base__() {
    }
};


//property handler
template <class T> struct __property_handler__ : public __property_base__ {
    //get
    virtual T get(const void *object) const = 0;

    //set
    virtual void set(void *object, T value) const = 0;
};


//property handler class
template <class C, class T> struct __property__ : public __property_handler__<T> {
    //type of getters/setters
    typedef T (C::*Getter)() const;
    typedef void (C::*Setter)(T);

    //pointer to member getters/setters
    Getter getter;
    Setter setter;

    //default constructor
    __property__(Getter g, Setter s) : getter(g), setter(s) {
    }

    //get
    virtual T get(const void *object) const {
        const C *o = (const C *)(object);
        return (o->*getter)();
    }

    //set
    virtual void set(void *object, T value) const {
        C *o = (C *)(object);
        (o->*setter)(value);
    }
};


//declares a method
#define __METHOD__(ACCESS_ATTR, VIRTUAL, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
private:\
    struct __method_##METHOD_NAME##__ {\
        __method_##METHOD_NAME##__() {\
            static agm::reflection::__register_method__ reg(__create_callable__(&ClassType::METHOD_NAME), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS, #VIRTUAL);\
        }\
    } __method_##METHOD_NAME##__;\
    friend struct __method_##METHOD_NAME##__;\
ACCESS_ATTR :\
    VIRTUAL RETURN_TYPE METHOD_NAME METHOD_ARGS


//macro that defines an 'invoke' method with a return type
#define __INVOKE__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> void invoke(R &result, C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> CallableType1;\
        typedef const __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> CallableType2;\
        CallableType1 *cb1 = dynamic_cast<CallableType1 *>(m_callable);\
        if (cb1) {\
            result = cb1->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__));\
            return;\
        }\
        CallableType2 *cb2 = dynamic_cast<CallableType2 *>(m_callable);\
        if (cb2) {\
            result = cb2->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//macro that defines an 'invoke' method without a return type
#define __INVOKE_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> void invokeVoid(C *object __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> CallableType1;\
        typedef const __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> CallableType2;\
        CallableType1 *cb1 = dynamic_cast<CallableType1 *>(m_callable);\
        if (cb1) {\
            cb1->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__));\
            return;\
        }\
        CallableType2 *cb2 = dynamic_cast<CallableType2 *>(m_callable);\
        if (cb2) {\
            cb2->invoke(object __REPEAT(N, __PARAM__, __COMMA__, __COMMA__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke void method with 0 params
#define __STATIC_INVOKE__0\
    template <class R> inline void invoke(R &result) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable0__<R> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            result = cb->invoke();\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke non-void method
#define __STATIC_INVOKE__(N)\
    template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __COMMA__)> void invoke(R &result __REPEAT(N, __ARG__, __COMMA__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__, __COMMA__)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            result = cb->invoke(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke void method with 0 params
#define __STATIC_INVOKE_VOID__0\
    inline void invokeVoid() const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable_void0__ CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            cb->invoke();\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke void method
#define __STATIC_INVOKE_VOID__(N)\
    template <__REPEAT(N, __TEMPLATE_ARG__, __COMMA__, __NOTHING__)> void invokeVoid(__REPEAT(N, __ARG__, __COMMA__, __NOTHING__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __COMMA__, __NOTHING__)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            cb->invoke(__REPEAT(N, __PARAM__, __COMMA__, __NOTHING__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


/** the CLASS macro is used to declare reflection support in a class. It must
    be placed on the public part of the class.
    @param CLASS_NAME name of this class.
    @param SUPER_CLASS_NAME name of the super class; if there is no base class,
           pass NullClass.
 */
#define CLASS(CLASS_NAME, SUPER_CLASS_NAME)\
private:\
    typedef CLASS_NAME ClassType;\
    static const agm::reflection::Class *getClassStaticPtr() {\
        static agm::reflection::Class _class(#CLASS_NAME, SUPER_CLASS_NAME::getClassStaticPtr());\
        return &_class;\
    }\
public:\
    static const agm::reflection::Class &getClassStatic() {\
        return *CLASS_NAME::getClassStaticPtr();\
    }\
    virtual const agm::reflection::Class &getClass() const {\
        return *CLASS_NAME::getClassStaticPtr();\
    }


/** The FIELD macro is used to declare a reflected field.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param FIELD_TYPE type of the field
    @param FIELD_NAME name of the field
 */
#define FIELD(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME)\
private:\
    struct __field_##FIELD_NAME##__ {\
        __field_##FIELD_NAME##__() {\
            static agm::reflection::__register_field__ reg(__OFFSET__(ClassType, FIELD_NAME), typeid(FIELD_TYPE), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME);\
        }\
    } __field_##FIELD_NAME##__;\
    friend struct __field_##FIELD_NAME##__;\
ACCESS_ATTR :\
    FIELD_TYPE FIELD_NAME


/** The STATIC_FIELD macro is used to declare a reflected static field.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param FIELD_TYPE type of the field
    @param FIELD_NAME name of the field
 */
#define STATIC_FIELD(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME)\
private:\
    struct __static_field_##FIELD_NAME##__ {\
        __static_field_##FIELD_NAME##__() {\
            static agm::reflection::__register_static_field__ reg((void *)&FIELD_NAME, typeid(FIELD_TYPE), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME);\
        }\
    } __static_field_##FIELD_NAME##__;\
    friend struct __static_field_##FIELD_NAME##__;\
ACCESS_ATTR :\
    static FIELD_TYPE FIELD_NAME


/** The METHOD macro is used to declare a reflected method.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
    __METHOD__(ACCESS_ATTR, __NOT_VIRTUAL__, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)


/** The VIRTUAL_METHOD macro is used to declare a reflected method that is
    virtual, i.e. can be overloaded by subclasses.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param VIRTUAL pass virtual or no_virtual
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define VIRTUAL_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
    __METHOD__(ACCESS_ATTR, virtual, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)


/** The STATIC_METHOD macro is used to declare a reflected static method.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define STATIC_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
private:\
    struct __static_method_##METHOD_NAME##__ {\
        __static_method_##METHOD_NAME##__() {\
            static agm::reflection::__register_static_method__ reg(__create_static_callable__(&ClassType::METHOD_NAME), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS);\
        }\
    } __static_method_##METHOD_NAME##__;\
    friend struct __static_method_##METHOD_NAME##__;\
ACCESS_ATTR :\
    static RETURN_TYPE METHOD_NAME METHOD_ARGS


/** The PROPERTY macro is used to declare an object's property. When used,
    the class must have two private methods in the form of TYPE get()/void set(TYPE)
    for setting and getting the property. By using this macro, the object 
    gets a property interface for managing the value is if it was a field.
    Arithmetic and other operators work as expected. The defined property
    is reflected: it can be examined and set in run-time. Properties do
    not define an access attribute: there are always public. The property
    does not add any extra bytes to the class, besides those needed for reflection.
    @param TYPE type of the property
    @param NAME name of the property
 */
#define PROPERTY(TYPE, NAME)\
private:\
    template <class T> class __property__##NAME {\
    public:\
        typedef __property__##NAME<T> Type;\
        __property__##NAME<T>() {\
            static agm::reflection::__register_property__ reg(new __property__<ClassType, T>(&ClassType::get_##NAME, &ClassType::set_##NAME), getClassStaticPtr(), #TYPE, #NAME);\
        }\
        inline TYPE get() const {\
            return owner()->get_##NAME();\
        }\
        inline void set(TYPE value) {\
            owner()->set_##NAME(value);\
        }\
        inline operator TYPE () const {\
            return get();\
        }\
        inline bool operator == (TYPE value) const {\
            return get() == value;\
        }\
        inline bool operator != (TYPE value) const {\
            return get() != value;\
        }\
        inline bool operator < (TYPE value) const {\
            return get() < value;\
        }\
        inline bool operator > (TYPE value) const {\
            return get() > value;\
        }\
        inline bool operator <= (TYPE value) const {\
            return get() <= value;\
        }\
        inline bool operator >= (TYPE value) const {\
            return get() >= value;\
        }\
        inline Type &operator = (TYPE value) {\
            set(value);\
            return *this;\
        }\
        inline Type &operator = (const Type &prop) {\
            if (&prop != this) set(prop.get());\
            return *this;\
        }\
        inline Type &operator += (TYPE value) {\
            set(get() + value);\
            return *this;\
        }\
        inline Type &operator -= (TYPE value) {\
            set(get() - value);\
            return *this;\
        }\
        inline Type &operator *= (TYPE value) {\
            set(get() * value);\
            return *this;\
        }\
        inline Type &operator /= (TYPE value) {\
            set(get() / value);\
            return *this;\
        }\
        inline Type &operator %= (TYPE value) {\
            set(get() % value);\
            return *this;\
        }\
        inline Type &operator <<= (int bits) {\
            set(get() << bits);\
            return *this;\
        }\
        inline Type &operator >>= (int bits) {\
            set(get() >> bits);\
            return *this;\
        }\
        inline Type &operator &= (TYPE value) {\
            set(get() & value);\
            return *this;\
        }\
        inline Type &operator |= (TYPE value) {\
            set(get() | value);\
            return *this;\
        }\
        inline Type &operator ^= (TYPE value) {\
            set(get() ^ value);\
            return *this;\
        }\
        inline TYPE operator ~() {\
            return ~get();\
        }\
        inline TYPE operator ++() {\
            TYPE val = get();\
            set(val + 1);\
            return val;\
        }\
        inline TYPE operator ++(int i) {\
            TYPE val = get();\
            set(val + 1);\
            return get();\
        }\
        inline TYPE operator --() {\
            TYPE val = get();\
            set(val - 1);\
            return val;\
        }\
        inline TYPE operator --(int i) {\
            TYPE val = get();\
            set(val - 1);\
            return get();\
        }\
    private:\
        ClassType *owner() {\
            return (ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
        const ClassType *owner() const {\
            return (const ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
    };\
    friend class __property__##NAME< TYPE >;\
public:\
    __property__##NAME< TYPE > NAME


/** access type enumeration
 */
enum ACCESS_TYPE {
    ///public access
    ACCESS_PUBLIC,

    ///protected access
    ACCESS_PROTECTED,

    ///private access
    ACCESS_PRIVATE
};


//access synonyms used in reflection
#define ACCESS_public        ACCESS_PUBLIC
#define ACCESS_protected     ACCESS_PROTECTED
#define ACCESS_private       ACCESS_PRIVATE


class Class;


/** Exception thrown when there is a type mismatch.
 */
class TypeMismatchError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    TypeMismatchError(const std::string &what) : runtime_error(what) {
    }
};


/** Exception thrown when there is an illegal access error.
 */
class IllegalAccessError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    IllegalAccessError(const std::string &what) : runtime_error(what) {
    }
};


/** Exception thrown when an unknown field has been requested.
 */
class UnknownFieldError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownFieldError(const std::string &what) : runtime_error(what) {
    }
};


/** Exception thrown when an unknown method has been requested.
 */
class UnknownMethodError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownMethodError(const std::string &what) : runtime_error(what) {
    }
};


/** Exception thrown when an unknown property has been requested.
 */
class UnknownPropertyError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownPropertyError(const std::string &what) : runtime_error(what) {
    }
};


/** Exception thrown when there the superclass of a class was asked to be
    returned but there was no superclass.
 */
class SuperClassError : public std::runtime_error {
public:
    /** the default constructor
        @param what the item that caused the error
     */
    SuperClassError(const std::string &what) : runtime_error(what) {
    }
};


/** The Field class represents an object's field.
 */
class Field {
public:
    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const {
        return *m_class;
    }

    /** returns the field's access.
        @return the field's access.
     */
    const ACCESS_TYPE getAccess() const {
        return m_access;
    }

    /** returns the field's type.
        @return the field's type.
     */
    const char *getType() const {
        return m_type;
    }

    /** returns the field's name.
        @return the field's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** retrieves the field of the given object.
        @param result variable to store the result of the field
        @param object object to set the field of
        @exception TypeMismatchError thrown if the object is of invalid class or 
                   the result is of the wrong type.
     */
    template <class Object, class Value> void get(Value &result, Object *object) const {
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("result");
        result = *(const Value *)(((const char *)object) + m_offset);
    }

    /** sets the field of the given object.
        @param object object to set the field of
        @param value new value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or 
                   the value is of the wrong type.
        @exception IllegalAccessError thrown if the field's access is not public.
     */
    template <class Object, class Value> void set(Object *object, const Value &value) const {
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        *(Value *)(((char *)object) + m_offset) = value;
    }

private:
    const type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    unsigned long m_offset;

    //default constructor
    Field(unsigned long offset, const type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) :
        m_offset(offset),
        m_typeinfo(typeinfo),
        m_class(pclass),
        m_access(access),
        m_type(type), 
        m_name(name) {
    }

    friend class Class;
    friend struct __register_field__;
};


/** The StaticField class represents an object's static field.
 */
class StaticField {
public:
    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const {
        return *m_class;
    }

    /** returns the field's access.
        @return the field's access.
     */
    const ACCESS_TYPE getAccess() const {
        return m_access;
    }

    /** returns the field's type.
        @return the field's type.
     */
    const char *getType() const {
        return m_type;
    }

    /** returns the field's name.
        @return the field's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** retrieves the static field's value.
        @param value variable to store the value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or 
                   the value is of the wrong type.
     */
    template <class Value> void get(Value &value) const {
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        value = *(const Value *)(m_address);
    }

    /** sets the static field's value.
        @param value new value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or 
                   the value is of the wrong type.
        @exception IllegalAccessError thrown if the field's access is not public.
     */
    template <class Value> void set(const Value &value) const {
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        *(Value *)(m_address) = value;
    }

private:
    const type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    void *m_address;

    //default constructor
    StaticField(void *address, const type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) :
        m_address(address),
        m_typeinfo(typeinfo),
        m_class(pclass),
        m_access(access),
        m_type(type), 
        m_name(name) {
    }

    friend class Class;
    friend struct __register_static_field__;
};


/** The Method class represents an object's method.
 */
class Method {
public:
    ///destructor
    ~Method() {
        if (m_callable) delete m_callable;
    }

    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const {
        return *m_class;
    }

    /** returns the method's access.
        @return the method's access.
     */
    const ACCESS_TYPE getAccess() const {
        return m_access;
    }

    /** returns the method's type (return type).
        @return the method's type.
     */
    const char *getType() const {
        return m_type;
    }

    /** returns the method's name.
        @return the method's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** returns the method's id (name + arguments).
        @return the method's name.
     */
    const char *getId() const {
        return m_id.c_str();
    }

    /** returns the method's arguments.
        @return the method's arguments.
     */
    const char *getArgs() const {
        return m_args;
    }

    /** returns true if the method is virtual.
        @return true if the method is virtual.
     */
    bool isVirtual() const {
        return m_virtual;
    }

    /** invokes a method that has a result
        @param result optional variable to store the result (if the method is non-void)
        @param object object to execute the method of
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __INVOKE__(0)
    __INVOKE__(1)
    __INVOKE__(2)
    __INVOKE__(3)
    __INVOKE__(4)
    __INVOKE__(5)
    __INVOKE__(6)
    __INVOKE__(7)
    __INVOKE__(8)
    __INVOKE__(9)
    __INVOKE__(10)
    __INVOKE__(11)
    __INVOKE__(12)
    __INVOKE__(13)
    __INVOKE__(14)
    __INVOKE__(15)
    __INVOKE__(16)
    __INVOKE__(17)
    __INVOKE__(18)
    __INVOKE__(19)
    __INVOKE__(20)

    /** invokes a method that 'returns' void
        @param object object to execute the method of
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __INVOKE_VOID__(0)
    __INVOKE_VOID__(1)
    __INVOKE_VOID__(2)
    __INVOKE_VOID__(3)
    __INVOKE_VOID__(4)
    __INVOKE_VOID__(5)
    __INVOKE_VOID__(6)
    __INVOKE_VOID__(7)
    __INVOKE_VOID__(8)
    __INVOKE_VOID__(9)
    __INVOKE_VOID__(10)
    __INVOKE_VOID__(11)
    __INVOKE_VOID__(12)
    __INVOKE_VOID__(13)
    __INVOKE_VOID__(14)
    __INVOKE_VOID__(15)
    __INVOKE_VOID__(16)
    __INVOKE_VOID__(17)
    __INVOKE_VOID__(18)
    __INVOKE_VOID__(19)
    __INVOKE_VOID__(20)

private:
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    std::string m_id;
    const char *m_type;
    const char *m_name;
    const char *m_args;
    __callable__ *m_callable;
    bool m_virtual:1;

    //default constructor
    Method(const Class *pclass, ACCESS_TYPE access, const char *type, const char *name, const char *args, bool virt) :
        m_class(pclass),
        m_access(access),
        m_type(type), 
        m_name(name),
        m_args(args),
        m_virtual(virt),
        m_callable(0),
        m_id(name) {
        m_id += args;
    }

    friend class Class;
    friend struct __register_method__;
};


/** The StaticMethod class represents an object's static method.
 */
class StaticMethod {
public:
    ///destructor
    ~StaticMethod() {
        if (m_callable) delete m_callable;
    }

    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const {
        return *m_class;
    }

    /** returns the method's access.
        @return the method's access.
     */
    const ACCESS_TYPE getAccess() const {
        return m_access;
    }

    /** returns the method's type (return type).
        @return the method's type.
     */
    const char *getType() const {
        return m_type;
    }

    /** returns the method's name.
        @return the method's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** returns the method's id (name + arguments).
        @return the method's name.
     */
    const char *getId() const {
        return m_id.c_str();
    }

    /** returns the method's arguments.
        @return the method's arguments.
     */
    const char *getArgs() const {
        return m_args;
    }

    /** invokes the non-void method
        @param result optional variable to store the result (if the method is non-void)
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __STATIC_INVOKE__0
    __STATIC_INVOKE__(1)
    __STATIC_INVOKE__(2)
    __STATIC_INVOKE__(3)
    __STATIC_INVOKE__(4)
    __STATIC_INVOKE__(5)
    __STATIC_INVOKE__(6)
    __STATIC_INVOKE__(7)
    __STATIC_INVOKE__(8)
    __STATIC_INVOKE__(9)
    __STATIC_INVOKE__(10)
    __STATIC_INVOKE__(11)
    __STATIC_INVOKE__(12)
    __STATIC_INVOKE__(13)
    __STATIC_INVOKE__(14)
    __STATIC_INVOKE__(15)
    __STATIC_INVOKE__(16)
    __STATIC_INVOKE__(17)
    __STATIC_INVOKE__(18)
    __STATIC_INVOKE__(19)
    __STATIC_INVOKE__(20)

    /** invokes the void method
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __STATIC_INVOKE_VOID__0
    __STATIC_INVOKE_VOID__(1)
    __STATIC_INVOKE_VOID__(2)
    __STATIC_INVOKE_VOID__(3)
    __STATIC_INVOKE_VOID__(4)
    __STATIC_INVOKE_VOID__(5)
    __STATIC_INVOKE_VOID__(6)
    __STATIC_INVOKE_VOID__(7)
    __STATIC_INVOKE_VOID__(8)
    __STATIC_INVOKE_VOID__(9)
    __STATIC_INVOKE_VOID__(10)
    __STATIC_INVOKE_VOID__(11)
    __STATIC_INVOKE_VOID__(12)
    __STATIC_INVOKE_VOID__(13)
    __STATIC_INVOKE_VOID__(14)
    __STATIC_INVOKE_VOID__(15)
    __STATIC_INVOKE_VOID__(16)
    __STATIC_INVOKE_VOID__(17)
    __STATIC_INVOKE_VOID__(18)
    __STATIC_INVOKE_VOID__(19)
    __STATIC_INVOKE_VOID__(20)

private:
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    std::string m_id;
    const char *m_type;
    const char *m_name;
    const char *m_args;
    __callable__ *m_callable;

    //default constructor
    StaticMethod(const Class *pclass, ACCESS_TYPE access, const char *type, const char *name, const char *args) :
        m_class(pclass),
        m_access(access),
        m_type(type), 
        m_name(name),
        m_args(args),
        m_callable(0),
        m_id(name) {
        m_id += args;
    }

    friend class Class;
    friend struct __register_static_method__;
};


/** The Property class holds information about an object's property.
 */
class Property {
public:
    ///destructor
    ~Property() {
        if (m_handler) delete m_handler;
    }

    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const {
        return *m_class;
    }

    /** returns the property's type.
        @return the property's type.
     */
    const char *getType() const {
        return m_type;
    }

    /** returns the property's name.
        @return the property's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** returns the value of the property
        @param result result to store the value to
        @param object object to get the property of
        @exception TypeMismatchError thrown if there is a type mismatch
     */
    template <class Object, class Value> void get(Value &result, const Object *object) const {
        if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
        typedef const __property_handler__<Value> PropertyType;
        PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
        if (!prop) throw TypeMismatchError(m_name);
        result = prop->get((const void *)object);
    }

    /** sets the property's value
        @param object object to set the property of
        @param value value of the object
        @exception TypeMismatchError thrown if there is a type mismatch
     */
    template <class Object, class Value> void set(Object *object, const Value &value) const {
        if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
        typedef const __property_handler__<Value> PropertyType;
        PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
        if (!prop) throw TypeMismatchError(m_name);
        prop->set((void *)object, value);
    }

private:
    const char *m_type;
    const char *m_name;
    __property_base__ *m_handler;
    const Class *m_class;

    //default constructor
    Property(const Class *pclass, const char *type, const char *name) :
        m_class(pclass),
        m_type(type),
        m_name(name),
        m_handler(0) {
    }

    friend class Class;
    friend struct __register_property__;
};


/** The Class class is used as a placeholder for run-time information of an
    an object's class. A class can be used to query an object's fields, 
    methods, properties, events and super class.
 */
class Class {
public:
    ///type of list of fields
    typedef std::list<Field> FieldList;

    ///type of list of static fields
    typedef std::list<StaticField> StaticFieldList;

    ///type of list of methods
    typedef std::list<Method> MethodList;

    ///type of list of static methods
    typedef std::list<StaticMethod> StaticMethodList;

    ///type of list of properties
    typedef std::list<Property> PropertyList;

    //the default constructor
    Class(const char *name, const Class *super) :
        m_name(name),
        m_super(super) {
    }

    /** returns the class name
        @return the class name
     */
    const char *getName() const {
        return m_name;
    }

    /** checks if the class has a superclass
        @return true if the class has a superclass
     */
    const bool hasSuper() const {
        return m_super != 0;
    }

    /** returns the superclass of the class
        @return the superclass of the class; the superclass must not be null
        @exception SuperClassError thrown if there is no super class
     */
    const Class &getSuper() const {
        if (!m_super) throw SuperClassError(m_name);
        return *m_super;
    }

    /** returns the collection of fields
        @return the collection of fields
     */
    const FieldList &getFields() const {
        return m_fields;
    }

    /** returns the collection of static fields
        @return the collection of static fields
     */
    const StaticFieldList &getStaticFields() const {
        return m_staticFields;
    }

    /** returns the list of methods
        @return the list of methods
     */
    const MethodList &getMethods() const {
        return m_methods;
    }

    /** returns the collection of static methods
        @return the collection of static methods
     */
    const StaticMethodList &getStaticMethods() const {
        return m_staticMethods;
    }

    /** returns the collection of properties
        @return the collection of properties
     */
    const PropertyList &getProperties() const {
        return m_properties;
    }

    /** checks if this class is a base class of the given class.
        @param cl class to check against
        @return true if this class is a base class of the given class.
     */
    bool isBase(const Class &cl) const {
        for(const Class *c = cl.m_super; c; c = c->m_super) {
            if (c == this) return true;
        }
        return false;
    }

    /** checks if this class is the super class of the given class.
        @param cl class to check against
        @return true if this class is the super class of the given class.
     */
    bool isSuper(const Class &cl) const {
        return cl.m_super == this;
    }

    /** checks if this class is the same as given class
        @param cl class to check against
     */
    bool isSame(const Class &cl) const {
        return &cl == this;
    }

    /** checks if the given class is relative to this class. In other words,
        it checks if this class is a base class of the given class, or if they
        are the same class or the given class is a base class of this class.
        @param cl class to check against
        @return true if this class is a base class of the given class or the
                same class.
     */
    bool isRelative(const Class &cl) const {
        return isSame(cl) || isBase(cl) || cl.isBase(*this);
    }

    /** returns field from name
        @param name name of the field to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the field of given name
        @exception UnknownFieldError
     */
    const Field &getField(const char *name, bool searchSuper = true) const {
        _FieldMap::const_iterator it = m_fieldMap.find(name);
        if (it == m_fieldMap.end()) {
            if (searchSuper && m_super) return m_super->getField(name, true);
            throw UnknownFieldError(name);
        }
        return *it->second;
    }

    /** returns static field from name
        @param name name of the field to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the field of given name
        @exception UnknownFieldError
     */
    const StaticField &getStaticField(const char *name, bool searchSuper = true) const {
        _StaticFieldMap::const_iterator it = m_staticFieldMap.find(name);
        if (it == m_staticFieldMap.end()) {
            if (searchSuper && m_super) return m_super->getStaticField(name, true);
            throw UnknownFieldError(name);
        }
        return *it->second;
    }

    /** returns method from name
        @param name name of the method to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the method of given id
        @exception UnknownMethodError
     */
    const Method &getMethod(const char *name, bool searchSuper = true) const {
        _MethodMap::const_iterator it = m_methodMap.find(name);
        if (it == m_methodMap.end()) {
            if (searchSuper && m_super) return m_super->getMethod(name, true);
            throw UnknownMethodError(name);
        }
        return *it->second;
    }

    /** returns static method from name
        @param name name of the method to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the method of given name
        @exception UnknownMethodError
     */
    const StaticMethod &getStaticMethod(const char *name, bool searchSuper = true) const {
        _StaticMethodMap::const_iterator it = m_staticMethodMap.find(name);
        if (it == m_staticMethodMap.end()) {
            if (searchSuper && m_super) return m_super->getStaticMethod(name, true);
            throw UnknownMethodError(name);
        }
        return *it->second;
    }

    /** returns property from name
        @param name name of the property to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the property of given name
        @exception UnknownPropertyError
     */
    const Property &getProperty(const char *name, bool searchSuper = true) const {
        _PropertyMap::const_iterator it = m_propertyMap.find(name);
        if (it == m_propertyMap.end()) {
            if (searchSuper && m_super) return m_super->getProperty(name, true);
            throw UnknownPropertyError(name);
        }
        return *it->second;
    }

private:
    typedef std::map<std::string, Field *> _FieldMap;
    typedef std::map<std::string, StaticField *> _StaticFieldMap;
    typedef std::map<std::string, Method *> _MethodMap;
    typedef std::map<std::string, StaticMethod *> _StaticMethodMap;
    typedef std::map<std::string, Property *> _PropertyMap;
    const char *m_name;
    const Class *m_super;
    FieldList m_fields;
    _FieldMap m_fieldMap;
    StaticFieldList m_staticFields;
    _StaticFieldMap m_staticFieldMap;
    MethodList m_methods;
    _MethodMap m_methodMap;
    StaticMethodList m_staticMethods;
    _StaticMethodMap m_staticMethodMap;
    PropertyList m_properties;
    _PropertyMap m_propertyMap;

    //adds a field
    void _addField(const Field &field) {
        m_fields.push_back(field);
        m_fieldMap[field.m_name] = &m_fields.back();
    }

    //adds a static field
    void _addStaticField(const StaticField &field) {
        m_staticFields.push_back(field);
        m_staticFieldMap[field.m_name] = &m_staticFields.back();
    }

    //adds a method
    void _addMethod(const Method &method, __callable__ *cb) {
        m_methods.push_back(method);
        Method &m = m_methods.back();
        m.m_callable = cb;
        m_methodMap[method.m_name] = &m;
    }

    //adds a static method
    void _addStaticMethod(const StaticMethod &method, __callable__ *cb) {
        m_staticMethods.push_back(method);
        StaticMethod &m = m_staticMethods.back();
        m.m_callable = cb;
        m_staticMethodMap[method.m_name] = &m;
    }

    //adds a property
    void _addProperty(const Property &prop, __property_base__ *handler) {
        m_properties.push_back(prop);
        Property &p = m_properties.back();
        p.m_handler = handler;
        m_propertyMap[prop.m_name] = &p;
    }

    friend struct __register_field__;
    friend struct __register_static_field__;
    friend struct __register_method__;
    friend struct __register_static_method__;
    friend struct __register_property__;
};


/** Pseudo-class used when a class does not inherit from another class. It
    can not be instantiated.
 */
class NullClass {
public:
    static const Class *getClassStaticPtr() { return 0; }
private:
    NullClass() {}
    ~NullClass() {}
};


//internal class for registering a field
struct __register_field__ {
    __register_field__(unsigned long offset, const type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) {
        Field field(offset, typeinfo, pclass, access, type, name);
        ((Class *)pclass)->_addField(field);
    }
};


//internal class for registering a static field
struct __register_static_field__ {
    __register_static_field__(void *address, const type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) {
        StaticField field(address, typeinfo, pclass, access, type, name);
        ((Class *)pclass)->_addStaticField(field);
    }
};


//internal class for registering a method
struct __register_method__ {
    __register_method__(__callable__ *cb, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name, const char *args, const char *virt) {
        Method method(pclass, access, type, name, args, virt[0] == 'v' ? true : false);
        ((Class *)pclass)->_addMethod(method, cb);
    }
};


//internal class for registering a static method
struct __register_static_method__ {
    __register_static_method__(__callable__ *cb, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name, const char *args) {
        StaticMethod method(pclass, access, type, name, args);
        ((Class *)pclass)->_addStaticMethod(method, cb);
    }
};


//internal class for registering a property
struct __register_property__ {
    __register_property__(__property_base__ *handler, const Class *pclass, const char *type, const char *name) {
        Property property(pclass, type, name);
        ((Class *)pclass)->_addProperty(property, handler);
    }
};


}}


///alias to Null class for reflection
typedef agm::reflection::NullClass NullClass;


/** operator that outputs the access type to the given stream
    @param str stream to output the access to
    @param access access type to output
    @return the given stream
    @exception std::range_error thrown if the access value is invalid
 */
inline std::ostream &operator << (std::ostream &str, agm::reflection::ACCESS_TYPE access)
{
    switch (access) {
        case agm::reflection::ACCESS_PUBLIC:
            str << "public";
            break;

        case agm::reflection::ACCESS_PROTECTED:
            str << "protected";
            break;

        case agm::reflection::ACCESS_PRIVATE:
            str << "private";
            break;

        default:
            throw std::range_error("access");
    }

    return str;
}


#endif //AGM_REFLECTION_HPP
