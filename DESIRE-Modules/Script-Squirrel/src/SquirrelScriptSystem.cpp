#include "SquirrelScriptSystem.h"
#include "SquirrelScriptComponent.h"
#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/fs/IReadFile.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/String/StackString.h"
#include "Engine/Scene/Object.h"
#include "Engine/Utils/Enumerator.h"

#include <stdarg.h>

SquirrelScriptSystem::SquirrelScriptSystem()
{
	// Create a VM with initial stack size 1024 
	vm = sq_open(1024);
	sq_setprintfunc(vm, &SquirrelScriptSystem::PrintCallback, &SquirrelScriptSystem::ErrorCallback);

#if defined(DESIRE_DISTRIBUTION)
	sq_enabledebuginfo(vm, SQFalse);
#else
	sq_enabledebuginfo(vm, SQTrue);
	sq_setnativedebughook(vm, &SquirrelScriptSystem::DebugHookCallback);
#endif

	sq_pushroottable(vm);

	// Set error handlers
	sq_newclosure(vm, RuntimeErrorHandler, 0);
	sq_seterrorhandler(vm);
	sq_setcompilererrorhandler(vm, &SquirrelScriptSystem::CompilerErrorCallback);
	sq_pop(vm, 1);

	// Register Script API
	Sqrat::RootTable rootTable(vm);
	RegisterCoreAPI_Squirrel(rootTable);
	RegisterComponentAPI_Squirrel(rootTable);
	RegisterInputAPI_Squirrel(rootTable);
	RegisterNetworkAPI_Squirrel(rootTable);
	RegisterPhysicsAPI_Squirrel(rootTable);
	RegisterRenderAPI_Squirrel(rootTable);
	RegisterSoundAPI_Squirrel(rootTable);
}

SquirrelScriptSystem::~SquirrelScriptSystem()
{
	sq_close(vm);
}

ScriptComponent* SquirrelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName)
{
	ASSERT(scriptName != nullptr);

	// Get factory function
	sq_pushroottable(vm);
	sq_pushstring(vm, scriptName, -1);
	SQRESULT result = sq_get(vm, -2);
	if(SQ_FAILED(result))
	{
		CompileScript(scriptName, vm);

		sq_pushstring(vm, scriptName, -1);
		result = sq_get(vm, -2);
		if(SQ_FAILED(result))
		{
			sq_pop(vm, 1);	// pop root table
			return nullptr;
		}
	}

	SquirrelScriptComponent& scriptComponent = object.AddComponent<SquirrelScriptComponent>(vm);

	// Call the constructor
	sq_pushroottable(vm);	// the 'this' parameter
	Sqrat::PushVar(vm, &scriptComponent);
	result = sq_call(vm, 2, true, true);
	if(SQ_SUCCEEDED(result))
	{
		sq_getstackobj(vm, -1, &scriptComponent.scriptObject);
		sq_addref(vm, &scriptComponent.scriptObject);

		sq_pop(vm, 1);	// pop instance

		// Cache built-in functions
		const char *builtinFunctionNames[] =
		{
			"Update",
			"Init",
			"Kill",
		};
		DESIRE_CHECK_ARRAY_SIZE(builtinFunctionNames, ScriptComponent::EBuiltinFuncType::Num);

		for(auto i : Enumerator<ScriptComponent::EBuiltinFuncType>())
		{
			sq_pushstring(vm, builtinFunctionNames[i], -1);
			result = sq_get(vm, -2);
			if(SQ_SUCCEEDED(result) && sq_gettype(vm, -1) == OT_CLOSURE)
			{
				sq_getstackobj(vm, -1, &scriptComponent.builtinFunctions[i]);
				sq_addref(vm, &scriptComponent.builtinFunctions[i]);

				sq_pop(vm, 1);
			}
		}
	}
	else
	{
		object.RemoveComponent(&scriptComponent);
	}

	sq_pop(vm, 2);	// pop class and root table

	return &scriptComponent;
}

void SquirrelScriptSystem::CompileScript(const char *scriptName, HSQUIRRELVM vm)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.nut", scriptName);
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename.Str());
		return;
	}

	MemoryBuffer content = file->ReadFileContent();
	DynamicString scriptSrc = DynamicString::Format(
		"class %s"
		"{"
		"	self = null;"
		"	constructor(component) { self = component; }"
		, scriptName);
	scriptSrc.Append(content.data, content.size);
	scriptSrc += "}";

	SQRESULT result = sq_compilebuffer(vm, scriptSrc.Str(), (SQInteger)scriptSrc.Length(), scriptName, SQTrue);
	if(SQ_FAILED(result))
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return;
	}

	HSQOBJECT scriptModule;
	sq_getstackobj(vm, -1, &scriptModule);
	ASSERT(sq_isclosure(scriptModule));

	// Execute script module
	sq_pushroottable(vm);
	result = sq_call(vm, 1 /*root table*/, false, true);
	ASSERT(SQ_SUCCEEDED(result));

	sq_pop(vm, 1);	// pop scriptModule
}

void SquirrelScriptSystem::PrintCallback(HSQUIRRELVM vm, const SQChar *format, ...)
{
	DESIRE_UNUSED(vm);

	Log::LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "DBG";

	va_list args;
	va_start(args, format);
	vsnprintf(logData.message, sizeof(logData.message), format, args);
	va_end(args);

	Log::LogWithData(logData);
}

void SquirrelScriptSystem::ErrorCallback(HSQUIRRELVM vm, const SQChar *format, ...)
{
	DESIRE_UNUSED(vm);

	Log::LogData logData;
	logData.file = __FILE__;
	logData.line = __LINE__;
	logData.logType = "ERR";

	va_list args;
	va_start(args, format);
	vsnprintf(logData.message, sizeof(logData.message), format, args);
	va_end(args);

	Log::LogWithData(logData);
}

void SquirrelScriptSystem::CompilerErrorCallback(HSQUIRRELVM vm, const SQChar *desc, const SQChar *source, SQInteger line, SQInteger column)
{
	DESIRE_UNUSED(vm);

	LOG_ERROR("%s(%d, %d): %s", source, line, column, desc);
}

void SquirrelScriptSystem::DebugHookCallback(HSQUIRRELVM vm, SQInteger type, const SQChar *sourcename, SQInteger line, const SQChar *funcname)
{
	DESIRE_UNUSED(vm);
	DESIRE_UNUSED(type);
	DESIRE_UNUSED(sourcename);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(funcname);

	DESIRE_TODO("Squirrel debugging");
}

SQInteger SquirrelScriptSystem::RuntimeErrorHandler(HSQUIRRELVM vm)
{
	const SQChar *errorStr = 0;
	if(sq_gettop(vm) >= 1)
	{
		if(SQ_SUCCEEDED(sq_getstring(vm, 2, &errorStr)))
		{
			LOG_ERROR("Runtime error: %s", errorStr);
		}
		else
		{
			LOG_ERROR("Unknown runtime error");
		}

		// TODO: print callstack, check sqstd_printcallstack() function
	}

	return 0;
}
