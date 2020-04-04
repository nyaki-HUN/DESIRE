#include "stdafx_AngelScript.h"
#include "AngelScriptCallbacks.h"

#include "AngelScriptSystem.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* AngelScriptCallbacks::MallocWrapper(size_t size)
{
	return MemorySystem::Alloc(size);
}

void AngelScriptCallbacks::FreeWrapper(void* ptr)
{
	MemorySystem::Free(ptr);
}

void AngelScriptCallbacks::PrintCallback(asIScriptGeneric* gen)
{
	const std::string* message = static_cast<const std::string*>(gen->GetArgObject(0));
	LOG_DEBUG("%s", message->c_str());
}

void AngelScriptCallbacks::MessageCallback(const asSMessageInfo* msg, void* userData)
{
	DESIRE_UNUSED(userData);

	switch(msg->type)
	{
		case asMSGTYPE_ERROR:
			LOG_ERROR("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
		case asMSGTYPE_WARNING:
			LOG_WARNING("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
		case asMSGTYPE_INFORMATION:
			LOG_MESSAGE("%s(%d, %d): %s", msg->section, msg->row, msg->col, msg->message);
			break;
	}
}

asIScriptContext* AngelScriptCallbacks::RequestContextCallback(asIScriptEngine* engine, void* userData)
{
	DESIRE_UNUSED(userData);

	AngelScriptSystem* scriptSystem = static_cast<AngelScriptSystem*>(userData);

	asIScriptContext* ctx = nullptr;
	if(scriptSystem->contextPool.IsEmpty())
	{
		ctx = engine->CreateContext();
		int result = ctx->SetExceptionCallback(asFUNCTION(&AngelScriptCallbacks::ExceptionCallback), scriptSystem, asCALL_CDECL);
		ASSERT(result == asSUCCESS);
		result = ctx->SetLineCallback(asFUNCTION(AngelScriptCallbacks::LineCallback), scriptSystem, asCALL_CDECL);
		ASSERT(result == asSUCCESS);
	}
	else
	{
		ctx = scriptSystem->contextPool.GetLast();
		scriptSystem->contextPool.RemoveLast();
	}

	return ctx;
}

void AngelScriptCallbacks::ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* userData)
{
	DESIRE_UNUSED(engine);

	ctx->Unprepare();

	AngelScriptSystem* scriptSystem = static_cast<AngelScriptSystem*>(userData);
	scriptSystem->contextPool.Add(ctx);
}

void AngelScriptCallbacks::ExceptionCallback(asIScriptContext* ctx, void* userData)
{
	DESIRE_UNUSED(userData);

	LOG_ERROR("Exception: %s", ctx->GetExceptionString());
	const asIScriptFunction* function = ctx->GetExceptionFunction();
	LOG_ERROR("module: %s", function->GetModuleName());
	LOG_ERROR("func: %s", function->GetDeclaration());
//	LOG_ERROR("section: %s", function->GetScriptSectionName());
	LOG_ERROR("line: %d", ctx->GetExceptionLineNumber());
}

void AngelScriptCallbacks::LineCallback(asIScriptContext* ctx, void* userData)
{
	AngelScriptSystem* scriptSystem = static_cast<AngelScriptSystem*>(userData);

	// Determine if we have reached a break point
	int column;
	const char* scriptSection;
	int line = ctx->GetLineNumber(0, &column, &scriptSection);
	asIScriptFunction* function = ctx->GetFunction();
	if(scriptSystem->IsBreakpoint(scriptSection, line, function))
	{
		// A breakpoint has been reached so the execution of the script should be suspended
		ctx->Suspend();

		// Show the call stack
		for(asUINT i = 0; i < ctx->GetCallstackSize(); ++i)
		{
			asIScriptFunction* func = ctx->GetFunction(i);
			line = ctx->GetLineNumber(i, &column, &scriptSection);
			LOG_MESSAGE("%s:%s:%d,%d\n", scriptSection, func->GetDeclaration(), line, column);
		}
	}
}
