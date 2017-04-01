#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratConst: Constant and Enumeration Binding
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

namespace Sqrat {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Facilitates exposing a C++ constant to Squirrel
///
/// \remarks
/// The ConstTable class only facilitates binding C++ constants that are integers,
/// floats, and strings because the Squirrel constant table can only contain these types of
/// values. Other types of constants can be bound using Class::SetStaticValue instead.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ConstTable : public Object
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Constructs a ConstTable object to represent the given VM's const table
	///
	/// \param v VM to get the ConstTable for
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ConstTable(HSQUIRRELVM v)
		: Object(v, false)
	{
		sq_pushconsttable(vm);
		sq_getstackobj(vm, -1, &obj);
		sq_pop(v, 1); // No addref needed, since the consttable is always around
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Binds a constant value
	///
	/// \param name Name of the value as it will appear in Squirrel
	/// \param val  Value to bind
	///
	/// \return The ConstTable itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ConstTable& Const(const SQChar* name, const int val)
	{
		BindValue<int>(name, val, false);
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Binds a constant value
	///
	/// \param name Name of the value as it will appear in Squirrel
	/// \param val  Value to bind
	///
	/// \return The ConstTable itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ConstTable& Const(const SQChar* name, const float val)
	{
		BindValue<float>(name, val, false);
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Binds a constant value
	///
	/// \param name Name of the value as it will appear in Squirrel
	/// \param val  Value to bind
	///
	/// \return The ConstTable itself so the call can be chained
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ConstTable& Const(const SQChar* name, const SQChar* val)
	{
		BindValue<const SQChar*>(name, val, false);
		return *this;
	}
};

}
