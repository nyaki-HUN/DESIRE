#include "stdafx_Lua.h"
#include "LuaScriptComponent.h"

#include "Engine/Core/String/String.h"

LuaScriptComponent::LuaScriptComponent(Object& object, lua_State* L)
	: ScriptComponent(object)
	, m_L(L)
{
	sol::state_view lua(m_L);
	lua["self"] = this;
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

	Call(functionNames[static_cast<size_t>(funcType)]);
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
			if(from_L == m_L)
			{
				lua_pushvalue(m_L, argIdxOffsetFromTop);
			}
			else
			{
				lua_xmove(from_L, m_L, argIdxOffsetFromTop);
			}

			m_numFunctionCallArgs++;
		}

		ExecuteFunctionCall();
	}

	return 0;
}

bool LuaScriptComponent::PrepareFunctionCall(const String& functionName)
{
	lua_getglobal(m_L, functionName.Str());
	if(!lua_isfunction(m_L, -1))
	{
		lua_pop(m_L, 1);
		return false;
	}

	m_numFunctionCallArgs = 0;
	return true;
}

void LuaScriptComponent::ExecuteFunctionCall()
{
	const int statusCode = lua_pcall(m_L, m_numFunctionCallArgs, 0, 0);

	switch(statusCode)
	{
		case LUA_OK:			break;
		case LUA_ERRRUN:		LOG_ERROR("Execution error: %s", lua_tostring(m_L, -1)); break;
		case LUA_ERRSYNTAX:		LOG_ERROR("Syntax error: %s", lua_tostring(m_L, -1)); break;
		case LUA_ERRMEM:		LOG_ERROR("Memory error: %s", lua_tostring(m_L, -1)); break;
		case LUA_ERRERR:		LOG_ERROR("Error in ERROR HANDLER: %s", lua_tostring(m_L, -1)); break;
	}
}

bool LuaScriptComponent::AddFunctionCallArg(int arg)
{
	lua_pushinteger(m_L, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(float arg)
{
	lua_pushnumber(m_L, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(double arg)
{
	lua_pushnumber(m_L, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(bool arg)
{
	lua_pushboolean(m_L, arg);
	m_numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(void* pArg)
{
	ASSERT(pArg != nullptr);

	lua_pushlightuserdata(m_L, pArg);
	m_numFunctionCallArgs++;
	return true;
}

bool LuaScriptComponent::AddFunctionCallArg(const String& arg)
{
	lua_pushlstring(m_L, arg.Str(), arg.Length());
	m_numFunctionCallArgs++;
	return true;
}
