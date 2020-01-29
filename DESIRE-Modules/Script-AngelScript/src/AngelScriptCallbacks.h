#pragma once

class AngelScriptCallbacks
{
public:
	static void* MallocWrapper(size_t size);
	static void FreeWrapper(void* ptr);

	static void PrintCallback(asIScriptGeneric* gen);
	static void MessageCallback(const asSMessageInfo* msg, void* userData);
	static asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* userData);
	static void ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* userData);

	static void ExceptionCallback(asIScriptContext* ctx, void* userData);
	static void LineCallback(asIScriptContext* ctx, void* userData);		// The line callback function is called by the VM for each statement that is executed
};
