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

#define __REPEAT_A0					
#define __REPEAT_A1					, class T1
#define __REPEAT_A2					, class T1, class T2
#define __REPEAT_A3					, class T1, class T2, class T3
#define __REPEAT_A4					, class T1, class T2, class T3, class T4
#define __REPEAT_A5					, class T1, class T2, class T3, class T4, class T5
#define __REPEAT_A6					, class T1, class T2, class T3, class T4, class T5, class T6
#define __REPEAT_A7					, class T1, class T2, class T3, class T4, class T5, class T6, class T7
#define __REPEAT_A8					, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8
#define __REPEAT_A9					, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9
#define __REPEAT_A10				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10
#define __REPEAT_A11				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11
#define __REPEAT_A12				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12
#define __REPEAT_A13				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13
#define __REPEAT_A14				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14
#define __REPEAT_A15				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15
#define __REPEAT_A16				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16
#define __REPEAT_A17				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17
#define __REPEAT_A18				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18
#define __REPEAT_A19				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19
#define __REPEAT_A20				, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20
#define __REPEAT_A(N)       __REPEAT_A##N

#define __REPEAT_B0					
#define __REPEAT_B1					T1
#define __REPEAT_B2					T1, T2
#define __REPEAT_B3					T1, T2, T3
#define __REPEAT_B4					T1, T2, T3, T4
#define __REPEAT_B5					T1, T2, T3, T4, T5
#define __REPEAT_B6					T1, T2, T3, T4, T5, T6
#define __REPEAT_B7					T1, T2, T3, T4, T5, T6, T7
#define __REPEAT_B8					T1, T2, T3, T4, T5, T6, T7, T8
#define __REPEAT_B9					T1, T2, T3, T4, T5, T6, T7, T8, T9
#define __REPEAT_B10				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10
#define __REPEAT_B11				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11
#define __REPEAT_B12				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
#define __REPEAT_B13				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13
#define __REPEAT_B14				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14
#define __REPEAT_B15				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
#define __REPEAT_B16				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16
#define __REPEAT_B17				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17
#define __REPEAT_B18				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18
#define __REPEAT_B19				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19
#define __REPEAT_B20				T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20
#define __REPEAT_B(N)       __REPEAT_B##N

#define __REPEAT_C0					
#define __REPEAT_C1					, T1 t1
#define __REPEAT_C2					, T1 t1, T2 t2
#define __REPEAT_C3					, T1 t1, T2 t2, T3 t3
#define __REPEAT_C4					, T1 t1, T2 t2, T3 t3, T4 t4
#define __REPEAT_C5					, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5
#define __REPEAT_C6					, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6
#define __REPEAT_C7					, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7
#define __REPEAT_C8					, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8
#define __REPEAT_C9					, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9
#define __REPEAT_C10				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10
#define __REPEAT_C11				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11
#define __REPEAT_C12				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12
#define __REPEAT_C13				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13
#define __REPEAT_C14				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14
#define __REPEAT_C15				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15
#define __REPEAT_C16				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16
#define __REPEAT_C17				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17
#define __REPEAT_C18				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18
#define __REPEAT_C19				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18, T19 t19
#define __REPEAT_C20				, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18, T19 t19, T20 t20
#define __REPEAT_C(N)       __REPEAT_C##N

#define __REPEAT_D0					
#define __REPEAT_D1					t1
#define __REPEAT_D2					t1, t2
#define __REPEAT_D3					t1, t2, t3
#define __REPEAT_D4					t1, t2, t3, t4
#define __REPEAT_D5					t1, t2, t3, t4, t5
#define __REPEAT_D6					t1, t2, t3, t4, t5, t6
#define __REPEAT_D7					t1, t2, t3, t4, t5, t6, t7
#define __REPEAT_D8					t1, t2, t3, t4, t5, t6, t7, t8
#define __REPEAT_D9					t1, t2, t3, t4, t5, t6, t7, t8, t9
#define __REPEAT_D10				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10
#define __REPEAT_D11				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11
#define __REPEAT_D12				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12
#define __REPEAT_D13				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13
#define __REPEAT_D14				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14
#define __REPEAT_D15				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15
#define __REPEAT_D16				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16
#define __REPEAT_D17				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17
#define __REPEAT_D18				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18
#define __REPEAT_D19				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19
#define __REPEAT_D20				t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20
#define __REPEAT_D(N)       __REPEAT_D##N

