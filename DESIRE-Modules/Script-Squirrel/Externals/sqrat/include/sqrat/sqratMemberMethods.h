#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratMemberMethods: Member Methods
//

//
// Copyright (c) 2009 Brandon Jones
// Copyright 2011 Li-Cheng (Andy) Tai
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

namespace Sqrat {

//
// Squirrel Global Functions
//

template <class C, class R>
class SqMember
{
public:

    // Arg Count 0
	template <bool overloaded /*= false*/>
	static SQInteger Func0(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif

		typedef R(C::*M)();
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		R ret = (ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <bool overloaded /*= false*/>
	static SQInteger Func0C(HSQUIRRELVM vm)
	{
#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)() const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		R ret = (ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 1
	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 2
	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 3
	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 4
	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 5
	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4, A5);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4, A5) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	// Arg Count 6
	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4, A5, A6);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R(C::*M)(A1, A2, A3, A4, A5, A6) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVar(vm, ret);
		return 1;
	}
};

//
// reference return specialization
//

template <class C, class R>
class SqMember<C, R&>
{
public:
	// Arg Count 0
	template <bool overloaded /*= false*/>
	static SQInteger Func0(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)();
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		R& ret = (ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <bool overloaded /*= false*/>
	static SQInteger Func0C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)() const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		R& ret = (ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 1
	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 2
	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 3
	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 4
	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 5
	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4, A5);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4, A5) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	// Arg Count 6
	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4, A5, A6);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef R& (C::*M)(A1, A2, A3, A4, A5, A6) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		R& ret = (ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		PushVarR(vm, ret);
		return 1;
	}
};

//
// void return specialization
//

template <class C>
class SqMember<C, void>
{
public:
	// Arg Count 0
	template <bool overloaded /*= false*/>
	static SQInteger Func0(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)();
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		(ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <bool overloaded /*= false*/>
	static SQInteger Func0C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 2)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)() const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		(ptr->*method)();
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 1
	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, bool overloaded /*= false*/>
	static SQInteger Func1C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 3)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 2
	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, class A2, bool overloaded /*= false*/>
	static SQInteger Func2C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 4)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 3
	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, class A2, class A3, bool overloaded /*= false*/>
	static SQInteger Func3C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 5)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 4
	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, class A2, class A3, class A4, bool overloaded /*= false*/>
	static SQInteger Func4C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 6)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 5
	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4, A5);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, class A2, class A3, class A4, class A5, bool overloaded /*= false*/>
	static SQInteger Func5C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 7)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4, A5) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	// Arg Count 6
	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4, A5, A6);
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, bool overloaded /*= false*/>
	static SQInteger Func6C(HSQUIRRELVM vm)
	{

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(!SQRAT_CONST_CONDITION(overloaded) && sq_gettop(vm) != 8)
		{
			return sq_throwerror(vm, _SC("wrong number of parameters"));
		}
#endif
		typedef void (C::*M)(A1, A2, A3, A4, A5, A6) const;
		M* methodPtr;
		sq_getuserdata(vm, -1, (SQUserPointer*)&methodPtr, NULL);
		M method = *methodPtr;

		C* ptr;
		ptr = Var<C*>(vm, 1).value;
		if(Error::Occurred(vm))
		{
			Error::Clear(vm); // clear the previous error
			assert(0); // may fail because C is not a type bound in the VM
			return sq_throwerror(vm, Error::Message(vm));
		}

		Var<A1> a1(vm, 2);
		Var<A2> a2(vm, 3);
		Var<A3> a3(vm, 4);
		Var<A4> a4(vm, 5);
		Var<A5> a5(vm, 6);
		Var<A6> a6(vm, 7);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		(ptr->*method)(
			a1.value,
			a2.value,
			a3.value,
			a4.value,
			a5.value,
			a6.value
			);
		if(Error::Occurred(vm))
		{
			return sq_throwerror(vm, Error::Message(vm));
		}
		return 0;
	}
};

