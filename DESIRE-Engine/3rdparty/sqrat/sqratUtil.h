#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratUtil: Squirrel Utilities
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

#include <cassert>
#include <unordered_map>
#include "Script-Squirrel/include/squirrel.h"
#include "Core/String.h"

namespace Sqrat
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Define an inline function to avoid MSVC's "conditional expression is constant" warning
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
	template <typename T>
	inline T _c_def(T value) { return value; }
	#define SQRAT_CONST_CONDITION(value) _c_def(value)
#else
	#define SQRAT_CONST_CONDITION(value) value
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The class that must be used to deal with errors that Sqrat has
///
/// \remarks
/// When documentation in Sqrat says, "This function MUST have its error handled if it occurred," that
/// means that after the function has been run, you must call Error::Occurred to see if the function
/// ran successfully. If the function did not run successfully, then you MUST either call Error::Clear
/// or Error::Message to clear the error buffer so new ones may occur and Sqrat does not get confused.
///
/// \remarks
/// Any error thrown inside of a bound C++ function will be thrown in the given Squirrel VM and
/// automatically handled.
///
/// \remarks
/// If compiling with SCRAT_NO_ERROR_CHECKING defined, Sqrat will run significantly faster,
/// but it will no longer check for errors and a Squirrel script may crash the C++ application if errors occur in it.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Error
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Clears the error associated with a given VM
	///
	/// \param vm Target VM
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void Clear(HSQUIRRELVM vm)
	{
#if !defined(SCRAT_NO_ERROR_CHECKING)
		sq_pushregistrytable(vm);
		sq_pushstring(vm, "__error", -1);
		sq_rawdeleteslot(vm, -2, false);
		sq_pop(vm, 1);
#endif
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Clears the error associated with a given VM and returns the associated error message
	///
	/// \param vm Target VM
	///
	/// \return String containing a nice error message
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static const char* Message(HSQUIRRELVM vm)
	{
#if !defined(SCRAT_NO_ERROR_CHECKING)
		sq_pushregistrytable(vm);
		sq_pushstring(vm, "__error", -1);
		if(SQ_SUCCEEDED(sq_rawget(vm, -2)))
		{
			static String errorMessage;

			String **ud = nullptr;
			sq_getuserdata(vm, -1, (SQUserPointer*)&ud, nullptr);
			sq_pop(vm, 1);
			errorMessage = **ud;
			sq_pushstring(vm, "__error", -1);
			sq_rawdeleteslot(vm, -2, false);
			sq_pop(vm, 1);
			return errorMessage.c_str();
		}
		sq_pushstring(vm, "__error", -1);
		sq_rawdeleteslot(vm, -2, false);
		sq_pop(vm, 1);
		return "an unknown error has occurred";
#else
		return "";
#endif
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Returns whether a Sqrat error has occurred with a given VM
	///
	/// \param vm Target VM
	///
	/// \return True if an error has occurred, otherwise false
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static bool Occurred(HSQUIRRELVM vm)
	{
#if !defined(SCRAT_NO_ERROR_CHECKING)
		sq_pushregistrytable(vm);
		sq_pushstring(vm, "__error", -1);
		if(SQ_SUCCEEDED(sq_rawget(vm, -2)))
		{
			sq_pop(vm, 2);
			return true;
		}
		sq_pop(vm, 1);
#endif
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Raises an error in a given VM with a given error message
	///
	/// \param vm  Target VM
	/// \param err A nice error message
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void Throw(HSQUIRRELVM vm, const String& err)
	{
#if !defined(SCRAT_NO_ERROR_CHECKING)
		sq_pushregistrytable(vm);
		sq_pushstring(vm, "__error", -1);
		if(SQ_FAILED(sq_rawget(vm, -2)))
		{
			sq_pushstring(vm, "__error", -1);
			String** ud = reinterpret_cast<String**>(sq_newuserdata(vm, sizeof(String*)));
			*ud = new String(err);
			sq_setreleasehook(vm, -1, &error_cleanup_hook);
			sq_rawset(vm, -3);
			sq_pop(vm, 1);
			return;
		}
		sq_pop(vm, 2);
#endif
	}

private:
	Error() {}

	static SQInteger error_cleanup_hook(SQUserPointer ptr, SQInteger /*size*/)
	{
		String **ud = reinterpret_cast<String**>(ptr);
		delete *ud;
		return 0;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Returns a string that has been formatted to give a nice type error message (for usage with Class::SquirrelFunc)
///
/// \param vm           VM the error occurred with
/// \param idx          Index on the stack of the argument that had a type error
/// \param expectedType The name of the type that the argument should have been
///
/// \return String containing a nice type error message
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline String FormatTypeError(HSQUIRRELVM vm, SQInteger idx, const String& expectedType)
{
	String err;
#if !defined(SCRAT_NO_ERROR_CHECKING)
	const SQChar *actualType = "unknown";
	if(SQ_SUCCEEDED(sq_typeof(vm, idx)))
	{
		sq_tostring(vm, -1);
		sq_getstring(vm, -1, &actualType);
		sq_pop(vm, 2);
	}
	err = String::CreateFormattedString("wrong type (%s expected, got %s)", expectedType.c_str(), actualType);
#endif
	return err;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Returns the last error that occurred with a Squirrel VM (not associated with Sqrat errors)
///
/// \param vm Target VM
///
/// \return String containing a nice error message
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline String LastErrorString(HSQUIRRELVM vm)
{
	const SQChar* sqErr;
	sq_getlasterror(vm);
	if(sq_gettype(vm, -1) == OT_NULL)
	{
		sq_pop(vm, 1);
		return String();
	}
	sq_tostring(vm, -1);
	sq_getstring(vm, -1, &sqErr);
	sq_pop(vm, 2);
	return String(sqErr);
}

}
