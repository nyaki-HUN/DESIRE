#include "stdafx_AngelScript.h"
#include "API/AngelScriptAPI.h"

#include "Engine/Input/Input.h"

void RegisterInputAPI_AngelScript(asIScriptEngine& engine)
{
	if(Modules::Input == nullptr)
	{
		return;
	}

	int result = asSUCCESS;

	// Input
	result = engine.RegisterObjectType("IInput", 0, asOBJ_REF | asOBJ_NOHANDLE);	ASSERT(result >= asSUCCESS);
	result = engine.RegisterGlobalProperty("IInput Input", Modules::Input.get());	ASSERT(result >= asSUCCESS);
}
