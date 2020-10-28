#include "stdafx_AngelScript.h"
#include "AngelScriptCallbacks.h"

#include "AngelScriptSystem.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* AngelScriptCallbacks::MallocWrapper(size_t size)
{
	return MemorySystem::Alloc(size);
}

void AngelScriptCallbacks::FreeWrapper(void* pMemory)
{
	MemorySystem::Free(pMemory);
}

void AngelScriptCallbacks::PrintCallback(asIScriptGeneric* pGeneric)
{
	const std::string* pMessage = static_cast<const std::string*>(pGeneric->GetArgObject(0));
	LOG_DEBUG("%s", pMessage->c_str());
}

void AngelScriptCallbacks::MessageCallback(const asSMessageInfo* pMessage, void* pUserData)
{
	DESIRE_UNUSED(pUserData);

	switch(pMessage->type)
	{
		case asMSGTYPE_ERROR:
			LOG_ERROR("%s(%d, %d): %s", pMessage->section, pMessage->row, pMessage->col, pMessage->message);
			break;
		case asMSGTYPE_WARNING:
			LOG_WARNING("%s(%d, %d): %s", pMessage->section, pMessage->row, pMessage->col, pMessage->message);
			break;
		case asMSGTYPE_INFORMATION:
			LOG_MESSAGE("%s(%d, %d): %s", pMessage->section, pMessage->row, pMessage->col, pMessage->message);
			break;
	}
}

asIScriptContext* AngelScriptCallbacks::RequestContextCallback(asIScriptEngine* pEngine, void* pUserData)
{
	DESIRE_UNUSED(pUserData);

	AngelScriptSystem* pScriptSystem = static_cast<AngelScriptSystem*>(pUserData);

	asIScriptContext* pContext = nullptr;
	if(pScriptSystem->m_contextPool.IsEmpty())
	{
		pContext = pEngine->CreateContext();
		int result = pContext->SetExceptionCallback(asFUNCTION(&AngelScriptCallbacks::ExceptionCallback), pScriptSystem, asCALL_CDECL);
		ASSERT(result == asSUCCESS);
		result = pContext->SetLineCallback(asFUNCTION(AngelScriptCallbacks::LineCallback), pScriptSystem, asCALL_CDECL);
		ASSERT(result == asSUCCESS);
	}
	else
	{
		pContext = pScriptSystem->m_contextPool.GetLast();
		pScriptSystem->m_contextPool.RemoveLast();
	}

	return pContext;
}

void AngelScriptCallbacks::ReturnContextCallback(asIScriptEngine* pEngine, asIScriptContext* pContext, void* pUserData)
{
	DESIRE_UNUSED(pEngine);

	pContext->Unprepare();

	AngelScriptSystem* pScriptSystem = static_cast<AngelScriptSystem*>(pUserData);
	pScriptSystem->m_contextPool.Add(pContext);
}

void AngelScriptCallbacks::ExceptionCallback(asIScriptContext* pContext, void* pUserData)
{
	DESIRE_UNUSED(pUserData);

	LOG_ERROR("Exception: %s", pContext->GetExceptionString());
	const asIScriptFunction* function = pContext->GetExceptionFunction();
	LOG_ERROR("module: %s", function->GetModuleName());
	LOG_ERROR("func: %s", function->GetDeclaration());
//	LOG_ERROR("section: %s", function->GetScriptSectionName());
	LOG_ERROR("line: %d", pContext->GetExceptionLineNumber());
}

void AngelScriptCallbacks::LineCallback(asIScriptContext* pContext, void* pUserData)
{
	AngelScriptSystem* pScriptSystem = static_cast<AngelScriptSystem*>(pUserData);

	// Determine if we have reached a break point
	int column;
	const char* pScriptSection = nullptr;
	int line = pContext->GetLineNumber(0, &column, &pScriptSection);
	asIScriptFunction* pFunction = pContext->GetFunction();
	if(pScriptSystem->IsBreakpoint(pScriptSection, line, pFunction))
	{
		// A breakpoint has been reached so the execution of the script should be suspended
		pContext->Suspend();

		// Show the call stack
		for(asUINT i = 0; i < pContext->GetCallstackSize(); ++i)
		{
			asIScriptFunction* func = pContext->GetFunction(i);
			line = pContext->GetLineNumber(i, &column, &pScriptSection);
			LOG_MESSAGE("%s:%s:%d,%d\n", pScriptSection, func->GetDeclaration(), line, column);
		}
	}
}
