#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratObject: Referenced Squirrel Object Wrapper
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

#include "SQUIRREL3/include/squirrel.h"
#include "sqratAllocator.h"
#include "sqratTypes.h"
#include "sqratOverloadMethods.h"

namespace Sqrat {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The base class for classes that represent Squirrel objects
///
/// \remarks
/// All Object and derived classes MUST be destroyed before calling sq_close or your application will crash when exiting.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Object
{
protected:
	HSQUIRRELVM vm;
	HSQOBJECT obj;
	bool release;

	Object(HSQUIRRELVM v, bool releaseOnDestroy = true)
		: vm(v)
		, release(releaseOnDestroy)
	{
		sq_resetobject(&obj);
	}

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Default constructor (null)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object()
		: vm(0)
		, release(true)
	{
		sq_resetobject(&obj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor
	///
	/// \param so Object to copy
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object(const Object& so)
		: vm(so.vm)
		, obj(so.obj)
		, release(so.release)
	{
		sq_addref(vm, &obj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Constructs an Object from a Squirrel object
	///
	/// \param o Squirrel object
	/// \param v VM that the object will exist in
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object(HSQOBJECT o, HSQUIRRELVM v)
		: vm(v)
		, obj(o)
		, release(true)
	{
		sq_addref(vm, &obj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Constructs an Object from a C++ instance
	///
	/// \param instance Pointer to a C++ class instance that has been bound already
	/// \param v        VM that the object will exist in
	///
	/// \tparam T Type of instance
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	Object(T *instance, HSQUIRRELVM v)
		: vm(v)
		, release(true)
	{
		ClassType<T>::PushInstance(vm, instance);
		sq_getstackobj(vm, -1, &obj);
		sq_addref(vm, &obj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Destructor
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual ~Object()
	{
		if(release)
		{
			Release();
			release = false;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Assignment operator
	///
	/// \param so Object to copy
	///
	/// \return The Object itself
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object& operator=(const Object& so)
	{
		if(release)
		{
			Release();
		}
		vm = so.vm;
		obj = so.obj;
		release = so.release;
		sq_addref(vm, &GetObject());
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Gets the Squirrel VM for this Object (copy)
	///
	/// \return Squirrel VM associated with the Object
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	HSQUIRRELVM GetVM() const
	{
		return vm;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Gets the type of the Object as defined by the Squirrel API
	///
	/// \return SQObjectType for the Object
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SQObjectType GetType() const
	{
		return GetObject()._type;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Checks whether the Object is null
	///
	/// \return True if the Object currently has a null value, otherwise false
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool IsNull() const
	{
		return sq_isnull(GetObject());
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Gets the Squirrel object for this Object (copy)
	///
	/// \return Squirrel object
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual HSQOBJECT GetObject() const
	{
		return obj;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Gets the Squirrel object for this Object (reference)
	///
	/// \return Squirrel object
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual HSQOBJECT& GetObject()
	{
		return obj;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Sets the Object to null (removing its references to underlying Squirrel objects)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Release()
	{
		sq_release(vm, &obj);
		sq_resetobject(&obj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value of a slot from the object
	///
	/// \param slot Name of the slot
	///
	/// \return An Object representing the value of the slot (can be a null object if nothing was found)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object GetSlot(const SQChar* slot) const
	{
		HSQOBJECT slotObj;
		sq_pushobject(vm, GetObject());
		sq_pushstring(vm, slot, -1);

#if !defined(SCRAT_NO_ERROR_CHECKING)
		if(SQ_FAILED(sq_get(vm, -2)))
		{
			sq_pop(vm, 1);
			return Object(vm); // Return a NULL object
		}
		else
		{
			sq_getstackobj(vm, -1, &slotObj);
			Object ret(slotObj, vm); // must addref before the pop!
			sq_pop(vm, 2);
			return ret;
	}
#else
		sq_get(vm, -2);
		sq_getstackobj(vm, -1, &slotObj);
		Object ret(slotObj, vm); // must addref before the pop!
		sq_pop(vm, 2);
		return ret;
#endif
}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value of an index from the object
	///
	/// \param index Index of the slot
	///
	/// \return An Object representing the value of the slot (can be a null object if nothing was found)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Object GetSlot(SQInteger index) const
	{
		HSQOBJECT slotObj;
		sq_pushobject(vm, GetObject());
		sq_pushinteger(vm, index);

#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(SQ_FAILED(sq_get(vm, -2)))
		{
			sq_pop(vm, 1);
			return Object(vm); // Return a NULL object
		}
		else
		{
			sq_getstackobj(vm, -1, &slotObj);
			Object ret(slotObj, vm); // must addref before the pop!
			sq_pop(vm, 2);
			return ret;
		}
#else
		sq_get(vm, -2);
		sq_getstackobj(vm, -1, &slotObj);
		Object ret(slotObj, vm); // must addref before the pop!
		sq_pop(vm, 2);
		return ret;
#endif
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Casts the object to a certain C++ type
	///
	/// \tparam T Type to cast to
	///
	/// \return A copy of the value of the Object with the given type
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	T Cast() const
	{
		sq_pushobject(vm, GetObject());
		T ret = Var<T>(vm, -1).value;
		sq_pop(vm, 1);
		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the size of the Object
	///
	/// \return Size of Object
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SQInteger GetSize() const
	{
		sq_pushobject(vm, GetObject());
		SQInteger ret = sq_getsize(vm, -1);
		sq_pop(vm, 1);
		return ret;
	}

protected:
	// Bind a function and it's associated Squirrel closure to the object
	inline void BindFunc(const SQChar* name, void* method, size_t methodSize, SQFUNCTION func, bool staticVar = false)
	{
		sq_pushobject(vm, GetObject());
		sq_pushstring(vm, name, -1);

		SQUserPointer methodPtr = sq_newuserdata(vm, static_cast<SQUnsignedInteger>(methodSize));
		memcpy(methodPtr, method, methodSize);

		sq_newclosure(vm, func, 1);
		sq_newslot(vm, -3, (SQBool)staticVar);
		sq_pop(vm, 1); // pop table
	}

	// Bind a function and it's associated Squirrel closure to the object
	inline void BindOverload(const SQChar* name, void* method, size_t methodSize, SQFUNCTION func, SQFUNCTION overload, int argCount, bool staticVar = false)
	{
		String overloadName = SqOverloadName::Get(name, argCount);

		sq_pushobject(vm, GetObject());

		// Bind overload handler
		sq_pushstring(vm, name, -1);
		sq_pushstring(vm, name, -1); // function name is passed as a free variable
		sq_newclosure(vm, overload, 1);
		sq_newslot(vm, -3, (SQBool)staticVar);

		// Bind overloaded function
		sq_pushstring(vm, overloadName.c_str(), -1);
		SQUserPointer methodPtr = sq_newuserdata(vm, static_cast<SQUnsignedInteger>(methodSize));
		memcpy(methodPtr, method, methodSize);
		sq_newclosure(vm, func, 1);
		sq_newslot(vm, -3, (SQBool)staticVar);

		sq_pop(vm, 1); // pop table
	}

	// Set the value of a variable on the object. Changes to values set this way are not reciprocated
	template<class V>
	inline void BindValue(const SQChar* name, const V& val, bool staticVar = false)
	{
		sq_pushobject(vm, GetObject());
		sq_pushstring(vm, name, -1);
		PushVar(vm, val);
		sq_newslot(vm, -3, (SQBool)staticVar);
		sq_pop(vm, 1); // pop table
	}

	// Set the value of an instance on the object. Changes to values set this way are reciprocated back to the source instance
	template<class V>
	inline void BindInstance(const SQChar* name, V* val, bool staticVar = false)
	{
		sq_pushobject(vm, GetObject());
		sq_pushstring(vm, name, -1);
		PushVar(vm, val);
		sq_newslot(vm, -3, (SQBool)staticVar);
		sq_pop(vm, 1); // pop table
	}
};

template<>
inline void Object::BindValue<int>(const SQChar* name, const int & val, bool staticVar)
{
	sq_pushobject(vm, GetObject());
	sq_pushstring(vm, name, -1);
	PushVar<int>(vm, val);
	sq_newslot(vm, -3, (SQBool)staticVar);
	sq_pop(vm, 1); // pop table
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push Object instances to and from the stack as references (Object is always a reference)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<Object>
{
	Object value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as an Object
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		HSQOBJECT sqValue;
		sq_getstackobj(vm, idx, &sqValue);
		value = Object(sqValue, vm);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put an Object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const Object& value)
	{
		sq_pushobject(vm, value.GetObject());
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push Object instances to and from the stack as references (Object is always a reference)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<Object&> : Var<Object> {Var(HSQUIRRELVM vm, SQInteger idx) : Var<Object>(vm, idx) {}};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push Object instances to and from the stack as references (Object is always a reference)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<const Object&> : Var<Object> {Var(HSQUIRRELVM vm, SQInteger idx) : Var<Object>(vm, idx) {}};

}
