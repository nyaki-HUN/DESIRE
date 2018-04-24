#include "API/AngelScriptAPI.h"

#include "Engine/Core/Modules.h"
#include "Engine/Render/Render.h"

void RegisterRenderAPI_AngelScript(asIScriptEngine *engine)
{
	Render *render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// Render
	result = engine->RegisterObjectType("IRender", 0, asOBJ_REF | asOBJ_NOHANDLE);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("IRender Render", render);								ASSERT(result >= asSUCCESS);
}
