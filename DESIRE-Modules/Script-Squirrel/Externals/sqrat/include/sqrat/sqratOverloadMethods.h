#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratOverloadMethods: Overload Methods
//

//
// Copyright (c) 2009 Brandon Jones
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

#include "squirrel.h"
#include "sqratTypes.h"
#include "sqratUtil.h"
#include "sqratGlobalMethods.h"
#include "sqratMemberMethods.h"

namespace Sqrat {

//
// Overload name generator
//

class SqOverloadName
{
public:
	static DynamicString Get(const SQChar* name, SQInteger args)
	{
		return DynamicString::Format("__overload_%s%d", name, (int)args);
	}
};


//
// Squirrel Overload Functions
//

template <class R>
class SqOverload
{
public:
    static SQInteger Func(HSQUIRRELVM vm)
	{
        // Get the arg count
        SQInteger argCount = sq_gettop(vm) - 2;

        const SQChar* funcName;
        sq_getstring(vm, -1, &funcName); // get the function name (free variable)

        DynamicString overloadName = SqOverloadName::Get(funcName, argCount);

        sq_pushstring(vm, overloadName.Str(), -1);

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (SQ_FAILED(sq_get(vm, 1))) { // Lookup the proper overload
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#else
        sq_get(vm, 1);
#endif

        // Push the args again
        for (int i = 1; i <= argCount + 1; ++i) {
            sq_push(vm, i);
        }

#if !defined (SCRAT_NO_ERROR_CHECKING)
        SQRESULT result = sq_call(vm, argCount + 1, true, true);
        if (SQ_FAILED(result)) {
            return sq_throwerror(vm, LastErrorString(vm).Str());
        }
#else
        sq_call(vm, argCount + 1, true, true);
#endif

        return 1;
    }
};


//
// void return specialization
//

template <>
class SqOverload<void>
{
public:
    static SQInteger Func(HSQUIRRELVM vm) {
        // Get the arg count
		SQInteger argCount = sq_gettop(vm) - 2;

        const SQChar* funcName;
        sq_getstring(vm, -1, &funcName); // get the function name (free variable)

        DynamicString overloadName = SqOverloadName::Get(funcName, argCount);

        sq_pushstring(vm, overloadName.Str(), -1);

#if !defined (SCRAT_NO_ERROR_CHECKING)
        if (SQ_FAILED(sq_get(vm, 1))) { // Lookup the proper overload
            return sq_throwerror(vm, _SC("wrong number of parameters"));
        }
#else
        sq_get(vm, 1);
#endif

        // Push the args again
        for (int i = 1; i <= argCount + 1; ++i) {
            sq_push(vm, i);
        }

#if !defined (SCRAT_NO_ERROR_CHECKING)
        SQRESULT result = sq_call(vm, argCount + 1, false, true);
        if (SQ_FAILED(result)) {
            return sq_throwerror(vm, LastErrorString(vm).Str());
        }
#else
        sq_call(vm, argCount + 1, false, true);
#endif

        return 0;
    }
};


//
// Global Overloaded Function Resolvers
//

// Arg Count 0
template <class R>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)()) {
    return &SqGlobal<R>::template Func0<true>;
}

template <class R>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)()) {
    return &SqGlobal<R&>::template Func0<true>;
}

// Arg Count 1
template <class R, class A1>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1)) {
    return &SqGlobal<R>::template Func1<A1, true>;
}

template <class R, class A1>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1)) {
    return &SqGlobal<R&>::template Func1<A1, true>;
}

// Arg Count 2
template <class R, class A1, class A2>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1, A2)) {
    return &SqGlobal<R>::template Func2<A1, A2, true>;
}

template <class R, class A1, class A2>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1, A2)) {
    return &SqGlobal<R&>::template Func2<A1, A2, true>;
}

// Arg Count 3
template <class R, class A1, class A2, class A3>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1, A2, A3)) {
    return &SqGlobal<R>::template Func3<A1, A2, A3, true>;
}

template <class R, class A1, class A2, class A3>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1, A2, A3)) {
    return &SqGlobal<R&>::template Func3<A1, A2, A3, true>;
}

// Arg Count 4
template <class R, class A1, class A2, class A3, class A4>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1, A2, A3, A4)) {
    return &SqGlobal<R>::template Func4<A1, A2, A3, A4, true>;
}

template <class R, class A1, class A2, class A3, class A4>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1, A2, A3, A4)) {
    return &SqGlobal<R&>::template Func4<A1, A2, A3, A4, true>;
}

// Arg Count 5
template <class R, class A1, class A2, class A3, class A4, class A5>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqGlobal<R>::template Func5<A1, A2, A3, A4, A5, true>;
}