#define __REPEAT_E0					
#define __REPEAT_E1					T1 t1
#define __REPEAT_E2					T1 t1, T2 t2
#define __REPEAT_E3					T1 t1, T2 t2, T3 t3
#define __REPEAT_E4					T1 t1, T2 t2, T3 t3, T4 t4
#define __REPEAT_E5					T1 t1, T2 t2, T3 t3, T4 t4, T5 t5
#define __REPEAT_E6					T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6
#define __REPEAT_E7					T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7
#define __REPEAT_E8					T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8
#define __REPEAT_E9					T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9
#define __REPEAT_E10				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10
#define __REPEAT_E11				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11
#define __REPEAT_E12				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12
#define __REPEAT_E13				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13
#define __REPEAT_E14				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14
#define __REPEAT_E15				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15
#define __REPEAT_E16				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16
#define __REPEAT_E17				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17
#define __REPEAT_E18				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18
#define __REPEAT_E19				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18, T19 t19
#define __REPEAT_E20				T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12, T13 t13, T14 t14, T15 t15, T16 t16, T17 t17, T18 t18, T19 t19, T20 t20
#define __REPEAT_E(N)       __REPEAT_E##N

#define __REPEAT_F0					
#define __REPEAT_F1					class T1
#define __REPEAT_F2					class T1, class T2
#define __REPEAT_F3					class T1, class T2, class T3
#define __REPEAT_F4					class T1, class T2, class T3, class T4
#define __REPEAT_F5					class T1, class T2, class T3, class T4, class T5
#define __REPEAT_F6					class T1, class T2, class T3, class T4, class T5, class T6
#define __REPEAT_F7					class T1, class T2, class T3, class T4, class T5, class T6, class T7
#define __REPEAT_F8					class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8
#define __REPEAT_F9					class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9
#define __REPEAT_F10				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10
#define __REPEAT_F11				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11
#define __REPEAT_F12				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12
#define __REPEAT_F13				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13
#define __REPEAT_F14				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14
#define __REPEAT_F15				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15
#define __REPEAT_F16				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16
#define __REPEAT_F17				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17
#define __REPEAT_F18				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18
#define __REPEAT_F19				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19
#define __REPEAT_F20				class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20
#define __REPEAT_F(N)       __REPEAT_F##N

#define __REPEAT_G0					
#define __REPEAT_G1					, T1
#define __REPEAT_G2					, T1, T2
#define __REPEAT_G3					, T1, T2, T3
#define __REPEAT_G4					, T1, T2, T3, T4
#define __REPEAT_G5					, T1, T2, T3, T4, T5
#define __REPEAT_G6					, T1, T2, T3, T4, T5, T6
#define __REPEAT_G7					, T1, T2, T3, T4, T5, T6, T7
#define __REPEAT_G8					, T1, T2, T3, T4, T5, T6, T7, T8
#define __REPEAT_G9					, T1, T2, T3, T4, T5, T6, T7, T8, T9
#define __REPEAT_G10				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10
#define __REPEAT_G11				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11
#define __REPEAT_G12				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
#define __REPEAT_G13				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13
#define __REPEAT_G14				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14
#define __REPEAT_G15				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
#define __REPEAT_G16				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16
#define __REPEAT_G17				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17
#define __REPEAT_G18				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18
#define __REPEAT_G19				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19
#define __REPEAT_G20				, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20
#define __REPEAT_G(N)       __REPEAT_G##N

