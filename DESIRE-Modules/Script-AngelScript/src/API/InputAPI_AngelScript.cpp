#include "API/AngelScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Input/Input.h"

void RegisterInputAPI_AngelScript(asIScriptEngine *engine)
{
	Input *input = Modules::Input.get();
	if(input == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// Input
	result = engine->RegisterObjectType("IInput", 0, asOBJ_REF | asOBJ_NOHANDLE);					ASSERT(result >= asSUCCESS);
	result = engine->RegisterGlobalProperty("IInput Input", input);									ASSERT(result >= asSUCCESS);
}