template <class R, class A1, class A2, class A3, class A4, class A5>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqGlobal<R&>::template Func5<A1, A2, A3, A4, A5, true>;
}

// Arg Count 6
template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
SQFUNCTION SqGlobalOverloadedFunc(R (* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqGlobal<R>::template Func6<A1, A2, A3, A4, A5, A6, true>;
}

template <class R, class A1, class A2, class A3, class A4, class A5, class A6>
SQFUNCTION SqGlobalOverloadedFunc(R& (* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqGlobal<R&>::template Func6<A1, A2, A3, A4, A5, A6, true>;
}

//
// Member Overloaded Function Resolvers
//

// Arg Count 0
template <class C, class R>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)()) {
    return &SqMember<C, R>::template Func0<true>;
}

template <class C, class R>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)() const) {
    return &SqMember<C, R>::template Func0C<true>;
}

template <class C, class R>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)()) {
    return &SqMember<C, R&>::template Func0<true>;
}
template <class C, class R>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)() const) {
    return &SqMember<C, R&>::template Func0C<true>;
}

// Arg Count 1
template <class C, class R, class A1>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1)) {
    return &SqMember<C, R>::template Func1<A1, true>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1) const) {
    return &SqMember<C, R>::template Func1C<A1, true>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1)) {
    return &SqMember<C, R&>::template Func1<A1, true>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1) const) {
    return &SqMember<C, R&>::template Func1C<A1, true>;
}

// Arg Count 2
template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2)) {
    return &SqMember<C, R>::template Func2<A1, A2, true>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2) const) {
    return &SqMember<C, R>::template Func2C<A1, A2, true>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2)) {
    return &SqMember<C, R&>::template Func2<A1, A2, true>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2) const) {
    return &SqMember<C, R&>::template Func2C<A1, A2, true>;
}

// Arg Count 3
template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3)) {
    return &SqMember<C, R>::template Func3<A1, A2, A3, true>;
}

template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3) const) {
    return &SqMember<C, R>::template Func3C<A1, A2, A3, true>;
}
template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3)) {
    return &SqMember<C, R&>::template Func3<A1, A2, A3, true>;
}

template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3) const) {
    return &SqMember<C, R&>::template Func3C<A1, A2, A3, true>;
}

// Arg Count 4
template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4)) {
    return &SqMember<C, R>::template Func4<A1, A2, A3, A4, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4) const) {
    return &SqMember<C, R>::template Func4C<A1, A2, A3, A4, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4)) {
    return &SqMember<C, R&>::template Func4<A1, A2, A3, A4, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4) const) {
    return &SqMember<C, R&>::template Func4C<A1, A2, A3, A4, true>;
}

// Arg Count 5
template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqMember<C, R>::template Func5<A1, A2, A3, A4, A5, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4, A5) const) {
    return &SqMember<C, R>::template Func5C<A1, A2, A3, A4, A5, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5)) {
    return &SqMember<C, R&>::template Func5<A1, A2, A3, A4, A5, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5) const) {
    return &SqMember<C, R&>::template Func5C<A1, A2, A3, A4, A5, true>;
}

// Arg Count 6
template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqMember<C, R>::template Func6<A1, A2, A3, A4, A5, A6, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberOverloadedFunc(R (C::* /*method*/)(A1, A2, A3, A4, A5, A6) const) {
    return &SqMember<C, R>::template Func6C<A1, A2, A3, A4, A5, A6, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5, A6)) {
    return &SqMember<C, R&>::template Func6<A1, A2, A3, A4, A5, A6, true>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberOverloadedFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5, A6) const) {
    return &SqMember<C, R&>::template Func6C<A1, A2, A3, A4, A5, A6, true>;
}

//
// Overload handler resolver
//

template<class R>
inline SQFUNCTION SqOverloadFunc(R (* /*method*/))
{
    return &SqOverload<R>::Func;
}

template<class C, class R>
inline SQFUNCTION SqOverloadFunc(R (C::* /*method*/))
{
    return &SqOverload<R>::Func;
}

template<class C, class R, class... Args>
inline SQFUNCTION SqOverloadFunc(R (C::* /*method*/)(Args...) const)
{
    return &SqOverload<R>::Func;
}

//
// Query argument count
//

template<class R, class... Args>
inline int SqGetArgCount(R (* /*method*/)(Args...))
{
	return sizeof...(Args);
}

template<class C, class R, class... Args>
inline int SqGetArgCount(R (C::* /*method*/)(Args...))
{
	return sizeof...(Args);
}

template<class C, class R, class... Args>
inline int SqGetArgCount(R (C::* /*method*/)(Args...) const)
{
	return sizeof...(Args);
}

}