#define __REPEAT_H0					
#define __REPEAT_H1					, t1
#define __REPEAT_H2					, t1, t2
#define __REPEAT_H3					, t1, t2, t3
#define __REPEAT_H4					, t1, t2, t3, t4
#define __REPEAT_H5					, t1, t2, t3, t4, t5
#define __REPEAT_H6					, t1, t2, t3, t4, t5, t6
#define __REPEAT_H7					, t1, t2, t3, t4, t5, t6, t7
#define __REPEAT_H8					, t1, t2, t3, t4, t5, t6, t7, t8
#define __REPEAT_H9					, t1, t2, t3, t4, t5, t6, t7, t8, t9
#define __REPEAT_H10				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10
#define __REPEAT_H11				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11
#define __REPEAT_H12				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12
#define __REPEAT_H13				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13
#define __REPEAT_H14				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14
#define __REPEAT_H15				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15
#define __REPEAT_H16				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16
#define __REPEAT_H17				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17
#define __REPEAT_H18				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18
#define __REPEAT_H19				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19
#define __REPEAT_H20				, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20
#define __REPEAT_H(N)       __REPEAT_H##N

//various defs needed for parameters
#define __MAX_PARAMS__       20
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
template <class R, class C __REPEAT_A(N)> struct __callable##N##__ : public __callable__ {\
    typedef R (C::*MethodType)(__REPEAT_B(N));\
    MethodType method;\
    __callable##N##__(MethodType m) : method(m) {\
    }\
    R invoke(C *object __REPEAT_C(N)) const {\
        return (object->*method)(__REPEAT_D(N));\
    }\
};


//callable class macro with return type and const type
#define __CALLABLE_CONST__(N)\
template <class R, class C __REPEAT_A(N)> struct __callable_const##N##__ : public __callable__ {\
    typedef R (C::*MethodType)(__REPEAT_B(N)) const;\
    MethodType method;\
    __callable_const##N##__(MethodType m) : method(m) {\
    }\
    R invoke(C *object __REPEAT_C(N)) const {\
        return (object->*method)(__REPEAT_D(N));\
    }\
};


//callable class macro with void return type
#define __CALLABLE_VOID__(N)\
template <class C __REPEAT_A(N)> struct __callable_void##N##__ : public __callable__ {\
    typedef void (C::*MethodType)(__REPEAT_B(N));\
    MethodType method;\
    __callable_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(C *object __REPEAT_C(N)) const {\
        (object->*method)(__REPEAT_D(N));\
    }\
};


//callable class macro with void return type and const type
#define __CALLABLE_CONST_VOID__(N)\
template <class C __REPEAT_A(N)> struct __callable_const_void##N##__ : public __callable__ {\
    typedef void (C::*MethodType)(__REPEAT_B(N)) const;\
    MethodType method;\
    __callable_const_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(C *object __REPEAT_C(N)) const {\
        (object->*method)(__REPEAT_D(N));\
    }\
};


//static callable class macro with return type
#define __STATIC_CALLABLE__(N)\
template <class R __REPEAT_A(N)> struct __static_callable##N##__ : public __callable__ {\
    typedef R (*MethodType)(__REPEAT_B(N));\
    MethodType method;\
    __static_callable##N##__(MethodType m) : method(m) {\
    }\
    R invoke(__REPEAT_E(N)) const {\
        return (*method)(__REPEAT_D(N));\
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
template <__REPEAT_F(N)> struct __static_callable_void##N##__ : public __callable__ {\
    typedef void (*MethodType)(__REPEAT_B(N));\
    MethodType method;\
    __static_callable_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(__REPEAT_E(N)) const {\
        (*method)(__REPEAT_D(N));\
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
    template <class C __REPEAT_A(N)> static inline __callable##N##__<R, C __REPEAT_G(N)> *create(R (C::*method)(__REPEAT_B(N))) {\
        return new __callable##N##__<R, C __REPEAT_G(N)>(method);\
    }


//macro of a inline method that accepts a method pointer and creates a const callable for it
#define __CREATE_CALLABLE_CONST__(N)\
    template <class C __REPEAT_A(N)> static inline __callable_const##N##__<R, C __REPEAT_G(N)> *create(R (C::*method)(__REPEAT_B(N)) const) {\
        return new __callable_const##N##__<R, C __REPEAT_G(N)>(method);\
    }


