#include "LuaScriptComponent.h"

#include "Engine/Common.h"

#include "Engine/Core/String/String.h"

#include "sol/sol.hpp"

LuaScriptComponent::LuaScriptComponent(Object& object, lua_State* L)
	: ScriptComponent(object)
	, L(L)
{
	sol::state_view lua(L);
	lua["self"] = this;
}

LuaScriptComponent::~LuaScriptComponent()
{
}

void LuaScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	const String functionNames[] =
	{
		"Update",
		"Init",
		"Kill"
	};
	DESIRE_CHECK_ARRAY_SIZE(functionNames, EBuiltinFuncType::Num);

	Call(functionNames[(size_t)funcType]);
}

int LuaScriptComponent::CallFromScript(sol::this_state ts)
{
	lua_State* from_L = ts;

	// argCount is the number of arguments to call the script function with (-2 because we need to exclude the instance and the function name)
	const int argCount = lua_gettop(from_L) - 2;
	size_t size = 0;
	const char* functionName = lua_tolstring(from_L, -1 - argCount, &size);

	if(PrepareFunctionCall(String(functionName, size)))
	{
		// Push the args
		const int argIdxOffsetFromTop = -argCount - 1;	// -1 because the function is pushed in PrepareFunctionCall()
		for(int i = 0; i < argCount; ++i)
		{
			if(from_L == L)
			{
				lua_pushvalue(L, argIdxOffsetFromTop);
			}
			else
			{
				lua_xmove(from_L, L, argIdxOffsetFromTop);
			}

			numFunctionCallArgs++;
		}

		ExecuteFunctionCall();
	}

	return 0;
}

bool LuaScriptComponent::PrepareFunctionCall(const String& functionName)
{
	lua_getglobal(L, functionName.Str());
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return false;
	}

	numFunctionCallArgs = 0;
	return true;
}

void LuaScriptComponent::ExecuteFunctionCall()
{
	const int statusCode = lua_pcall(L, numFunctionCallArgs, 0, 0);

	switch(statusCode)
	{
		case LUA_OK:			break;
		case LUA_ERRRUN:		LOG_ERROR("Execution error: %s", lua_tostring(L, -1)); break;
		case LUA_ERRSYNTAX:		LOG_ERROR("Syntax error: %s", lua_tostring(L, -1)); break;
		case LUA_ERRMEM:		LOG_ERROR("Memory error: %s", lua_tostring(L, -1)); break;
		case LUA_ERRERR:		LOG_ERROR("Error in ERROR HANDLER: %s", lua_tostring(L, -1)); break;
	}
}

bool LuaScriptComponent::AddFunctionCallArg(int arg)
{
	lua_pushinteger(L, arg);
	numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(float arg)
{
	lua_pushnumber(L, arg);
	numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(double arg)
{
	lua_pushnumber(L, arg);
	numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(bool arg)
{
	lua_pushboolean(L, arg);
	numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(void* arg)
{
	lua_pushlightuserdata(L, arg);
	numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(const String& arg)
{
	lua_pushlstring(L, arg.Str(), arg.Length());
	numFunctionCallArgs++;
	return true;
}
