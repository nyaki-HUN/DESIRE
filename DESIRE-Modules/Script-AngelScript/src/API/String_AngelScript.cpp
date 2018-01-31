#include "API/AngelScriptAPI.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/String.h"

#include <unordered_map> 

// The string pool will be kept as user data in the engine (the add-ons have reserved the numbers 1000 through 1999)
constexpr asPWORD STRING_POOL = 1001;

const String& StringFactory(asUINT length, const char *s)
{
	// Each engine instance has its own string pool
	asIScriptContext *ctx = asGetActiveContext();
	if(ctx == nullptr)
	{
		ASSERT(false && "The String factory can only be called from a script");
		return String::emptyString;
	}

	asIScriptEngine *engine = ctx->GetEngine();

	std::unordered_map<const char*, String> *pool = reinterpret_cast<std::unordered_map<const char*, String>*>(engine->GetUserData(STRING_POOL));
	if(pool == nullptr)
	{
		asAcquireExclusiveLock();
		// Make sure the string pool wasn't created while we were waiting for the lock
		pool = reinterpret_cast<std::unordered_map<const char*, String>*>(engine->GetUserData(STRING_POOL));
		if(pool == nullptr)
		{
			pool = new std::unordered_map<const char*, String>;
			engine->SetUserData(pool, STRING_POOL);
		}
		asReleaseExclusiveLock();
	}

	// We can't let other threads modify the pool while we query it
	asAcquireSharedLock();
	// First check if a String object hasn't been created already
	auto it = pool->find(s);
	if(it != pool->end())
	{
		asReleaseSharedLock();
		return it->second;
	}
	asReleaseSharedLock();

	asAcquireExclusiveLock();
	// Make sure the String wasn't created while we were waiting for the exclusive lock
	it = pool->find(s);
	if(it == pool->end())
	{
		it = pool->emplace(s, String(s, length)).first;
	}
	asReleaseExclusiveLock();

	return it->second;
}

// Generic add operator for String class
template<class T, class U>
String OpAddString(T a, U b)
{
	String rv;
	rv += a;
	rv += b;
	return rv;
}

void RegisterString_AngelScript(asIScriptEngine *engine)
{
	engine->RegisterObjectType("String", sizeof(String), asOBJ_VALUE | asGetTypeTraits<String>());

	engine->RegisterStringFactory("const String&", asFUNCTION(StringFactory), asCALL_CDECL);
	engine->SetEngineUserDataCleanupCallback([](asIScriptEngine *engine)
	{
		std::unordered_map<const char*, String> *pool = reinterpret_cast<std::unordered_map<const char*, String>*>(engine->GetUserData(STRING_POOL));
		delete pool;
	}, STRING_POOL);

	engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(AngelScriptAPI<String>::Construct), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f(const String &in)", asFUNCTION(AngelScriptAPI<String>::ConstructWithArgs<String&>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("String", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AngelScriptAPI<String>::Destruct), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "void ToLower()", asMETHODPR(String, ToLower, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void ToUpper()", asMETHODPR(String, ToUpper, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void Trim()", asMETHODPR(String, Trim, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void Insert(uint64, const String &in)", asMETHODPR(String, Insert, (size_t, const String&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void Remove(uint64, uint64)", asMETHODPR(String, Remove, (size_t, size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void RemoveFromEnd(uint64)", asMETHODPR(String, RemoveFromEnd, (size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "uint64 Find(const String &in, uint64 start = 0) const", asMETHODPR(String, Find, (const String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "uint64 FindLast(const String &in) const", asMETHODPR(String, FindLast, (const String&) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void Replace(const String &in, const String &in)", asMETHODPR(String, Replace, (const String&, const String&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void ReplaceAll(const String &in, const String &in)", asMETHODPR(String, ReplaceAll, (const String&, const String&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String SubString(uint64 start, uint64 numChars = -1) const", asMETHODPR(String, SubString, (size_t, size_t) const, String), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "uint64 Length() const", asMETHODPR(String, Length, () const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "uint64 LengthUTF8() const", asMETHODPR(String, LengthUTF8, () const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int IntValue() const", asMETHODPR(String, IntValue, () const, int), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "float FloatValue() const", asMETHODPR(String, FloatValue, () const, float), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAssign(const String &in)", asMETHODPR(String, operator =, (const String&), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String opAdd(const String &in) const", asMETHODPR(String, operator +, (const String&) const, String), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(const String &in)", asMETHODPR(String, operator +=, (const String&), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(int)", asMETHODPR(String, operator +=, (int32_t), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(uint)", asMETHODPR(String, operator +=, (uint32_t), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(int64)", asMETHODPR(String, operator +=, (int64_t), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(uint64)", asMETHODPR(String, operator +=, (uint64_t), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(float)", asMETHODPR(String, operator +=, (float), String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int opCmp(const String &in) const", asMETHODPR(String, Compare, (const String&) const, int), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int Compare(const String &in) const", asMETHODPR(String, Compare, (const String&) const, int), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int CompareIgnoreCase(const String &in) const", asMETHODPR(String, CompareIgnoreCase, (const String&) const, int), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "bool Equals(const String &in) const", asMETHODPR(String, Equals, (const String&) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "bool EqualsIgnoreCase(const String &in) const", asMETHODPR(String, EqualsIgnoreCase, (const String&) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "bool StartsWith(const String &in) const", asMETHODPR(String, StartsWith, (const String&) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "bool EndsWith(const String &in) const", asMETHODPR(String, EndsWith, (const String&) const, bool), asCALL_THISCALL);

	// Add operators are not implementd in String class, but we add them to the script system for ease of use
	engine->RegisterObjectMethod("String", "String opAdd(int64) const", asFUNCTION((OpAddString<const String&, int64_t>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String opAdd_r(int64) const", asFUNCTION((OpAddString<int64_t, const String&>)), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("String", "String opAdd(uint64) const", asFUNCTION((OpAddString<const String&, uint64_t>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String opAdd_r(uint64) const", asFUNCTION((OpAddString<uint64_t, const String&>)), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("String", "String opAdd(float) const", asFUNCTION((OpAddString<const String&, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String opAdd_r(float) const", asFUNCTION((OpAddString<float, const String&>)), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("String", "String opAdd(bool) const", asFUNCTION((OpAddString<const String&, bool>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String opAdd_r(bool) const", asFUNCTION((OpAddString<bool, const String&>)), asCALL_CDECL_OBJLAST);
}