//void version
#define __CREATE_CALLABLE_VOID__(N)\
    template <class C __REPEAT_A(N)> static inline __callable_void##N##__<C __REPEAT_G(N)> *create(void (C::*method)(__REPEAT_B(N))) {\
        return new __callable_void##N##__<C __REPEAT_G(N)>(method);\
    }


//const void version
#define __CREATE_CALLABLE_CONST_VOID__(N)\
    template <class C __REPEAT_A(N)> static inline __callable_const_void##N##__<C __REPEAT_G(N)> *create(void (C::*method)(__REPEAT_B(N)) const) {\
        return new __callable_const_void##N##__<C __REPEAT_G(N)>(method);\
    }


//macro to create a static callable
#define __CREATE_STATIC_CALLABLE__(N)\
    template <class R1 __REPEAT_A(N)> static inline __static_callable##N##__<R1 __REPEAT_G(N)> *create(R1 (*method)(__REPEAT_B(N))) {\
        return new __static_callable##N##__<R1 __REPEAT_G(N)>(method);\
    }


//void version for 0 params
#define __CREATE_STATIC_CALLABLE_VOID__0\
    static inline __static_callable_void0__ *create(void (*method)()) {\
        return new __static_callable_void0__(method);\
    }


//void version
#define __CREATE_STATIC_CALLABLE_VOID__(N)\
    template <__REPEAT_F(N)> static inline __static_callable_void##N##__<__REPEAT_B(N)> *create(void (*method)(__REPEAT_B(N))) {\
        return new __static_callable_void##N##__<__REPEAT_B(N)>(method);\
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
    template <class R, class C __REPEAT_A(N)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT_B(N))) {\
        return __callable_factory__<R>::create(method);\
    }


//const callable generator
#define __CALLABLE_GENERATOR_CONST__(N)\
    template <class R, class C __REPEAT_A(N)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT_B(N)) const) {\
        return __callable_factory__<R>::create(method);\
    }


