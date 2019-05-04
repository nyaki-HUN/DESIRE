#include "AngelScriptSystem.h"
#include "AngelScriptComponent.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/fs/FileSystem.h"
#include "Engine/Core/fs/IReadFile.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/String/StackString.h"
#include "Engine/Scene/Object.h"
#include "Engine/Utils/Enumerator.h"

#define CONTEXT_POOL_DEFAULT_SIZE	10

AngelScriptSystem::AngelScriptSystem()
{
	engine = asCreateScriptEngine();
	int result = engine->SetMessageCallback(asFUNCTION(MessageCallback), this, asCALL_CDECL);													ASSERT(result >= asSUCCESS);
	result = engine->SetContextCallbacks(&AngelScriptSystem::RequestContextCallback, &AngelScriptSystem::ReturnContextCallback, this);			ASSERT(result >= asSUCCESS);

	result = engine->SetEngineProperty(asEEngineProp::asEP_REQUIRE_ENUM_SCOPE, false);															ASSERT(result >= asSUCCESS);
	result = engine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_GLOBAL_VARS, true);															ASSERT(result >= asSUCCESS);
	result = engine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, true);											ASSERT(result >= asSUCCESS);
	result = engine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_EMPTY_LIST_ELEMENTS, true);													ASSERT(result >= asSUCCESS);

#if defined(DESIRE_DISTRIBUTION)
	result = engine->SetEngineProperty(asEEngineProp::asEP_BUILD_WITHOUT_LINE_CUES, true);														ASSERT(result >= asSUCCESS);
#else
	result = engine->SetEngineProperty(asEEngineProp::asEP_BUILD_WITHOUT_LINE_CUES, false);														ASSERT(result >= asSUCCESS);
#endif

	// Register Script API
	RegisterStdString(engine);
	RegisterCoreAPI_AngelScript(engine);
	RegisterInputAPI_AngelScript(engine);
	RegisterNetworkAPI_AngelScript(engine);
	RegisterPhysicsAPI_AngelScript(engine);
	RegisterRenderAPI_AngelScript(engine);
	RegisterSceneAPI_AngelScript(engine);
	RegisterSoundAPI_AngelScript(engine);

	result = engine->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(AngelScriptSystem::PrintCallback), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);

	// ScriptComponent
	ANGELSCRIPT_API_REGISTER_COMPONENT(ScriptComponent);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);

	// Init context pool
	contextPool.Reserve(CONTEXT_POOL_DEFAULT_SIZE);
	for(int i = 0; i < CONTEXT_POOL_DEFAULT_SIZE; i++)
	{
		contextPool.Add(CreateScriptContext());
	}
}

AngelScriptSystem::~AngelScriptSystem()
{
	for(asIScriptContext *ctx : contextPool)
	{
		ctx->Release();
	}
	contextPool.Clear();

	engine->ShutDownAndRelease();
	engine = nullptr;
}

ScriptComponent* AngelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const char *scriptName)
{
	ASSERT(scriptName != nullptr);

	asIScriptModule *module = engine->GetModule(scriptName);
	if(module == nullptr)
	{
		module = CompileScript(scriptName, engine);
		if(module == nullptr)
		{
			return nullptr;
		}
	}

	// Get factory function
	asIScriptFunction *factoryFunc = (asIScriptFunction*)module->GetUserData();
	if(factoryFunc == nullptr)
	{
		return nullptr;
	}

	AngelScriptComponent *scriptComponent = &object.AddComponent<AngelScriptComponent>();

	// Call the constructor
	asIScriptContext *ctx = engine->RequestContext();
	ctx->Prepare(factoryFunc);
	ctx->SetArgObject(0, scriptComponent);
	int result = ctx->Execute();
	if(result == asEXECUTION_FINISHED)
	{
		// Get the object that was created and increase the reference, otherwise it would be destroyed when the context is reused or destroyed
		scriptComponent->scriptObject = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
		scriptComponent->scriptObject->AddRef();
	}
	else
	{
		object.RemoveComponent(scriptComponent);
		scriptComponent = nullptr;
	}

	engine->ReturnContext(ctx);

	return scriptComponent;
}