//
// Member Function Resolvers
//

// Arg Count 0
template <class C, class R>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)())
{
	return &SqMember<C, R>::template Func0<false>;
}

template <class C, class R>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)() const)
{
	return &SqMember<C, R>::template Func0C<false>;
}

template <class C, class R>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)())
{
	return &SqMember<C, R&>::template Func0<false>;
}

template <class C, class R>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)() const)
{
	return &SqMember<C, R&>::template Func0C<false>;
}

// Arg Count 1
template <class C, class R, class A1>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1))
{
	return &SqMember<C, R>::template Func1<A1, false>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1) const)
{
	return &SqMember<C, R>::template Func1C<A1, false>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1))
{
	return &SqMember<C, R&>::template Func1<A1, false>;
}

template <class C, class R, class A1>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1) const)
{
	return &SqMember<C, R&>::template Func1C<A1, false>;
}

// Arg Count 2
template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2))
{
	return &SqMember<C, R>::template Func2<A1, A2, false>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2) const)
{
	return &SqMember<C, R>::template Func2C<A1, A2, false>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2))
{
	return &SqMember<C, R&>::template Func2<A1, A2, false>;
}

template <class C, class R, class A1, class A2>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2) const)
{
	return &SqMember<C, R&>::template Func2C<A1, A2, false>;
}

// Arg Count 3
template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3))
{
	return &SqMember<C, R>::template Func3<A1, A2, A3, false>;
}

template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3) const)
{
	return &SqMember<C, R>::template Func3C<A1, A2, A3, false>;
}

template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3))
{
	return &SqMember<C, R&>::template Func3<A1, A2, A3, false>;
}

template <class C, class R, class A1, class A2, class A3>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3) const)
{
	return &SqMember<C, R&>::template Func3C<A1, A2, A3, false>;
}

// Arg Count 4
template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4))
{
	return &SqMember<C, R>::template Func4<A1, A2, A3, A4, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4) const)
{
	return &SqMember<C, R>::template Func4C<A1, A2, A3, A4, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3, A4))
{
	return &SqMember<C, R&>::template Func4<A1, A2, A3, A4, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3, A4) const)
{
	return &SqMember<C, R&>::template Func4C<A1, A2, A3, A4, false>;
}

// Arg Count 5
template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4, A5))
{
	return &SqMember<C, R>::template Func5<A1, A2, A3, A4, A5, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4, A5) const)
{
	return &SqMember<C, R>::template Func5C<A1, A2, A3, A4, A5, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5))
{
	return &SqMember<C, R&>::template Func5<A1, A2, A3, A4, A5, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5>
inline SQFUNCTION SqMemberFunc(R&(C::* /*method*/)(A1, A2, A3, A4, A5) const)
{
	return &SqMember<C, R&>::template Func5C<A1, A2, A3, A4, A5, false>;
}

// Arg Count 6
template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4, A5, A6))
{
	return &SqMember<C, R>::template Func6<A1, A2, A3, A4, A5, A6, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberFunc(R(C::* /*method*/)(A1, A2, A3, A4, A5, A6) const)
{
	return &SqMember<C, R>::template Func6C<A1, A2, A3, A4, A5, A6, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5, A6))
{
	return &SqMember<C, R&>::template Func6<A1, A2, A3, A4, A5, A6, false>;
}

template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
inline SQFUNCTION SqMemberFunc(R& (C::* /*method*/)(A1, A2, A3, A4, A5, A6) const)
{
	return &SqMember<C, R&>::template Func6C<A1, A2, A3, A4, A5, A6, false>;
}

//
// Variable Get
//

template <class C, class V>
inline SQInteger sqDefaultGet(HSQUIRRELVM vm)
{
	C* ptr;
	ptr = Var<C*>(vm, 1).value;
	if(Error::Occurred(vm))
	{
		Error::Clear(vm); // clear the previous error
		return sq_throwerror(vm, Error::Message(vm));
	}

	typedef V C::*M;
	M* memberPtr = nullptr;
	sq_getuserdata(vm, -1, (SQUserPointer*)&memberPtr, nullptr); // Get Member...
	M member = *memberPtr;

	PushVarR(vm, ptr->*member);

	return 1;
}

