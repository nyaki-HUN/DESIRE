#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratTypes: Type Translators
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
#include "sqratClassType.h"
#include "sqratUtil.h"

namespace Sqrat {

template <typename T, bool b>
struct popAsInt
{
	T value;
	popAsInt(HSQUIRRELVM vm, SQInteger idx)
	{
		SQObjectType value_type = sq_gettype(vm, idx);
		switch(value_type)
		{
			case OT_BOOL:
				SQBool sqValueb;
				sq_getbool(vm, idx, &sqValueb);
				value = static_cast<T>(sqValueb);
				break;
			case OT_INTEGER:
				SQInteger sqValue;
				sq_getinteger(vm, idx, &sqValue);
				value = static_cast<T>(sqValue);
				break;
			case OT_FLOAT:
				SQFloat sqValuef;
				sq_getfloat(vm, idx, &sqValuef);
				value = static_cast<T>(static_cast<int>(sqValuef));
				break;

			case OT_NULL:
			case OT_STRING:
			case OT_TABLE:
			case OT_ARRAY:
			case OT_USERDATA:
			case OT_CLOSURE:
			case OT_NATIVECLOSURE:
			case OT_GENERATOR:
			case OT_USERPOINTER:
			case OT_THREAD:
			case OT_FUNCPROTO:
			case OT_CLASS:
			case OT_INSTANCE:
			case OT_WEAKREF:
			case OT_OUTER:
			default:
				Error::Throw(vm, FormatTypeError(vm, idx, "integer"));
				value = static_cast<T>(0);
				break;
		}
	}
};

template <typename T>
struct popAsInt<T, false>
{
	T value;  // cannot be initialized because unknown constructor parameters
	popAsInt(HSQUIRRELVM /*vm*/, SQInteger /*idx*/)
	{
		// keep the current error message already set previously, do not touch that here
	}
};

template <typename T>
struct popAsFloat
{
	T value;
	popAsFloat(HSQUIRRELVM vm, SQInteger idx)
	{
		SQObjectType value_type = sq_gettype(vm, idx);
		switch(value_type)
		{
			case OT_BOOL:
				SQBool sqValueb;
				sq_getbool(vm, idx, &sqValueb);
				value = static_cast<T>(sqValueb);
				break;

			case OT_INTEGER:
				SQInteger sqValue;
				sq_getinteger(vm, idx, &sqValue);
				value = static_cast<T>(sqValue);
				break;

			case OT_FLOAT:
				SQFloat sqValuef;
				sq_getfloat(vm, idx, &sqValuef);
				value = static_cast<T>(sqValuef);
				break;

			case OT_NULL:
			case OT_STRING:
			case OT_TABLE:
			case OT_ARRAY:
			case OT_USERDATA:
			case OT_CLOSURE:
			case OT_NATIVECLOSURE:
			case OT_GENERATOR:
			case OT_USERPOINTER:
			case OT_THREAD:
			case OT_FUNCPROTO:
			case OT_CLASS:
			case OT_INSTANCE:
			case OT_WEAKREF:
			case OT_OUTER:
			default:
				Error::Throw(vm, FormatTypeError(vm, idx, "float"));
				value = 0;
				break;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as copies
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/// \remarks
/// This specialization requires T to have a default constructor.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var
{
	T value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
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
		T* ptr = ClassType<T>::GetInstance(vm, idx);
		if(ptr != NULL)
		{
			value = *ptr;
#if !defined (SCRAT_NO_ERROR_CHECKING)
		}
		else if(std::is_convertible<T, SQInteger>::value)
		{ /* value is likely of integral type like enums */
			Error::Clear(vm); // clear the previous error
			value = popAsInt<T, std::is_convertible<T, SQInteger>::value>(vm, idx).value;
#endif
		}
		else
		{
			// initialize value to avoid warnings
			value = popAsInt<T, std::is_convertible<T, SQInteger>::value>(vm, idx).value;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const T& value)
	{
		if(ClassType<T>::hasClassData(vm))
			ClassType<T>::PushInstanceCopy(vm, value);
		else /* try integral type */
			pushAsInt<T, std::is_convertible<T, SQInteger>::value>().push(vm, value);
	}

private:
	template <class T2, bool b>
	struct pushAsInt
	{
		void push(HSQUIRRELVM vm, const T2& /*value*/)
		{
			assert(false); // fails because called before a Sqrat::Class for T exists and T is not convertible to SQInteger
			sq_pushnull(vm);
		}
	};

	template <class T2>
	struct pushAsInt<T2, true>
	{
		void push(HSQUIRRELVM vm, const T2& value)
		{
			sq_pushinteger(vm, static_cast<SQInteger>(value));
		}
	};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as references
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<T&>
{
	T& value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(*ClassType<T>::GetInstance(vm, idx))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVarR to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, T& value)
	{
		if(ClassType<T>::hasClassData(vm))
			ClassType<T>::PushInstance(vm, &value);
		else /* try integral type */
			pushAsInt<T, std::is_convertible<T, SQInteger>::value>().push(vm, value);
	}

private:
	template <class T2, bool b>
	struct pushAsInt
	{
		void push(HSQUIRRELVM vm, const T2& /*value*/)
		{
			assert(false); // fails because called before a Sqrat::Class for T exists and T is not convertible to SQInteger
			sq_pushnull(vm);
		}
	};

	template <class T2>
	struct pushAsInt<T2, true>
	{
		void push(HSQUIRRELVM vm, const T2& value)
		{
			sq_pushinteger(vm, static_cast<SQInteger>(value));
		}
	};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as pointers
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<T*>
{
	T *value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(ClassType<T>::GetInstance(vm, idx, true))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, T* value)
	{
		ClassType<T>::PushInstance(vm, value);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as pointers to const data
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<T* const>
{
	T* const value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(ClassType<T>::GetInstance(vm, idx, true))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, T* const value)
	{
		ClassType<T>::PushInstance(vm, value);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as const references
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<const T&> {

    const T& value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(*ClassType<T>::GetInstance(vm, idx))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const T& value)
	{
		ClassType<T>::PushInstanceCopy(vm, value);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as const pointers
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<const T*>
{
	const T* value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(ClassType<T>::GetInstance(vm, idx, true))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const T* value)
	{
		ClassType<T>::PushInstance(vm, const_cast<T*>(value));
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push class instances to and from the stack as const pointers to const data
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Var<const T* const>
{
	const T* const value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/// \remarks
	/// This function MUST have its Error handled if it occurred.
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
		: value(ClassType<T>::GetInstance(vm, idx, true))
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const T* const value)
	{
		ClassType<T>::PushInstance(vm, const_cast<T*>(value));
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get (as copies) and push (as references) class instances to and from the stack as a std::shared_ptr
///
/// \tparam T Type of instance (usually doesnt need to be defined explicitly)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> void PushVarR(HSQUIRRELVM vm, T& value);

template<class T>
struct Var<std::shared_ptr<T>>
{
	std::shared_ptr<T> value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as the given type
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
		if(sq_gettype(vm, idx) != OT_NULL)
		{
			Var<T> instance(vm, idx);
			if(Error::Occurred(vm))
			{
				return;
			}
			value.Init(new T(instance.value));
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a class object on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const std::shared_ptr<T>& value)
	{
		PushVarR(vm, *value);
	}
};

// Integer types
#define SCRAT_INTEGER(type)										\
	template<>													\
	struct Var<type> {											\
		type value;												\
		Var(HSQUIRRELVM vm, SQInteger idx) {					\
			value = popAsInt<type, true>(vm, idx).value;		\
		}														\
		static void push(HSQUIRRELVM vm, const type& value) {	\
			sq_pushinteger(vm, static_cast<SQInteger>(value));	\
		}														\
	};															\
	template<>													\
	struct Var<const type&> {									\
		type value;												\
		Var(HSQUIRRELVM vm, SQInteger idx) {					\
			value = popAsInt<type, true>(vm, idx).value;		\
		}														\
		static void push(HSQUIRRELVM vm, const type& value) {	\
			sq_pushinteger(vm, static_cast<SQInteger>(value));	\
		}														\
	};

SCRAT_INTEGER(uint32_t)
SCRAT_INTEGER(int32_t)
SCRAT_INTEGER(uint16_t)
SCRAT_INTEGER(int16_t)
SCRAT_INTEGER(uint8_t)
SCRAT_INTEGER(int8_t)
SCRAT_INTEGER(uint64_t)
SCRAT_INTEGER(int64_t)

// Float types
#define SCRAT_FLOAT(type)										\
	template<>													\
	struct Var<type> {											\
		type value;												\
		Var(HSQUIRRELVM vm, SQInteger idx) {					\
			value = popAsFloat<type>(vm, idx).value;			\
		}														\
		static void push(HSQUIRRELVM vm, const type& value) {	\
			sq_pushfloat(vm, static_cast<SQFloat>(value));		\
		}														\
	};															\
	template<>													\
	struct Var<const type&> {									\
		type value;												\
		Var(HSQUIRRELVM vm, SQInteger idx) {					\
			value = popAsFloat<type>(vm, idx).value;			\
		}														\
		static void push(HSQUIRRELVM vm, const type& value) {	\
			sq_pushfloat(vm, static_cast<SQFloat>(value));		\
		}														\
	};

SCRAT_FLOAT(float)
SCRAT_FLOAT(double)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push bools to and from the stack
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<bool>
{
	bool value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a bool
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		SQBool sqValue;
		sq_tobool(vm, idx, &sqValue);
		value = (sqValue != 0);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a bool on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const bool& value)
	{
		sq_pushbool(vm, static_cast<SQBool>(value));
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push const bool references to and from the stack
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<const bool&>
{
	bool value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a bool
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		SQBool sqValue;
		sq_tobool(vm, idx, &sqValue);
		value = (sqValue != 0);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a bool on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const bool& value)
	{
		sq_pushbool(vm, static_cast<SQBool>(value));
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push SQChar arrays to and from the stack (usually is a char array)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<SQChar*>
{
private:
	HSQOBJECT obj; /* hold a reference to the object holding value during the Var struct lifetime*/
	HSQUIRRELVM v;

public:
	SQChar* value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a character array
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		sq_tostring(vm, idx);
		sq_getstackobj(vm, -1, &obj);
		sq_getstring(vm, -1, (const SQChar**)&value);
		sq_addref(vm, &obj);
		sq_pop(vm, 1);
		v = vm;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Destructor
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	~Var()
	{
		if(v && !sq_isnull(obj))
		{
			sq_release(v, &obj);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a character array on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	/// \param len   Length of the string (defaults to finding the length by searching for a terminating null-character)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const SQChar* value, SQInteger len = -1)
	{
		sq_pushstring(vm, value, len);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push const SQChar arrays to and from the stack (usually is a const char array)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<const SQChar*>
{
private:
	HSQOBJECT obj; /* hold a reference to the object holding value during the Var struct lifetime*/
	HSQUIRRELVM v;

public:
	const SQChar* value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a character array
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		sq_tostring(vm, idx);
		sq_getstackobj(vm, -1, &obj);
		sq_getstring(vm, -1, &value);
		sq_addref(vm, &obj);
		sq_pop(vm, 1);
		v = vm;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Destructor
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	~Var()
	{
		if(v && !sq_isnull(obj))
		{
			sq_release(v, &obj);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a character array on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	/// \param len   Length of the string (defaults to finding the length by searching for a terminating null-character)
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const SQChar* value, SQInteger len = -1)
	{
		sq_pushstring(vm, value, len);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push strings to and from the stack (string is usually std::string)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<DynamicString>
{
	DynamicString value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a string
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		const SQChar* ret;
		sq_tostring(vm, idx);
		sq_getstring(vm, -1, &ret);
		value = DynamicString(ret, (size_t)sq_getsize(vm, -1));
		sq_pop(vm, 1);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a string on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const String& value)
	{
		sq_pushstring(vm, value.Str(), (SQInteger)value.Length());
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Used to get and push const string references to and from the stack as copies (strings are always copied)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<>
struct Var<const DynamicString&>
{
	DynamicString value; ///< The actual value of get operations

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Attempts to get the value off the stack at idx as a string
	///
	/// \param vm  Target VM
	/// \param idx Index trying to be read
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Var(HSQUIRRELVM vm, SQInteger idx)
	{
		const SQChar* ret;
		sq_tostring(vm, idx);
		sq_getstring(vm, -1, &ret);
		value = DynamicString(ret, (size_t)sq_getsize(vm, -1));
		sq_pop(vm, 1);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Called by Sqrat::PushVar to put a string on the stack
	///
	/// \param vm    Target VM
	/// \param value Value to push on to the VM's stack
	///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void push(HSQUIRRELVM vm, const String& value)
	{
		sq_pushstring(vm, value.Str(), (SQInteger)value.Length());
	}
};

// Non-referencable type definitions
template<class T> struct is_referencable {static const bool value = true;};

#define SCRAT_MAKE_NONREFERENCABLE( type ) \
 template<> struct is_referencable<type> {static const bool value = false;};

SCRAT_MAKE_NONREFERENCABLE(uint32_t)
SCRAT_MAKE_NONREFERENCABLE(int32_t)
SCRAT_MAKE_NONREFERENCABLE(uint16_t)
SCRAT_MAKE_NONREFERENCABLE(int16_t)
SCRAT_MAKE_NONREFERENCABLE(uint8_t)
SCRAT_MAKE_NONREFERENCABLE(int8_t)
SCRAT_MAKE_NONREFERENCABLE(uint64_t)
SCRAT_MAKE_NONREFERENCABLE(int64_t)
SCRAT_MAKE_NONREFERENCABLE(float)
SCRAT_MAKE_NONREFERENCABLE(double)
SCRAT_MAKE_NONREFERENCABLE(bool)
SCRAT_MAKE_NONREFERENCABLE(String)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Pushes a value on to a given VM's stack
///
/// \param vm    VM that the variable will be pushed on to the stack of
/// \param value The actual value being pushed
///
/// \tparam T Type of value (usually doesnt need to be defined explicitly)
///
/// \remarks
/// What this function does is defined by Sqrat::Var template specializations,
/// and thus you can create custom functionality for it by making new template specializations.
/// When making a custom type that is not referencable, you must use SCRAT_MAKE_NONREFERENCABLE( type )
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
inline void PushVar(HSQUIRRELVM vm, T* value)
{
	Var<T*>::push(vm, value);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Pushes a value on to a given VM's stack
///
/// \param vm    VM that the variable will be pushed on to the stack of
/// \param value The actual value being pushed
///
/// \tparam T Type of value (usually doesnt need to be defined explicitly)
///
/// \remarks
/// What this function does is defined by Sqrat::Var template specializations,
/// and thus you can create custom functionality for it by making new template specializations.
/// When making a custom type that is not referencable, you must use SCRAT_MAKE_NONREFERENCABLE( type )
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
inline void PushVar(HSQUIRRELVM vm, const T& value)
{
	Var<T>::push(vm, value);
}

template<class T, bool b>
struct PushVarR_helper
{
	inline static void push(HSQUIRRELVM vm, T value)
	{
		PushVar<T>(vm, value);
	}
};
template<class T>
struct PushVarR_helper<T, false>
{
	inline static void push(HSQUIRRELVM vm, const T& value)
	{
		PushVar<const T&>(vm, value);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Pushes a reference on to a given VM's stack (some types cannot be referenced and will be copied instead)
///
/// \param vm    VM that the reference will be pushed on to the stack of
/// \param value The actual referenced value being pushed
///
/// \tparam T Type of value (usually doesnt need to be defined explicitly)
///
/// \remarks
/// What this function does is defined by Sqrat::Var template specializations,
/// and thus you can create custom functionality for it by making new template specializations.
/// When making a custom type that is not referencable, you must use SCRAT_MAKE_NONREFERENCABLE( type )
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
inline void PushVarR(HSQUIRRELVM vm, T& value)
{
	if(!std::is_pointer<T>::value)
	{
		if(is_referencable<typename std::remove_cv<T>::type>::value)
		{
			Var<T&>::push(vm, value);
			return;
		}
	}

	PushVarR_helper<T, std::is_pointer<T>::value>::push(vm, value);
}

}
