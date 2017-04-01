#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratTable: Table Binding
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
#include "sqratObject.h"
#include "sqratGlobalMethods.h"

namespace Sqrat {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The base class for Table that implements almost all of its functionality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TableBase : public Object
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Default constructor (null)
	///
	/// \param v VM that the table will exist in
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TableBase(HSQUIRRELVM v)
		: Object(v, true)
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Construct the TableBase from an Object that already exists
	///
	/// \param obj An Object that should already represent a Squirrel table
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TableBase(const Object& obj)
		: Object(obj)
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Construct the TableBase from a HSQOBJECT and HSQUIRRELVM that already exist
	///
	/// \param o Squirrel object that should already represent a Squirrel table
	/// \param v Squirrel VM that contains the Squirrel object given
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TableBase(HSQOBJECT o, HSQUIRRELVM v)
		: Object(o, v)
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Binds a Table or Class to the Table (can be used to facilitate namespaces)
	///
	/// \param name The key in the table being assigned a Table or Class
	/// \param obj  Table or Class that is being placed in the table
	///
	/// \remarks
	/// Bind cannot be called "inline" like other functions because it introduces order-of-initialization bugs.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Bind(const SQChar *name, Object& object)
	{
		sq_pushobject(vm, obj);
		sq_pushstring(vm, name, -1);
		sq_pushobject(vm, object.GetObject());
		sq_newslot(vm, -3, false);
		sq_pop(vm, 1); // pop table
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Binds a raw Squirrel closure to the Table
	///
	/// \param name The key in the table being assigned a function
	/// \param func Squirrel function that is being placed in the Table
	///
	/// \return The Table itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TableBase& SquirrelFunc(const SQChar *name, SQFUNCTION func)
	{
		sq_pushobject(vm, obj);
		sq_pushstring(vm, name, -1);
		sq_newclosure(vm, func, 0);
		sq_newslot(vm, -3, false);
		sq_pop(vm, 1); // pop table
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Sets a key in the Table to a specific value
	///
	/// \param name The key in the table being assigned a value
	/// \param val  Value that is being placed in the Table
	///
	/// \tparam V Type of value (usually doesnt need to be defined explicitly)
	///
	/// \return The Table itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class V>
	TableBase& SetValue(const SQChar *name, const V& val)
	{
		BindValue<V>(name, val, false);
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Sets a key in the Table to a specific instance (like a reference)
	///
	/// \param name The key in the table being assigned a value
	/// \param val  Pointer to the instance that is being placed in the Table
	///
	/// \tparam V Type of instance (usually doesnt need to be defined explicitly)
	///
	/// \return The Table itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class V>
	TableBase& SetInstance(const SQChar *name, V *val)
	{
		BindInstance<V>(name, val, false);
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Sets a key in the Table to a specific function
	///
	/// \param name   The key in the table being assigned a value
	/// \param method Function that is being placed in the Table
	///
	/// \tparam F Type of function (only define this if you need to choose a certain template specialization or overload)
	///
	/// \return The Table itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class F>
	TableBase& Func(const SQChar *name, F method)
	{
		BindFunc(name, &method, sizeof(method), SqGlobalFunc(method));
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Sets a key in the Table to a specific function and allows the key to be overloaded with functions of a different amount of arguments
	///
	/// \param name   The key in the table being assigned a value
	/// \param method Function that is being placed in the Table
	///
	/// \tparam F Type of function (only define this if you need to choose a certain template specialization or overload)
	///
	/// \return The Table itself so the call can be chained
	///
	/// \remarks
	/// Overloading in Sqrat does not work for functions with the same amount of arguments (just like in Squirrel).
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class F>
	TableBase& Overload(const SQChar *name, F method)
	{
		BindOverload(name, &method, sizeof(method), SqGlobalOverloadedFunc(method), SqOverloadFunc(method), SqGetArgCount(method));
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the given key exists in the table
	///
	/// \param name Key to check
	///
	/// \return True on success, otherwise false
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool HasKey(const SQChar *name)
	{
		sq_pushobject(vm, obj);
		sq_pushstring(vm, name, -1);
		if(SQ_FAILED(sq_get(vm, -2)))
		{
			sq_pop(vm, 1);
			return false;
		}
		sq_pop(vm, 2);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the value at a given key
	///
	/// \param name Key of the element
	///
	/// \tparam T Type of value (fails if value is not of this type)
	///
	/// \return std::shared_ptr containing the value (or null if failed)
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	std::shared_ptr<T> GetValue(const SQChar* name)
	{
		sq_pushobject(vm, obj);
		sq_pushstring(vm, name, -1);
#if !defined (SCRAT_NO_ERROR_CHECKING)
		if(SQ_FAILED(sq_get(vm, -2)))
		{
			sq_pop(vm, 1);
			Error::Throw(vm, _SC("illegal index"));
			return std::shared_ptr<T>();
		}
#else
		sq_get(vm, -2);
#endif
		Var<std::shared_ptr<T> > entry(vm, -1);
		if(Error::Occurred(vm))
		{
			sq_pop(vm, 2);
			return std::shared_ptr<T>();
		}
		sq_pop(vm, 2);
		return entry.value;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Represents a table in Squirrel
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Table : public TableBase
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Constructs a Table
	///
	/// \param v VM to create the Table in
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Table(HSQUIRRELVM v)
		: TableBase(v)
	{
		sq_newtable(vm);
		sq_getstackobj(vm, -1, &obj);
		sq_addref(vm, &obj);
		sq_pop(vm, 1);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Construct the Table from an Object that already exists
	///
	/// \param obj An Object that should already represent a Squirrel table
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Table(const Object& obj)
		: TableBase(obj)
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Construct the Table from a HSQOBJECT and HSQUIRRELVM that already exist
	///
	/// \param o Squirrel object that should already represent a Squirrel table
	/// \param v Squirrel VM that contains the Squirrel object given
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Table(HSQOBJECT o, HSQUIRRELVM v)
		: TableBase(o, v)
	{

	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Table that is a reference to the Squirrel root table for a given VM
/// The Squirrel root table is usually where all globals are stored by the Squirrel language.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class RootTable : public TableBase
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Constructs a RootTable object to represent the given VM's root table
	///
	/// \param v VM to get the RootTable for
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RootTable(HSQUIRRELVM v)
		: TableBase(v)
	{
		sq_pushroottable(vm);
		sq_getstackobj(vm, -1, &obj);
		sq_addref(vm, &obj);
		sq_pop(v, 1); // pop root table
	}
};

}