template <class C, class V>
inline SQInteger sqStaticGet(HSQUIRRELVM vm)
{
	typedef V *M;
	M* memberPtr = nullptr;
	sq_getuserdata(vm, -1, (SQUserPointer*)&memberPtr, nullptr); // Get Member...
	M member = *memberPtr;

	PushVarR(vm, *member);

	return 1;
}

inline SQInteger sqVarGet(HSQUIRRELVM vm)
{
	// Find the get method in the get table
	sq_push(vm, 2);
#if !defined (SCRAT_NO_ERROR_CHECKING)
	if(SQ_FAILED(sq_rawget(vm, -2)))
	{
		sq_pushnull(vm);
		return sq_throwobject(vm);
	}
#else
	sq_rawget(vm, -2);
#endif

	// push 'this'
	sq_push(vm, 1);

	// Call the getter
#if !defined (SCRAT_NO_ERROR_CHECKING)
	SQRESULT result = sq_call(vm, 1, true, true);
	if(SQ_FAILED(result))
	{
		return sq_throwerror(vm, LastErrorString(vm).Str());
	}
#else
	sq_call(vm, 1, true, true);
#endif

	return 1;
}

//
// Variable Set
//

template <class C, class V>
inline SQInteger sqDefaultSet(HSQUIRRELVM vm)
{
	C* ptr;
	ptr = Var<C*>(vm, 1).value;
	if(Error::Occurred(vm))
	{
		Error::Clear(vm); // clear the previous error
		return sq_throwerror(vm, Error::Message(vm));
	}

	typedef V C::*M;
	M* memberPtr = NULL;
	sq_getuserdata(vm, -1, (SQUserPointer*)&memberPtr, NULL); // Get Member...
	M member = *memberPtr;

	if(std::is_pointer<V>::value)
	{
		ptr->*member = Var<V>(vm, 2).value;
	}
	else if(std::is_reference<V>::value)
	{
		ptr->*member = Var<V>(vm, 2).value;
	}
	else
	{
		ptr->*member = Var<const V&>(vm, 2).value;
	}
	if(Error::Occurred(vm))
	{
		return sq_throwerror(vm, Error::Message(vm));
	}

	return 0;
}

template <class C, class V>
inline SQInteger sqStaticSet(HSQUIRRELVM vm)
{
	typedef V *M;
	M* memberPtr = NULL;
	sq_getuserdata(vm, -1, (SQUserPointer*)&memberPtr, NULL); // Get Member...
	M member = *memberPtr;

	if(std::is_pointer<V>::value || std::is_reference<V>::value)
	{
		*member = Var<V>(vm, 2).value;
	}
	else
	{
		*member = Var<const V&>(vm, 2).value;
	}
	if(Error::Occurred(vm))
	{
		return sq_throwerror(vm, Error::Message(vm));
	}

	return 0;
}

inline SQInteger sqVarSet(HSQUIRRELVM vm)
{
	// Find the set method in the set table
	sq_push(vm, 2);
#if !defined (SCRAT_NO_ERROR_CHECKING)
	if(SQ_FAILED(sq_rawget(vm, -2)))
	{
		sq_pushnull(vm);
		return sq_throwobject(vm);
	}
#else
	sq_rawget(vm, -2);
#endif

	// push 'this'
	sq_push(vm, 1);
	sq_push(vm, 3);

	// Call the setter
#if !defined (SCRAT_NO_ERROR_CHECKING)
	SQRESULT result = sq_call(vm, 2, false, true);
	if(SQ_FAILED(result))
	{
		return sq_throwerror(vm, LastErrorString(vm).Str());
	}
#else
	sq_call(vm, 2, false, true);
#endif

	return 0;
}

}