//static callable generator
#define __STATIC_CALLABLE_GENERATOR__(N)\
    template <class R __REPEAT_A(N)> inline __callable__ *__create_static_callable__(R (*method)(__REPEAT_B(N))) {\
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
    template <class R, class C __REPEAT_A(N)> void invoke(R &result, C *object __REPEAT_C(N)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __callable##N##__<R, C __REPEAT_G(N)> CallableType1;\
        typedef const __callable_const##N##__<R, C __REPEAT_G(N)> CallableType2;\
        CallableType1 *cb1 = dynamic_cast<CallableType1 *>(m_callable);\
        if (cb1) {\
            result = cb1->invoke(object __REPEAT_H(N));\
            return;\
        }\
        CallableType2 *cb2 = dynamic_cast<CallableType2 *>(m_callable);\
        if (cb2) {\
            result = cb2->invoke(object __REPEAT_H(N));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//macro that defines an 'invoke' method without a return type
#define __INVOKE_VOID__(N)\
    template <class C __REPEAT_A(N)> void invokeVoid(C *object __REPEAT_C(N)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __callable_void##N##__<C __REPEAT_G(N)> CallableType1;\
        typedef const __callable_const_void##N##__<C __REPEAT_G(N)> CallableType2;\
        CallableType1 *cb1 = dynamic_cast<CallableType1 *>(m_callable);\
        if (cb1) {\
            cb1->invoke(object __REPEAT_H(N));\
            return;\
        }\
        CallableType2 *cb2 = dynamic_cast<CallableType2 *>(m_callable);\
        if (cb2) {\
            cb2->invoke(object __REPEAT_H(N));\
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
    template <class R __REPEAT_A(N)> void invoke(R &result __REPEAT_C(N)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable##N##__<R __REPEAT_G(N)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            result = cb->invoke(__REPEAT_D(N));\
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
    template <__REPEAT_F(N)> void invokeVoid(__REPEAT_E(N)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable_void##N##__<__REPEAT_B(N)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            cb->invoke(__REPEAT_D(N));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


/** the CLASS macro is used to declare reflection support in a class. It must
    be placed on the public part of the class.
    @param CLASS_NAME name of this class.
    @param SUPER_CLASS_NAME name of the super class; if there is no base class,
           pass NullClass.
    
    ������getClassStaticPtr����Ϊprotected���Ա���������Է��ʸ���ĸú���
 */
#define CLASS(CLASS_NAME, SUPER_CLASS_NAME)\
private:\
    typedef CLASS_NAME ClassType;\
protected:\
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
        
        �˴�ֻ����������ʵ�ַŵ�Class��֮��
     */
    template <class Object, class Value> void get(Value &result, Object *object) const;

    /** sets the field of the given object.
        @param object object to set the field of
        @param value new value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or 
                   the value is of the wrong type.
        @exception IllegalAccessError thrown if the field's access is not public.
      	
      	�˴�ֻ����������ʵ�ַŵ�Class��֮��
     */
    template <class Object, class Value> void set(Object *object, const Value &value) const;

private:
		/**type_info�������ռ���std
		*/
    const std::type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    unsigned long m_offset;

    //default constructor
    Field(unsigned long offset, const std::type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) :
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
    const std::type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    void *m_address;

    //default constructor
    StaticField(void *address, const std::type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) :
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
        
        �˴�ֻ����������ʵ�ַŵ�Class��֮��
     */
    template <class Object, class Value> void get(Value &result, const Object *object) const;

    /** sets the property's value
        @param object object to set the property of
        @param value value of the object
        @exception TypeMismatchError thrown if there is a type mismatch
        
        �˴�ֻ����������ʵ�ַŵ�Class��֮��
     */
    template <class Object, class Value> void set(Object *object, const Value &value) const;

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

/**���õ�ʵ�ַ���
*/
template <class Object, class Value> void Field::get(Value &result, Object *object) const {
    if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
    if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
    if (typeid(Value) != m_typeinfo) throw TypeMismatchError("result");
    result = *(const Value *)(((const char *)object) + m_offset);
}

template <class Object, class Value> void Field::set(Object *object, const Value &value) const {
    if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
    if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
    if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
    *(Value *)(((char *)object) + m_offset) = value;
}

template <class Object, class Value> void Property::get(Value &result, const Object *object) const {
    if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
    typedef const __property_handler__<Value> PropertyType;
    PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
    if (!prop) throw TypeMismatchError(m_name);
    result = prop->get((const void *)object);
}

template <class Object, class Value> void Property::set(Object *object, const Value &value) const {
    if (!m_class->isRelative(object->getClass())) throw TypeMismatchError("object");
    typedef const __property_handler__<Value> PropertyType;
    PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
    if (!prop) throw TypeMismatchError(m_name);
    prop->set((void *)object, value);
}


//internal class for registering a field
struct __register_field__ {
    __register_field__(unsigned long offset, const std::type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) {
        Field field(offset, typeinfo, pclass, access, type, name);
        ((Class *)pclass)->_addField(field);
    }
};


//internal class for registering a static field
struct __register_static_field__ {
    __register_static_field__(void *address, const std::type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) {
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


/**��ֹ������ͻ
*/
#if 0
///alias to Null class for reflection
typedef agm::reflection::NullClass NullClass;
#endif


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
