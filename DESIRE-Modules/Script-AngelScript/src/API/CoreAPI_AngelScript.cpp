#include "stdafx_AngelScript.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Application/Application.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/Timer.h"

void RegisterCoreAPI_AngelScript(asIScriptEngine& engine)
{
	int32_t result = asSUCCESS;

	result = engine.RegisterEnum("EComponent");									ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectType("Object", 0, asOBJ_REF | asOBJ_NOCOUNT);	ASSERT(result >= asSUCCESS);

	// Component
	result = engine.RegisterInterface("Component");																							ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Component", "void SetEnabled(bool)", asMETHOD(Component, SetEnabled), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Component", "bool IsEnabled() const", asMETHOD(Component, IsEnabled), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine.RegisterInterfaceMethod("Component", "Object& GetObject() const");														ASSERT(result >= asSUCCESS);

	// Object
	result = engine.RegisterObjectMethod("Object", "string GetObjectName() const", asFUNCTION((AngelScriptGenericAPI<Object>::MakeStringRvFrom<&Object::GetObjectName>)), asCALL_GENERIC);	ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "void SetActive(bool)", asMETHOD(Object, SetActive), asCALL_THISCALL);																	ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "bool IsActiveSelf() const", asMETHOD(Object, IsActiveSelf), asCALL_THISCALL);															ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "bool IsActiveInHierarchy() const", asMETHOD(Object, IsActiveInHierarchy), asCALL_THISCALL);												ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "void SetParent(Object@)", asMETHOD(Object, SetParent), asCALL_THISCALL);																ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "Object@ GetParent() const", asMETHOD(Object, GetParent), asCALL_THISCALL);																ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "void RemoveComponent(Component@)", asMETHOD(Object, RemoveComponent), asCALL_THISCALL);													ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "Component@ GetComponent(EComponent) const", asMETHOD(Object, GetComponentByTypeId), asCALL_THISCALL);									ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("Object", "Transform& GetTransform() const", asMETHOD(Object, GetTransform), asCALL_THISCALL);														ASSERT(result >= asSUCCESS);

	// Timer
	result = engine.RegisterObjectType("ITimer", 0, asOBJ_REF | asOBJ_NOHANDLE);															ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "uint64 GetTimeMicroSec() const", asMETHOD(Timer, GetTimeMicroSec), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "uint GetTimeMilliSec() const", asMETHOD(Timer, GetTimeMilliSec), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "float GetTimeSec() const", asMETHOD(Timer, GetTimeSec), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "uint64 GetMicroDelta() const", asMETHOD(Timer, GetMicroDelta), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "uint GetMilliDelta() const", asMETHOD(Timer, GetMilliDelta), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine.RegisterObjectMethod("ITimer", "float GetSecDelta() const", asMETHOD(Timer, GetSecDelta), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine.RegisterGlobalProperty("ITimer Timer", const_cast<Timer*>(Modules::Application->GetTimer()));							ASSERT(result >= asSUCCESS);
}