asIScriptModule* AngelScriptSystem::CompileScript(const char *scriptName, asIScriptEngine *engine)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.as", scriptName);
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename.Str());
		return nullptr;
	}

	MemoryBuffer content = file->ReadFileContent();
	DynamicString scriptSrc = DynamicString::Format(
		"class %s"
		"{"
		"	ScriptComponent @self;"
		"	TestScript(ScriptComponent @component) { @self = component; }"
		, scriptName);
	scriptSrc.Append(content.data, content.size);
	scriptSrc += "}";

	asIScriptModule *module = engine->GetModule(scriptName, asGM_ALWAYS_CREATE);
	module->AddScriptSection(scriptName, scriptSrc.Str(), scriptSrc.Length());
	int result = module->Build();
	if(result != asSUCCESS)
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return module;
	}

	asITypeInfo *typeInfo = engine->GetTypeInfoById(module->GetTypeIdByDecl(scriptName));
	
	// Cache factory in the script module
	asIScriptFunction *factoryFunc = typeInfo->GetFactoryByDecl(DynamicString::Format("%s@ %s(ScriptComponent @)", scriptName, scriptName).Str());
	module->SetUserData(factoryFunc);
	
	// Cache built-in functions in the object type
	const char *builtinFunctionNames[] =
	{
		"Update",
		"Init",
		"Kill",
	};
	DESIRE_CHECK_ARRAY_SIZE(builtinFunctionNames, ScriptComponent::EBuiltinFuncType::Num);

	for(auto i : Enumerator<ScriptComponent::EBuiltinFuncType>())
	{
		asIScriptFunction *func = typeInfo->GetMethodByName(builtinFunctionNames[i]);
		typeInfo->SetUserData(func, i);
	}

	return module;
}

asIScriptContext* AngelScriptSystem::CreateScriptContext()
{
	ASSERT(engine != nullptr);

	asIScriptContext *ctx = engine->CreateContext();
	int result = ctx->SetExceptionCallback(asMETHODPR(AngelScriptSystem, ExceptionCallback, (asIScriptContext *), void), this, asCALL_THISCALL);
	ASSERT(result == asSUCCESS);
	result = ctx->SetLineCallback(asMETHODPR(AngelScriptSystem, LineCallback, (asIScriptContext *), void), this, asCALL_THISCALL);
	ASSERT(result == asSUCCESS);

	return ctx;
}

bool AngelScriptSystem::IsBreakpoint(const char *scriptSection, int line, asIScriptFunction *function) const
{
	DESIRE_UNUSED(scriptSection);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(function);

	DESIRE_TODO("Add debugging functionality");
	return false;
}

void AngelScriptSystem::PrintCallback(asIScriptGeneric *gen)
{
	const std::string *message = static_cast<const std::string*>(gen->GetArgObject(0));
	LOG_DEBUG("%s", message->c_str());
}

void AngelScriptSystem::MessageCallback(const asSMessageInfo *msg, void *thisPtr)
{
	DESIRE_UNUSED(thisPtr);

	switch(msg->type)
	{
		case asMSGTYPE_ERROR:
			LOG_ERROR("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
		case asMSGTYPE_WARNING:
			LOG_WARNING("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
		case asMSGTYPE_INFORMATION:
			LOG_MESSAGE("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
	}
}

asIScriptContext* AngelScriptSystem::RequestContextCallback(asIScriptEngine *engine, void *thisPtr)
{
	DESIRE_UNUSED(engine);

	AngelScriptSystem *scriptSystem = static_cast<AngelScriptSystem*>(thisPtr);

	asIScriptContext *ctx = nullptr;
	if(scriptSystem->contextPool.IsEmpty())
	{
		ctx = scriptSystem->CreateScriptContext();
	}
	else
	{
		ctx = scriptSystem->contextPool.GetLast();
		scriptSystem->contextPool.RemoveLast();
	}

	return ctx;
}

void AngelScriptSystem::ReturnContextCallback(asIScriptEngine *engine, asIScriptContext *ctx, void *thisPtr)
{
	DESIRE_UNUSED(engine);

	ctx->Unprepare();

	AngelScriptSystem *scriptSystem = static_cast<AngelScriptSystem*>(thisPtr);
	scriptSystem->contextPool.Add(ctx);
}

void AngelScriptSystem::ExceptionCallback(asIScriptContext *ctx)
{
	LOG_ERROR("Exception: %s", ctx->GetExceptionString());
	const asIScriptFunction *function = ctx->GetExceptionFunction();
	LOG_ERROR("module: %s", function->GetModuleName());
	LOG_ERROR("func: %s", function->GetDeclaration());
//	LOG_ERROR("section: %s", function->GetScriptSectionName());
	LOG_ERROR("line: %d", ctx->GetExceptionLineNumber());
}

void AngelScriptSystem::LineCallback(asIScriptContext *ctx)
{
	// Determine if we have reached a break point
	int column;
	const char *scriptSection;
	int line = ctx->GetLineNumber(0, &column, &scriptSection);
	asIScriptFunction *function = ctx->GetFunction();
	if(IsBreakpoint(scriptSection, line, function))
	{
		// A breakpoint has been reached so the execution of the script should be suspended
		ctx->Suspend();

		// Show the call stack
		for(asUINT i = 0; i < ctx->GetCallstackSize(); ++i)
		{
			asIScriptFunction *func = ctx->GetFunction(i);
			line = ctx->GetLineNumber(i, &column, &scriptSection);
			LOG_MESSAGE("%s:%s:%d,%d\n", scriptSection, func->GetDeclaration(), line, column);
		}
	}
}
