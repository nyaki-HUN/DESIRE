#include "stdafx_AngelScript.h"
#include "AngelScriptSystem.h"
#include "AngelScriptComponent.h"
#include "AngelScriptCallbacks.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Utils/Enumerator.h"

AngelScriptSystem::AngelScriptSystem()
{
	asSetGlobalMemoryFunctions(&AngelScriptCallbacks::MallocWrapper, &AngelScriptCallbacks::FreeWrapper);

	engine = asCreateScriptEngine();
	int result = engine->SetMessageCallback(asFUNCTION(AngelScriptCallbacks::MessageCallback), this, asCALL_CDECL);								ASSERT(result >= asSUCCESS);
	result = engine->SetContextCallbacks(&AngelScriptCallbacks::RequestContextCallback, &AngelScriptCallbacks::ReturnContextCallback, this);	ASSERT(result >= asSUCCESS);

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
	RegisterCoreAPI_Math_AngelScript(engine);
	RegisterCoreAPI_AngelScript(engine);
	RegisterInputAPI_AngelScript(engine);
	RegisterPhysicsAPI_AngelScript(engine);
	RegisterRenderAPI_AngelScript(engine);
	RegisterSoundAPI_AngelScript(engine);

	result = engine->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(AngelScriptCallbacks::PrintCallback), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);

	// ScriptComponent
	ANGELSCRIPT_API_REGISTER_COMPONENT(ScriptComponent);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
}

AngelScriptSystem::~AngelScriptSystem()
{
	for(asIScriptContext* ctx : contextPool)
	{
		ctx->Release();
	}
	contextPool.Clear();

	engine->ShutDownAndRelease();
	engine = nullptr;
}

ScriptComponent* AngelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	asIScriptModule* module = engine->GetModule(scriptName.Str());
	if(module == nullptr)
	{
		module = CompileScript(scriptName, engine);
		if(module == nullptr)
		{
			return nullptr;
		}
	}

	// Get factory function
	asIScriptFunction* factoryFunc = (asIScriptFunction*)module->GetUserData();
	if(factoryFunc == nullptr)
	{
		return nullptr;
	}

	AngelScriptComponent* scriptComponent = &object.AddComponent<AngelScriptComponent>();

	// Call the constructor
	asIScriptContext* ctx = engine->RequestContext();
	ctx->Prepare(factoryFunc);
	ctx->SetArgObject(0, scriptComponent);
	int result = ctx->Execute();
	if(result == asEXECUTION_FINISHED)
	{
		// Get the object that was created and increase the reference, otherwise it would be destroyed when the context is reused or destroyed
		scriptComponent->scriptObject = *(asIScriptObject * *)ctx->GetAddressOfReturnValue();
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

asIScriptModule* AngelScriptSystem::CompileScript(const String& scriptName, asIScriptEngine* engine)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.as", scriptName.Str());
	ReadFilePtr file = FileSystem::Get()->Open(filename);
	if(file == nullptr)
	{
		LOG_ERROR("Could not load script: %s", filename.Str());
		return nullptr;
	}

	MemoryBuffer data = file->ReadFileContent();
	DynamicString scriptSrc;
	scriptSrc.Sprintf(
		"class %s"
		"{"
		"	ScriptComponent @self;"
		"	%s(ScriptComponent @component) { @self = component; }"
		, scriptName.Str()
		, scriptName.Str());
	scriptSrc += data.AsString();
	scriptSrc += "}";

	asIScriptModule* module = engine->GetModule(scriptName.Str(), asGM_ALWAYS_CREATE);
	module->AddScriptSection(scriptName.Str(), scriptSrc.Str(), scriptSrc.Length());
	int result = module->Build();
	if(result != asSUCCESS)
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return module;
	}

	asITypeInfo* typeInfo = engine->GetTypeInfoById(module->GetTypeIdByDecl(scriptName.Str()));

	// Cache factory in the script module
	asIScriptFunction* factoryFunc = typeInfo->GetFactoryByDecl(StackString<512>::Format("%s@ %s(ScriptComponent @)", scriptName.Str(), scriptName.Str()).Str());
	module->SetUserData(factoryFunc);

	// Cache built-in functions in the object type
	const char* builtinFunctionNames[] =
	{
		"Update",
		"Init",
		"Kill",
	};
	DESIRE_CHECK_ARRAY_SIZE(builtinFunctionNames, ScriptComponent::EBuiltinFuncType::Num);

	for(auto i : Enumerator<ScriptComponent::EBuiltinFuncType>())
	{
		asIScriptFunction* func = typeInfo->GetMethodByName(builtinFunctionNames[i]);
		typeInfo->SetUserData(func, i);
	}

	return module;
}

bool AngelScriptSystem::IsBreakpoint(const char* scriptSection, int line, asIScriptFunction* function) const
{
	DESIRE_UNUSED(scriptSection);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(function);

	DESIRE_TODO("Add debugging functionality");
	return false;
}
