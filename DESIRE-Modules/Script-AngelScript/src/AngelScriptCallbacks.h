#pragma once

class AngelScriptCallbacks
{
public:
	static void* MallocWrapper(size_t size);
	static void FreeWrapper(void* pMemory);

	static void PrintCallback(asIScriptGeneric* pGeneric);
	static void MessageCallback(const asSMessageInfo* pMessage, void* pUserData);
	static asIScriptContext* RequestContextCallback(asIScriptEngine* pEngine, void* pUserData);
	static void ReturnContextCallback(asIScriptEngine* pEngine, asIScriptContext* pContext, void* pUserData);

	static void ExceptionCallback(asIScriptContext* pContext, void* pUserData);
	static void LineCallback(asIScriptContext* pContext, void* pUserData);		// The line callback function is called by the VM for each statement that is executed
};
