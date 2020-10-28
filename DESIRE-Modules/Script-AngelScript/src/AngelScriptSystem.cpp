#include "stdafx_AngelScript.h"
#include "AngelScriptSystem.h"

#include "AngelScriptComponent.h"
#include "AngelScriptCallbacks.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/String/DynamicString.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Utils/Enumerator.h"

AngelScriptSystem::AngelScriptSystem()
{
	asSetGlobalMemoryFunctions(&AngelScriptCallbacks::MallocWrapper, &AngelScriptCallbacks::FreeWrapper);

	m_pEngine = asCreateScriptEngine();
	int result = m_pEngine->SetMessageCallback(asFUNCTION(AngelScriptCallbacks::MessageCallback), this, asCALL_CDECL);							ASSERT(result >= asSUCCESS);
	result = m_pEngine->SetContextCallbacks(&AngelScriptCallbacks::RequestContextCallback, &AngelScriptCallbacks::ReturnContextCallback, this);	ASSERT(result >= asSUCCESS);

	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_REQUIRE_ENUM_SCOPE, false);														ASSERT(result >= asSUCCESS);
	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_GLOBAL_VARS, true);														ASSERT(result >= asSUCCESS);
	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, true);										ASSERT(result >= asSUCCESS);
	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_DISALLOW_EMPTY_LIST_ELEMENTS, true);												ASSERT(result >= asSUCCESS);

#if DESIRE_PUBLIC_BUILD
	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_BUILD_WITHOUT_LINE_CUES, true);													ASSERT(result >= asSUCCESS);
#else
	result = m_pEngine->SetEngineProperty(asEEngineProp::asEP_BUILD_WITHOUT_LINE_CUES, false);													ASSERT(result >= asSUCCESS);
#endif

	asIScriptEngine& engine = *m_pEngine;

	// Register Script API
	RegisterStdString(engine);
	RegisterCoreAPI_Math_AngelScript(engine);
	RegisterCoreAPI_AngelScript(engine);
	RegisterInputAPI_AngelScript(engine);
	RegisterPhysicsAPI_AngelScript(engine);
	RegisterRenderAPI_AngelScript(engine);
	RegisterSoundAPI_AngelScript(engine);

	result = engine.RegisterGlobalFunction("void print(const string& in)", asFUNCTION(AngelScriptCallbacks::PrintCallback), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);

	// ScriptComponent
	ANGELSCRIPT_API_REGISTER_COMPONENT(ScriptComponent);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);									ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);								ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);						ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);				ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);		ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ScriptComponent", "void Call(const string& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(AngelScriptComponent::CallFromScript), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
}

AngelScriptSystem::~AngelScriptSystem()
{
	for(asIScriptContext* pContext : m_contextPool)
	{
		pContext->Release();
	}
	m_contextPool.Clear();

	m_pEngine->ShutDownAndRelease();
	m_pEngine = nullptr;
}

ScriptComponent* AngelScriptSystem::CreateScriptComponentOnObject_Internal(Object& object, const String& scriptName)
{
	asIScriptModule* pModule = m_pEngine->GetModule(scriptName.Str());
	if(pModule == nullptr)
	{
		pModule = CompileScript(scriptName, *m_pEngine);
		if(pModule == nullptr)
		{
			return nullptr;
		}
	}

	// Get factory function
	asIScriptFunction* pFactoryFunc = static_cast<asIScriptFunction*>(pModule->GetUserData());
	if(pFactoryFunc == nullptr)
	{
		return nullptr;
	}

	AngelScriptComponent* pScriptComponent = &object.AddComponent<AngelScriptComponent>();

	// Call the constructor
	asIScriptContext* pContext = m_pEngine->RequestContext();
	pContext->Prepare(pFactoryFunc);
	pContext->SetArgObject(0, pScriptComponent);
	int result = pContext->Execute();
	if(result == asEXECUTION_FINISHED)
	{
		// Get the object that was created and increase the reference, otherwise it would be destroyed when the context is reused or destroyed
		pScriptComponent->m_pScriptObject = *static_cast<asIScriptObject**>(pContext->GetAddressOfReturnValue());
		pScriptComponent->m_pScriptObject->AddRef();
	}
	else
	{
		object.RemoveComponent(pScriptComponent);
		pScriptComponent = nullptr;
	}

	m_pEngine->ReturnContext(pContext);

	return pScriptComponent;
}

asIScriptModule* AngelScriptSystem::CompileScript(const String& scriptName, asIScriptEngine& engine)
{
	const StackString<DESIRE_MAX_PATH_LEN> filename = StackString<DESIRE_MAX_PATH_LEN>::Format("data/scripts/%s.as", scriptName.Str());
	DynamicString data = FileSystem::Get()->LoadTextFile(filename);
	DynamicString scriptSrc;
	scriptSrc.Sprintf(
		"class %s"
		"{"
		"	ScriptComponent @self;"
		"	%s(ScriptComponent @component) { @self = component; }"
		, scriptName.Str()
		, scriptName.Str());
	scriptSrc += data;
	scriptSrc += "}";

	asIScriptModule* pModule = engine.GetModule(scriptName.Str(), asGM_ALWAYS_CREATE);
	pModule->AddScriptSection(scriptName.Str(), scriptSrc.Str(), scriptSrc.Length());
	int result = pModule->Build();
	if(result != asSUCCESS)
	{
		LOG_ERROR("Could not compile script: %s", filename.Str());
		return pModule;
	}

	asITypeInfo* pTypeInfo = engine.GetTypeInfoById(pModule->GetTypeIdByDecl(scriptName.Str()));

	// Cache factory in the script module
	asIScriptFunction* factoryFunc = pTypeInfo->GetFactoryByDecl(StackString<512>::Format("%s@ %s(ScriptComponent @)", scriptName.Str(), scriptName.Str()).Str());
	pModule->SetUserData(factoryFunc);

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
		asIScriptFunction* pFunc = pTypeInfo->GetMethodByName(builtinFunctionNames[i]);
		pTypeInfo->SetUserData(pFunc, i);
	}

	return pModule;
}

bool AngelScriptSystem::IsBreakpoint(const char* pScriptSection, int line, asIScriptFunction* pFunction) const
{
	DESIRE_UNUSED(pScriptSection);
	DESIRE_UNUSED(line);
	DESIRE_UNUSED(pFunction);

	DESIRE_TODO("Add debugging functionality");
	return false;
}
