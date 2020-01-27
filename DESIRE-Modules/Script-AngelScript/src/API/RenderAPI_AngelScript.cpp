#include "stdafx_AngelScript.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Render/Render.h"
#include "Engine/Render/RenderComponent.h"

void RegisterRenderAPI_AngelScript(asIScriptEngine* engine)
{
	Render* render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// RenderComponent
	ANGELSCRIPT_API_REGISTER_COMPONENT(RenderComponent);
	result = engine->RegisterObjectMethod("RenderComponent", "void set_layer(int) property", asMETHOD(RenderComponent, SetLayer), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("RenderComponent", "int get_layer() const property", asMETHOD(RenderComponent, GetLayer), asCALL_THISCALL);	ASSERT(result >= asSUCCESS);

	// Render
	result = engine->RegisterObjectType("IRender", 0, asOBJ_REF | asOBJ_NOHANDLE);	ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("IRender Render", render);				ASSERT(result >= asSUCCESS);
}
