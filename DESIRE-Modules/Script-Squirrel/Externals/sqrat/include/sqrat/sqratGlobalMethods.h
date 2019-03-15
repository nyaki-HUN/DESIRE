#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratGlobalMethods: Global Methods
//

//
// Copyright (c) 2009 Brandon Jones
// Copyirght 2011 Li-Cheng (Andy) Tai
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//  distribution.
//

#if !defined(_SCRAT_GLOBAL_METHODS_H_)
#define _SCRAT_GLOBAL_METHODS_H_

#include <squirrel.h>
#include "sqratTypes.h"

namespace Sqrat {

/// @cond DEV

//
// Squirrel Global Functions
//
template <class R>
class SqGlobal {
public:

    // Arg Count 0
    template <bool overloaded /*= false*/>
    static SQInteger Func0(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)();
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        R ret = (*method)();
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 1
    template <class A1, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func1(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 1) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 2
    template <class A1, class A2, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func2(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1, A2);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value,
                    a2.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 3
    template <class A1, class A2, class A3, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func3(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 3) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1, A2, A3);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 4
    template <class A1, class A2, class A3, class A4, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func4(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 4) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1, A2, A3, A4);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 5
    template <class A1, class A2, class A3, class A4, class A5, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func5(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 5) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1, A2, A3, A4, A5);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value,
                    a5.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 6
    template <class A1, class A2, class A3, class A4, class A5, class A6, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func6(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 6) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R (*M)(A1, A2, A3, A4, A5, A6);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        Var<A6> a6(vm, startIdx + 5);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value,
                    a5.value,
                    a6.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVar(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }
};


//
// reference return specialization
//

template <class R>
class SqGlobal<R&> {
public:

    // Arg Count 0
    template <bool overloaded /*= false*/>
    static SQInteger Func0(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)();
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        R& ret = (*method)();
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 1
    template <class A1, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func1(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 1) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 2
    template <class A1, class A2, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func2(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1, A2);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value,
                    a2.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 3
    template <class A1, class A2, class A3, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func3(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 3) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1, A2, A3);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 4
    template <class A1, class A2, class A3, class A4, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func4(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 4) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1, A2, A3, A4);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 5
    template <class A1, class A2, class A3, class A4, class A5, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func5(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 5) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1, A2, A3, A4, A5);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value,
                    a5.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }

    // Arg Count 6
    template <class A1, class A2, class A3, class A4, class A5, class A6, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func6(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 6) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef R& (*M)(A1, A2, A3, A4, A5, A6);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        Var<A6> a6(vm, startIdx + 5);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        R& ret = (*method)(
                    a1.value,
                    a2.value,
                    a3.value,
                    a4.value,
                    a5.value,
                    a6.value
                );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        PushVarR(vm, ret);
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 1;
    }
};


//
// void return specialization
//

template <>
class SqGlobal<void> {
public:

    // Arg Count 0
    template <bool overloaded /*= false*/>
    static SQInteger Func0(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)();
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        (*method)();
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 1
    template <class A1, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func1(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 1) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 2
    template <class A1, class A2, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func2(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 2) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1, A2);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value,
            a2.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 3
    template <class A1, class A2, class A3, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func3(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 3) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1, A2, A3);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value,
            a2.value,
            a3.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 4
    template <class A1, class A2, class A3, class A4, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func4(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 4) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1, A2, A3, A4);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value,
            a2.value,
            a3.value,
            a4.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 5
    template <class A1, class A2, class A3, class A4, class A5, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func5(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 5) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1, A2, A3, A4, A5);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value,
            a2.value,
            a3.value,
            a4.value,
            a5.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }

    // Arg Count 6
    template <class A1, class A2, class A3, class A4, class A5, class A6, SQInteger startIdx, bool overloaded /*= false*/>
    static SQInteger Func6(HSQUIRRELVM vm) {

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != startIdx + 6) {
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#endif

        typedef void (*M)(A1, A2, A3, A4, A5, A6);
        M* method;
        sq_getuserdata(vm, -1, (SQUserPointer*)&method, NULL);

        SQTRY()
        Var<A1> a1(vm, startIdx);
        Var<A2> a2(vm, startIdx + 1);
        Var<A3> a3(vm, startIdx + 2);
        Var<A4> a4(vm, startIdx + 3);
        Var<A5> a5(vm, startIdx + 4);
        Var<A6> a6(vm, startIdx + 5);
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        (*method)(
            a1.value,
            a2.value,
            a3.value,
            a4.value,
            a5.value,
            a6.value
        );
        SQCATCH_NOEXCEPT(vm) {
            return sq_throwerror(vm, SQWHAT_NOEXCEPT(vm));
        }
        SQCATCH(vm) {
            return sq_throwerror(vm, SQWHAT(vm));
        }
        return 0;
    }
};


//
// Global Function Resolvers
//

// Arg Count 0
template <class R>
SQFUNCTION SqGlobalFunc(R (* /*method*/)()) {
    return &SqGlobal<R>::template Func0<false>;
}

// Arg Count 0
template <class R>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)()) {
    return &SqGlobal<R&>::template Func0<false>;
}

// Arg Count 1
template <class R, class A1>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1)) {
    return &SqGlobal<R>::template Func1<A1, 2, false>;
}

// Arg Count 1
template <class R, class A1>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1)) {
    return &SqGlobal<R&>::template Func1<A1, 2, false>;
}

// Arg Count 2
template <class R, class A1, class A2>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1, A2)) {
    return &SqGlobal<R>::template Func2<A1, A2, 2, false>;
}

// Arg Count 2
template <class R, class A1, class A2>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1, A2)) {
    return &SqGlobal<R&>::template Func2<A1, A2, 2, false>;
}

// Arg Count 3
template <class R, class A1, class A2, class A3>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1, A2, A3)) {
    return &SqGlobal<R>::template Func3<A1, A2, A3, 2, false>;
}

// Arg Count 3
template <class R, class A1, class A2, class A3>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1, A2, A3)) {
    return &SqGlobal<R&>::template Func3<A1, A2, A3, 2, false>;
}

// Arg Count 4
template <class R, class A1, class A2, class A3, class A4>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1, A2, A3, A4)) {
    return &SqGlobal<R>::template Func4<A1, A2, A3, A4, 2, false>;
}

// Arg Count 4
template <class R, class A1, class A2, class A3, class A4>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1, A2, A3, A4)) {
    return &SqGlobal<R&>::template Func4<A1, A2, A3, A4, 2, false>;
}

// Arg Count 5
template <class R, class A1, class A2, class A3, class A4, class A5>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqGlobal<R>::template Func5<A1, A2, A3, A4, A5, 2, false>;
}

// Arg Count 5
template <class R, class A1, class A2, class A3, class A4, class A5>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqGlobal<R&>::template Func5<A1, A2, A3, A4, A5, 2, false>;
}

// Arg Count 6
template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
SQFUNCTION SqGlobalFunc(R (* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqGlobal<R>::template Func6<A1, A2, A3, A4, A5, A6, 2, false>;
}

// Arg Count 6
template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
SQFUNCTION SqGlobalFunc(R& (* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqGlobal<R&>::template Func6<A1, A2, A3, A4, A5, A6, 2, false>;
}

/// @endcond

}

#endif
