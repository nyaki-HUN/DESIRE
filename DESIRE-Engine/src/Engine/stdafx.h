#pragma once

#include "Engine/Core/platform.h"
#include "Engine/Core/assert.h"
#include "Engine/Core/Log.h"

// --------------------------------------------------------------------------------------------------------------------

// STD library
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <cfloat>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <functional>
#include <utility>
#include <memory>

// Containers
#include <array>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <unordered_map>

// Thread support
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>

// SIMD intrinsics
#if defined(DESIRE_USE_SSE)
	// Includes used in vectormath_SSE.h
	#if defined(__SSE4_1__)
		#include <smmintrin.h>
	#else
		#include <pmmintrin.h>
	#endif
#endif

// --------------------------------------------------------------------------------------------------------------------

#ifdef DESIRE_PLATFORM_WINDOWS
// Enable some warnings which are turned off by default
#pragma warning(default: 4062)	// enumerator 'identifier' in a switch of enum 'enumeration' is not handled
#pragma warning(default: 4191)	// 'operator/operation': unsafe conversion from 'type of expression' to 'type required'
#pragma warning(default: 4242)	// 'identifier' : conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4254)	// 'operator' : conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4255)	// 'function' : no function prototype given : converting '()' to '(void)'
#pragma warning(default: 4263)	// 'function' : member function does not override any base class virtual member function
#pragma warning(default: 4264)	// 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
#pragma warning(default: 4265)	// 'class': class has virtual functions, but destructor is not virtual
#pragma warning(default: 4266)	// 'function' : no override available for virtual member function from base 'type'; function is hidden
#pragma warning(default: 4287)	// 'operator': unsigned / negative constant mismatch
#pragma warning(default: 4289)	// nonstandard extension used : 'var' : loop control variable declared in the for - loop is used outside the for - loop scope
#pragma warning(default: 4296)	// 'operator' : expression is always false
#pragma warning(default: 4302)	// 'conversion' : truncation from 'type1' to 'type2'
#pragma warning(default: 4311)	// 'variable' : pointer truncation from 'type' to 'type'
#pragma warning(default: 4312)	// 'operation' : conversion from 'type1' to 'type2' of greater size
#pragma warning(default: 4339)	// 'type' : use of undefined type detected in CLR meta - data - use of this type may lead to a runtime exception
#pragma warning(default: 4342)	// behavior change : 'function' called, but a member operator was called in previous versions
#pragma warning(default: 4355)	// 'this' : used in base member initializer list
#pragma warning(default: 4370)	// layout of class has changed from a previous version of the compiler due to better packing
#pragma warning(default: 4371)	// layout of class may have changed from a previous version of the compiler due to better packing of member 'member'
#pragma warning(default: 4388)	// signed / unsigned mismatch
#pragma warning(default: 4412)	// 'function' : function signature contains type 'type'; C++ objects are unsafe to pass between pure code and mixed or native
#pragma warning(default: 4431)	// missing type specifier - int assumed.Note: C no longer supports default - int
#pragma warning(default: 4435)	// 'class1' : Object layout under / vd2 will change due to virtual base 'class2'
#pragma warning(default: 4437)	// dynamic_cast from virtual base 'class1' to 'class2' could fail in some contexts
#pragma warning(default: 4444)	// top level '__unaligned' is not implemented in this context
#pragma warning(default: 4471)	// a forward declaration of an unscoped enumeration must have an underlying type(int assumed)
#pragma warning(default: 4472)	// 'identifier' is a native enum : add an access specifier(private / public) to declare a managed enum
#pragma warning(default: 4536)	// 'type name' : type - name exceeds meta - data limit of 'limit' characters
#pragma warning(default: 4545)	// expression before comma evaluates to a function which is missing an argument list
#pragma warning(default: 4546)	// function call before comma missing argument list
#pragma warning(default: 4547)	// 'operator' : operator before comma has no effect; expected operator with side - effect
#pragma warning(default: 4548)	// expression before comma has no effect; expected expression with side-effect
#pragma warning(default: 4549)	// 'operator': operator before comma has no effect; did you intend 'operator' ?
#pragma warning(default: 4555)	// expression has no effect; expected expression with side - effect
#pragma warning(default: 4574)	// 'identifier' is defined to be '0': did you mean to use '#if identifier' ?
#pragma warning(default: 4608)	// 'symbol1' has already been initialized by another union member in the initializer list, 'symbol2'
#pragma warning(default: 4619)	// #pragma warning : there is no warning number 'number'
#pragma warning(default: 4623)	// 'derived class' : default constructor could not be generated because a base class default constructor is inaccessible
#pragma warning(default: 4628)	// digraphs not supported with - Ze.Character sequence 'digraph' not interpreted as alternate token for 'char'
#pragma warning(default: 4682)	// 'symbol' : no directional parameter attribute specified, defaulting to[in]
#pragma warning(default: 4686)	// 'user-defined type' : possible change in behavior, change in UDT return calling convention
#pragma warning(default: 4692)	// 'function' : signature of non - private member contains assembly private native type 'native_type'
#pragma warning(default: 4738)	// storing 32 - bit float result in memory, possible loss of performance
#pragma warning(default: 4767)	// section name 'symbol' is longer than 8 characters and will be truncated by the linker
#pragma warning(default: 4786)	// 'symbol' : object name was truncated to 'number' characters in the debug information
#pragma warning(default: 4826)	// conversion from 'type1' to 'type2' is sign - extended.This may cause unexpected runtime behavior
#pragma warning(default: 4837)	// trigraph detected : '??%c' replaced by '%c'
#pragma warning(default: 4905)	// wide string literal cast to 'LPSTR'
#pragma warning(default: 4906)	// string literal cast to 'LPWSTR'
#pragma warning(default: 4917)	// 'declarator' : a GUID can only be associated with a class, interface, or namespace
#pragma warning(default: 4928)	// illegal copy - initialization; more than one user - defined conversion has been implicitly applied
#pragma warning(default: 4931)	// we are assuming the type library was built for number - bit pointers
#pragma warning(default: 4946)	// reinterpret_cast used between related classes : 'class1' and 'class2'
#pragma warning(default: 4962)	// 'function' : profile - guided optimizations disabled because optimizations caused profile data to become inconsistent
#pragma warning(default: 4986)	// 'symbol' : exception specification does not match previous declaration
#pragma warning(default: 4988)	// 'symbol' : variable declared outside class / function scope
#endif
