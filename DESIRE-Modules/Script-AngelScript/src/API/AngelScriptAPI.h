#pragma once

#include "Engine/Core/String/String.h"

// Template function for registering a class derived from Component
#define ANGELSCRIPT_API_REGISTER_COMPONENT(CLASS)																																				\
	result = engine.RegisterEnumValue("EComponent", "k"#CLASS, CLASS::kTypeId);																					ASSERT(result >= asSUCCESS);	\
	result = engine.RegisterObjectType(#CLASS, 0, asOBJ_REF | asOBJ_NOCOUNT);																					ASSERT(result >= asSUCCESS);	\
	result = engine.RegisterObjectMethod(#CLASS, "Object& GetObject() const", asMETHOD(Component, GetObject), asCALL_THISCALL);									ASSERT(result >= asSUCCESS);	\
	result = engine.RegisterObjectMethod(#CLASS, "Component@ opImplCast()", asFUNCTION((AngelScriptAPI<CLASS>::RefCast<Component>)), asCALL_CDECL_OBJLAST);		ASSERT(result >= asSUCCESS);	\
	result = engine.RegisterObjectMethod("Component", #CLASS"@ opImplCast()", asFUNCTION((AngelScriptAPI<Component>::RefCast<CLASS>)), asCALL_CDECL_OBJLAST);	ASSERT(result >= asSUCCESS)

// API register functions
void RegisterStdString(asIScriptEngine* pEngine);
void RegisterCoreAPI_Math_AngelScript(asIScriptEngine& engine);
void RegisterCoreAPI_AngelScript(asIScriptEngine& engine);
void RegisterInputAPI_AngelScript(asIScriptEngine& engine);
void RegisterPhysicsAPI_AngelScript(asIScriptEngine& engine);
void RegisterRenderAPI_AngelScript(asIScriptEngine& engine);
void RegisterSoundAPI_AngelScript(asIScriptEngine& engine);

template<class T>
class AngelScriptAPI
{
public:
	// Functions to be registered as asBEHAVE_CONSTRUCT
	static void Construct(T* pPtr)
	{
		new(pPtr) T();
	}

	template<class A1>
	static void ConstructWithArgs(T* pPtr, A1 a1)
	{
		new(pPtr) T(a1);
	}

	template<class A1, class A2>
	static void ConstructWithArgs(T* pPtr, A1 a1, A2 a2)
	{
		new(pPtr) T(a1, a2);
	}

	template<class A1, class A2, class A3>
	static void ConstructWithArgs(T* pPtr, A1 a1, A2 a2, A3 a3)
	{
		new(pPtr) T(a1, a2, a3);
	}

	template<class A1, class A2, class A3, class A4>
	static void ConstructWithArgs(T* pPtr, A1 a1, A2 a2, A3 a3, A4 a4)
	{
		new(pPtr) T(a1, a2, a3, a4);
	}

	// Function to be registered as asBEHAVE_DESTRUCT
	static void Destruct(T* pPtr)
	{
		pPtr->~T();
	}

	// Functions to be registered as asBEHAVE_FACTORY
	static T* Factory()
	{
		return new T();
	}

	template<class A1>
	static T* FactoryWithArgs(A1 a1)
	{
		return new T(a1);
	}

	template<class A1, class A2>
	static T* FactoryWithArgs(A1 a1, A2 a2)
	{
		return new T(a1, a2);
	}

	template<class A1, class A2, class A3>
	static T* FactoryWithArgs(A1 a1, A2 a2, A3 a3)
	{
		return new T(a1, a2, a3);
	}

	template<class A1, class A2, class A3, class A4>
	static T* FactoryWithArgs(A1 a1, A2 a2, A3 a3, A4 a4)
	{
		return new T(a1, a2, a3, a4);
	}

	// Function to be registered as asBEHAVE_RELEASE
	static void Release(T* ptr)
	{
		delete ptr;
	}

	// Function to be used for opCast and opImplCast
	template<class U>
	static T* RefCast(U* fromPtr)
	{
		return static_cast<T*>(fromPtr);
	}

	// Assignemt operators functions for asOBJ_SCOPED types
	template<class U> static void OpAssign(T& a, U b)			{ a = b; }
	template<class U> static void OpAddAssign(T& a, U b)		{ a += b; }
	template<class U> static void OpSubAssign(T& a, U b)		{ a -= b; }
	template<class U> static void OpMulAssign(T& a, U b)		{ a *= b; }
	template<class U> static void OpDivAssign(T& a, U b)		{ a /= b; }

	// Binary operators functions for asOBJ_SCOPED types
	template<class U> static T* OpAdd(const T& a, U b)			{ return new T(a + b); }
	template<class U> static T* OpSub(const T& a, U b)			{ return new T(a - b); }
	template<class U> static T* OpMul(const T& a, U b)			{ return new T(a * b); }
	template<class U> static U* OpMul_2(const T& a, const U& b)	{ return new U(a * b); }
	template<class U> static T* OpDiv(const T& a, U b)			{ return new T(a / b); }

	// Unary operators
	static T* OpNeg(const T& a)									{ return new T(-a); }
};

template<class T>
class AngelScriptGenericAPI
{
public:
	template<const String& (T::*func)() const>
	static void MakeStringRvFrom(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 0);
		const T* pSelf = static_cast<const T*>(pGeneric->GetObject());
		const String& rv = (pSelf->*func)();
		new (pGeneric->GetAddressOfReturnLocation()) std::string(rv.Str(), rv.Length());
	}

	template<T(*func)()>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 0);
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func());
	}

	template<class A0, T(*func)(A0)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 1);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0));
	}

	template<class A0, class A1, T(*func)(A0, A1)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 2);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		std::remove_reference<A1>::type* a1 = std::is_reference<A1>::value ? *static_cast<std::remove_reference<A1>::type**>(pGeneric->GetAddressOfArg(1)) : static_cast<std::remove_reference<A1>::type*>(pGeneric->GetAddressOfArg(1));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0, *a1));
	}

	template<class A0, class A1, class A2, T(*func)(A0, A1, A2)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 3);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		std::remove_reference<A1>::type* a1 = std::is_reference<A1>::value ? *static_cast<std::remove_reference<A1>::type**>(pGeneric->GetAddressOfArg(1)) : static_cast<std::remove_reference<A1>::type*>(pGeneric->GetAddressOfArg(1));
		std::remove_reference<A2>::type* a2 = std::is_reference<A2>::value ? *static_cast<std::remove_reference<A2>::type**>(pGeneric->GetAddressOfArg(2)) : static_cast<std::remove_reference<A2>::type*>(pGeneric->GetAddressOfArg(2));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0, *a1, *a2));
	}

	template<class A0, class A1, class A2, class A3, T(*func)(A0, A1, A2, A3)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 4);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		std::remove_reference<A1>::type* a1 = std::is_reference<A1>::value ? *static_cast<std::remove_reference<A1>::type**>(pGeneric->GetAddressOfArg(1)) : static_cast<std::remove_reference<A1>::type*>(pGeneric->GetAddressOfArg(1));
		std::remove_reference<A2>::type* a2 = std::is_reference<A2>::value ? *static_cast<std::remove_reference<A2>::type**>(pGeneric->GetAddressOfArg(2)) : static_cast<std::remove_reference<A2>::type*>(pGeneric->GetAddressOfArg(2));
		std::remove_reference<A3>::type* a3 = std::is_reference<A3>::value ? *static_cast<std::remove_reference<A3>::type**>(pGeneric->GetAddressOfArg(3)) : static_cast<std::remove_reference<A3>::type*>(pGeneric->GetAddressOfArg(3));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0, *a1, *a2, *a3));
	}

	template<class A0, class A1, class A2, class A3, class A4, T(*func)(A0, A1, A2, A3, A4)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 5);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		std::remove_reference<A1>::type* a1 = std::is_reference<A1>::value ? *static_cast<std::remove_reference<A1>::type**>(pGeneric->GetAddressOfArg(1)) : static_cast<std::remove_reference<A1>::type*>(pGeneric->GetAddressOfArg(1));
		std::remove_reference<A2>::type* a2 = std::is_reference<A2>::value ? *static_cast<std::remove_reference<A2>::type**>(pGeneric->GetAddressOfArg(2)) : static_cast<std::remove_reference<A2>::type*>(pGeneric->GetAddressOfArg(2));
		std::remove_reference<A3>::type* a3 = std::is_reference<A3>::value ? *static_cast<std::remove_reference<A3>::type**>(pGeneric->GetAddressOfArg(3)) : static_cast<std::remove_reference<A3>::type*>(pGeneric->GetAddressOfArg(3));
		std::remove_reference<A4>::type* a4 = std::is_reference<A4>::value ? *static_cast<std::remove_reference<A4>::type**>(pGeneric->GetAddressOfArg(4)) : static_cast<std::remove_reference<A4>::type*>(pGeneric->GetAddressOfArg(4));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0, *a1, *a2, *a3, *a4));
	}

	template<class A0, class A1, class A2, class A3, class A4, class A5, T(*func)(A0, A1, A2, A3, A4, A5)>
	static void StaticFunc(asIScriptGeneric* pGeneric)
	{
		ASSERT(pGeneric->GetArgCount() == 6);
		std::remove_reference<A0>::type* a0 = std::is_reference<A0>::value ? *static_cast<std::remove_reference<A0>::type**>(pGeneric->GetAddressOfArg(0)) : static_cast<std::remove_reference<A0>::type*>(pGeneric->GetAddressOfArg(0));
		std::remove_reference<A1>::type* a1 = std::is_reference<A1>::value ? *static_cast<std::remove_reference<A1>::type**>(pGeneric->GetAddressOfArg(1)) : static_cast<std::remove_reference<A1>::type*>(pGeneric->GetAddressOfArg(1));
		std::remove_reference<A2>::type* a2 = std::is_reference<A2>::value ? *static_cast<std::remove_reference<A2>::type**>(pGeneric->GetAddressOfArg(2)) : static_cast<std::remove_reference<A2>::type*>(pGeneric->GetAddressOfArg(2));
		std::remove_reference<A3>::type* a3 = std::is_reference<A3>::value ? *static_cast<std::remove_reference<A3>::type**>(pGeneric->GetAddressOfArg(3)) : static_cast<std::remove_reference<A3>::type*>(pGeneric->GetAddressOfArg(3));
		std::remove_reference<A4>::type* a4 = std::is_reference<A4>::value ? *static_cast<std::remove_reference<A4>::type**>(pGeneric->GetAddressOfArg(4)) : static_cast<std::remove_reference<A4>::type*>(pGeneric->GetAddressOfArg(4));
		std::remove_reference<A5>::type* a5 = std::is_reference<A5>::value ? *static_cast<std::remove_reference<A5>::type**>(pGeneric->GetAddressOfArg(5)) : static_cast<std::remove_reference<A5>::type*>(pGeneric->GetAddressOfArg(5));
		*static_cast<T**>(pGeneric->GetAddressOfReturnLocation()) = new T(func(*a0, *a1, *a2, *a3, *a4, *a5));
	}
};
