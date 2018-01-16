#include "API/AngelScriptAPI.h"

#include "Engine/Render/Render.h"

void RegisterRenderAPI_AngelScript(asIScriptEngine *engine)
{
	// Render
	engine->RegisterObjectType("IRender", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("IRender Render", Render::Get());
}
