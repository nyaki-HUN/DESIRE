#include "stdafx.h"
#include "Script/AngelScript/AngelScriptComponent.h"
#include "Core/String.h"

AngelScriptComponent::AngelScriptComponent()
	: scriptObject(nullptr)
	, functionCallCtx(nullptr)
	, numFunctionCallArgs(0)
{

}

AngelScriptComponent::~AngelScriptComponent()
{

}

void AngelScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	const asITypeInfo *typeInfo = scriptObject->GetObjectType();
	asIScriptFunction *func = static_cast<asIScriptFunction*>(typeInfo->GetUserData((asPWORD)funcType));
	if(func == nullptr)
	{
		return;
	}

	asIScriptContext *ctx = scriptObject->GetEngine()->RequestContext();
	int result = ctx->Prepare(func);
	if(result == asSUCCESS)
	{
		ctx->SetObject(scriptObject);
		result = ctx->Execute();
		ASSERT(result == asEXECUTION_FINISHED);
	}
	ctx->GetEngine()->ReturnContext(ctx);
}

void AngelScriptComponent::CallFromScript(asIScriptGeneric *gen)
{
	AngelScriptComponent *scriptComp = static_cast<AngelScriptComponent*>(gen->GetObject());

	const String *functionName = static_cast<const String*>(gen->GetArgObject(0));
	if(functionName == nullptr)
	{
		return;
	}

	if(scriptComp->PrepareFunctionCall(functionName->c_str()))
	{
		// Set the args
		for(asUINT i = 1; i < (asUINT)gen->GetArgCount(); ++i)
		{
			int result = asERROR;
			void *argPtr = gen->GetArgAddress(i);
			const int typeId = gen->GetArgTypeId(i);
			if(typeId & asTYPEID_MASK_OBJECT)
			{
				result = scriptComp->functionCallCtx->SetArgAddress(scriptComp->numFunctionCallArgs, *(void**)argPtr);
			}
			else
			{
				switch(typeId)
				{
					case asTYPEID_BOOL:
					case asTYPEID_INT8:
					case asTYPEID_UINT8:
						result = scriptComp->functionCallCtx->SetArgByte(scriptComp->numFunctionCallArgs, *(asBYTE*)argPtr);
						break;

					case asTYPEID_INT16:
					case asTYPEID_UINT16:
						result = scriptComp->functionCallCtx->SetArgWord(scriptComp->numFunctionCallArgs, *(asWORD*)argPtr);
						break;

					case asTYPEID_INT32:
					case asTYPEID_UINT32:
						result = scriptComp->functionCallCtx->SetArgDWord(scriptComp->numFunctionCallArgs, *(asDWORD*)argPtr);
						break;

					case asTYPEID_INT64:
					case asTYPEID_UINT64:
						result = scriptComp->functionCallCtx->SetArgQWord(scriptComp->numFunctionCallArgs, *(asQWORD*)argPtr);
						break;

					case asTYPEID_FLOAT:
						result = scriptComp->functionCallCtx->SetArgFloat(scriptComp->numFunctionCallArgs, *(float*)argPtr);
						break;

					case asTYPEID_DOUBLE:
						result = scriptComp->functionCallCtx->SetArgDouble(scriptComp->numFunctionCallArgs, *(double*)argPtr);
						break;

					default:
						ASSERT(false && "Not supported");
						break;
				}
			}

			if(result != asSUCCESS)
			{
				LOG_ERROR("Failed to set arg%u for function: %s", scriptComp->numFunctionCallArgs, functionName->c_str());
			}

			scriptComp->numFunctionCallArgs++;
		}

		scriptComp->ExecuteFunctionCall();
	}
}

bool AngelScriptComponent::PrepareFunctionCall(const char *functionName)
{
	ASSERT(functionName != nullptr);

	const asITypeInfo *typeInfo = scriptObject->GetObjectType();
	asIScriptFunction *func = typeInfo->GetMethodByName(functionName);
	if(func == nullptr)
	{
		return false;
	}

	ASSERT(functionCallCtx == nullptr);
	functionCallCtx = scriptObject->GetEngine()->RequestContext();
	if(functionCallCtx == nullptr)
	{
		return false;
	}

	int result = functionCallCtx->Prepare(func);
	if(result != asSUCCESS)
	{
		functionCallCtx->GetEngine()->ReturnContext(functionCallCtx);
		functionCallCtx = nullptr;
		return false;
	}

	functionCallCtx->SetObject(scriptObject);		// the 'this' parameter
	numFunctionCallArgs = 0;
	return true;
}

void AngelScriptComponent::ExecuteFunctionCall()
{
	ASSERT(functionCallCtx != nullptr);
	const int result = functionCallCtx->Execute();
	functionCallCtx->GetEngine()->ReturnContext(functionCallCtx);
	functionCallCtx = nullptr;

	switch(result)
	{
		case asEXECUTION_FINISHED:		break;
		case asEXECUTION_SUSPENDED:		LOG_ERROR("Execution suspended"); break;
		case asEXECUTION_ABORTED:		LOG_ERROR("Execution aborted"); break;
		case asEXECUTION_EXCEPTION:		break;	// This is handled in AngelScriptSystem::ExceptionCallback()
		case asERROR:					LOG_ERROR("Execution error"); break;
	}
}

bool AngelScriptComponent::AddFunctionCallArg(int arg)
{
	int result = functionCallCtx->SetArgDWord(numFunctionCallArgs++, (asDWORD)arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(float arg)
{
	int result = functionCallCtx->SetArgFloat(numFunctionCallArgs++, arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(double arg)
{
	int result = functionCallCtx->SetArgDouble(numFunctionCallArgs++, arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(bool arg)
{
	int result = functionCallCtx->SetArgByte(numFunctionCallArgs++, (arg ? 1u : 0));
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(void *arg)
{
	ASSERT(arg != nullptr);
	int result = functionCallCtx->SetArgAddress(numFunctionCallArgs++, arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(const String& arg)
{
	int result = functionCallCtx->SetArgObject(numFunctionCallArgs++, (void*)&arg);
	return (result == asSUCCESS);
}
