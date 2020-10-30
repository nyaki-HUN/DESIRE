#include "stdafx_AngelScript.h"
#include "AngelScriptComponent.h"

#include "Engine/Core/String/String.h"

extern asIStringFactory* GetStdStringFactorySingleton();

AngelScriptComponent::AngelScriptComponent(Object& object, asIScriptFunction& factoryFunc)
	: ScriptComponent(object)
{
	// Call the constructor
	asIScriptContext* pContext = factoryFunc.GetEngine()->RequestContext();
	pContext->Prepare(&factoryFunc);
	pContext->SetArgObject(0, this);
	int result = pContext->Execute();
	if(result == asEXECUTION_FINISHED)
	{
		// Get the object that was created and increase the reference, otherwise it would be destroyed when the context is reused or destroyed
		m_pScriptObject = *static_cast<asIScriptObject**>(pContext->GetAddressOfReturnValue());
		m_pScriptObject->AddRef();
	}

	factoryFunc.GetEngine()->ReturnContext(pContext);
}

AngelScriptComponent::~AngelScriptComponent()
{
	if(m_pScriptObject)
	{
		m_pScriptObject->Release();
		m_pScriptObject = nullptr;
	}
}

bool AngelScriptComponent::IsValid() const
{
	return (m_pScriptObject != nullptr);
}

void AngelScriptComponent::CallByType(EBuiltinFuncType funcType)
{
	if(!IsValid())
	{
		return;
	}

	const asITypeInfo* pTypeInfo = m_pScriptObject->GetObjectType();
	asIScriptFunction* pFunction = static_cast<asIScriptFunction*>(pTypeInfo->GetUserData(static_cast<asPWORD>(funcType)));
	if(pFunction == nullptr)
	{
		return;
	}

	asIScriptContext* pContext = m_pScriptObject->GetEngine()->RequestContext();
	int result = pContext->Prepare(pFunction);
	if(result == asSUCCESS)
	{
		pContext->SetObject(m_pScriptObject);
		result = pContext->Execute();
		ASSERT(result == asEXECUTION_FINISHED);
	}
	pContext->GetEngine()->ReturnContext(pContext);
}

void AngelScriptComponent::CallFromScript(asIScriptGeneric* pGeneric)
{
	AngelScriptComponent* pScriptComponent = static_cast<AngelScriptComponent*>(pGeneric->GetObject());

	const String* pFunctionName = static_cast<const String*>(pGeneric->GetArgObject(0));
	if(pFunctionName == nullptr)
	{
		return;
	}

	if(pScriptComponent->PrepareFunctionCall(*pFunctionName))
	{
		// Set the args
		for(asUINT i = 1; i < static_cast<asUINT>(pGeneric->GetArgCount()); ++i)
		{
			int result = asERROR;
			void* pArg = pGeneric->GetArgAddress(i);
			const int typeId = pGeneric->GetArgTypeId(i);
			if(typeId & asTYPEID_MASK_OBJECT)
			{
				result = pScriptComponent->m_pFunctionCallCtx->SetArgAddress(pScriptComponent->m_numFunctionCallArgs, *static_cast<void**>(pArg));
			}
			else
			{
				switch(typeId)
				{
					case asTYPEID_BOOL:
					case asTYPEID_INT8:
					case asTYPEID_UINT8:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgByte(pScriptComponent->m_numFunctionCallArgs, *static_cast<asBYTE*>(pArg));
						break;

					case asTYPEID_INT16:
					case asTYPEID_UINT16:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgWord(pScriptComponent->m_numFunctionCallArgs, *static_cast<asWORD*>(pArg));
						break;

					case asTYPEID_INT32:
					case asTYPEID_UINT32:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgDWord(pScriptComponent->m_numFunctionCallArgs, *static_cast<asDWORD*>(pArg));
						break;

					case asTYPEID_INT64:
					case asTYPEID_UINT64:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgQWord(pScriptComponent->m_numFunctionCallArgs, *static_cast<asQWORD*>(pArg));
						break;

					case asTYPEID_FLOAT:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgFloat(pScriptComponent->m_numFunctionCallArgs, *static_cast<float*>(pArg));
						break;

					case asTYPEID_DOUBLE:
						result = pScriptComponent->m_pFunctionCallCtx->SetArgDouble(pScriptComponent->m_numFunctionCallArgs, *static_cast<double*>(pArg));
						break;

					default:
						ASSERT(false && "Not supported");
						break;
				}
			}

			if(result != asSUCCESS)
			{
				LOG_ERROR("Failed to set arg%u for function: %s", pScriptComponent->m_numFunctionCallArgs, pFunctionName->Str());
			}

			pScriptComponent->m_numFunctionCallArgs++;
		}

		pScriptComponent->ExecuteFunctionCall();
	}
}

bool AngelScriptComponent::PrepareFunctionCall(const String& functionName)
{
	if(!IsValid())
	{
		return false;
	}

	const asITypeInfo* pTypeInfo = m_pScriptObject->GetObjectType();
	asIScriptFunction* pFunction = pTypeInfo->GetMethodByName(functionName.Str());
	if(pFunction == nullptr)
	{
		return false;
	}

	ASSERT(m_pFunctionCallCtx == nullptr);
	m_pFunctionCallCtx = m_pScriptObject->GetEngine()->RequestContext();
	if(m_pFunctionCallCtx == nullptr)
	{
		return false;
	}

	int result = m_pFunctionCallCtx->Prepare(pFunction);
	if(result != asSUCCESS)
	{
		m_pFunctionCallCtx->GetEngine()->ReturnContext(m_pFunctionCallCtx);
		m_pFunctionCallCtx = nullptr;
		return false;
	}

	m_pFunctionCallCtx->SetObject(m_pScriptObject);		// the 'this' parameter
	m_numFunctionCallArgs = 0;
	return true;
}

void AngelScriptComponent::ExecuteFunctionCall()
{
	ASSERT(m_pFunctionCallCtx != nullptr);

	const int result = m_pFunctionCallCtx->Execute();
	m_pFunctionCallCtx->GetEngine()->ReturnContext(m_pFunctionCallCtx);
	m_pFunctionCallCtx = nullptr;

	asIStringFactory* pStringFactory = GetStdStringFactorySingleton();
	for(const void* pStringArg : m_functionCallStringArgs)
	{
		pStringFactory->ReleaseStringConstant(pStringArg);
	}
	m_functionCallStringArgs.Clear();

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
	int result = m_pFunctionCallCtx->SetArgDWord(m_numFunctionCallArgs++, (asDWORD)arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(float arg)
{
	int result = m_pFunctionCallCtx->SetArgFloat(m_numFunctionCallArgs++, arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(double arg)
{
	int result = m_pFunctionCallCtx->SetArgDouble(m_numFunctionCallArgs++, arg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(bool arg)
{
	int result = m_pFunctionCallCtx->SetArgByte(m_numFunctionCallArgs++, (arg ? 1u : 0));
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(void* pArg)
{
	ASSERT(pArg != nullptr);

	int result = m_pFunctionCallCtx->SetArgAddress(m_numFunctionCallArgs++, pArg);
	return (result == asSUCCESS);
}

bool AngelScriptComponent::AddFunctionCallArg(const String& arg)
{
	const void* pStringArg = GetStdStringFactorySingleton()->GetStringConstant(arg.Str(), static_cast<asUINT>(arg.Length()));
	m_functionCallStringArgs.Add(pStringArg);
	int result = m_pFunctionCallCtx->SetArgObject(m_numFunctionCallArgs++, const_cast<void*>(pStringArg));
	return (result == asSUCCESS);
}
