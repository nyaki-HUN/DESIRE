#include "API/AngelScriptAPI.h"

#include "Engine/Input/Input.h"

void RegisterInputAPI_AngelScript(asIScriptEngine *engine)
{
	// Input
	engine->RegisterObjectType("IInput", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalProperty("IInput Input", Input::Get());
}
