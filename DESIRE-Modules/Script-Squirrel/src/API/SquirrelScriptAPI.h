#pragma once

#include "sqrat/sqratTable.h"
#include "sqrat/sqratClass.h"

// Engine API register functions
void RegisterComponentAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterCoreAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterInputAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterNetworkAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterPhysicsAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable);
void RegisterSoundAPI_Squirrel(Sqrat::RootTable& rootTable);

template<class T>
class SquirrelScriptAPI
{
public:
	// Native squirrel function to support multiplication operator overrides with different types
	template<typename... TypeList>
	static SQInteger OpMulOverrides(HSQUIRRELVM vm)
	{
		if(sq_gettop(vm) != 2)
		{
			// Only 2 arguments are supported
			return 0;
		}

		Sqrat::Var<const T&> thisVar(vm, 1);
		if(OpMul_Recursive<TypeList...>(vm, thisVar) == false)
		{
			// If failed, just push back the original variable unchanged
			Sqrat::PushVar(vm, thisVar);
		}

		return 1;
	}

	// Native squirrel function to support division operator overrides with different types
	template<typename... TypeList>
	static SQInteger OpDivOverrides(HSQUIRRELVM vm)
	{
		if(sq_gettop(vm) != 2)
		{
			// Only 2 arguments are supported
			return 0;
		}

		Sqrat::Var<const T&> thisVar(vm, 1);
		if(OpDiv_Recursive<TypeList...>(vm, thisVar) == false)
		{
			// If failed, just push back the original variable unchanged
			Sqrat::PushVar(vm, thisVar);
		}

		return 1;
	}

private:
	// --------------------------------------------------------------------------------------------------------------------
	// Multiplication
	// --------------------------------------------------------------------------------------------------------------------

	// Recursive variant to iterate over the types provided by the variadic template
	template<typename FirstType, typename SecondType, typename... TypeList>
	static bool OpMul_Recursive(HSQUIRRELVM vm, Sqrat::Var<const T&> thisVar)
	{
		if(OpMul_Recursive<FirstType>(vm, thisVar))
		{
			return true;
		}

		// Try with the next type
		return OpMul_Recursive<SecondType, TypeList...>(vm, thisVar);
	}

	template<typename FirstType>
	static bool OpMul_Recursive(HSQUIRRELVM vm, Sqrat::Var<const T&> thisVar)
	{
		Sqrat::Var<FirstType> otherVar(vm, 2);
		if(Sqrat::Error::Occurred(vm))
		{
			Sqrat::Error::Clear(vm);
			return false;
		}

		Sqrat::PushVar(vm, thisVar.value * otherVar.value);
		return true;
	}

	// --------------------------------------------------------------------------------------------------------------------
	// Division
	// --------------------------------------------------------------------------------------------------------------------

	// Recursive variant to iterate over the types provided by the variadic template
	template<typename FirstType, typename SecondType, typename... TypeList>
	static bool OpDiv_Recursive(HSQUIRRELVM vm, Sqrat::Var<const T&> thisVar)
	{
		if(OpDiv_Recursive<FirstType>(vm, thisVar))
		{
			return true;
		}

		// Try with the next type
		return OpDiv_Recursive<SecondType, TypeList...>(vm, thisVar);
	}

	template<typename FirstType>
	static bool OpDiv_Recursive(HSQUIRRELVM vm, Sqrat::Var<const T&> thisVar)
	{
		Sqrat::Var<FirstType> otherVar(vm, 2);
		if(Sqrat::Error::Occurred(vm))
		{
			Sqrat::Error::Clear(vm);
			return false;
		}

		Sqrat::PushVar(vm, thisVar.value / otherVar.value);
		return true;
	}
};
