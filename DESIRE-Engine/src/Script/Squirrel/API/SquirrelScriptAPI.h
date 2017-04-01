#pragma once

#include "Script/Squirrel/sqrat/sqratTable.h"
#include "Script/Squirrel/sqrat/sqratClass.h"

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
	// Native squirrel function to support multiplication operator overrides (with 1 arguments, but different types)
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

private:
	// Multiplication base variant
	template<typename FirstType>
	static bool OpMul_Recursive(HSQUIRRELVM vm, Sqrat::Var<const T&> thisVar)
	{
		Sqrat::Var<FirstType> otherVar(vm, 2);
		if(!Sqrat::Error::Occurred(vm))
		{
			Sqrat::PushVar(vm, thisVar.value * otherVar.value);
			return true;
		}
		Sqrat::Error::Clear(vm);
		return false;
	}

	// Multiplication recursive variant to iterate over the types provided by the variadic template
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
};
