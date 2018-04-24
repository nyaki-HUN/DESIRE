#include "API/AngelScriptAPI.h"

// Generic add operator for String class
template<class T, class U>
String OpAddString(T a, U b)
{
	String rv;
	rv += a;
	rv += b;
	return rv;
}

void RegisterString_AngelScript(asIScriptEngine *engine, asIStringFactory *stringFactory)
{
	int result = 0;

	result = engine->RegisterObjectType("String", sizeof(String), asOBJ_VALUE | asGetTypeTraits<String>());																								ASSERT(result >= asSUCCESS);
	result = engine->RegisterStringFactory("String", stringFactory);																																	ASSERT(result >= asSUCCESS);

	result = engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(AngelScriptAPI<String>::Construct), asCALL_CDECL_OBJFIRST);											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f(const String &in)", asFUNCTION(AngelScriptAPI<String>::ConstructWithArgs<String&>), asCALL_CDECL_OBJFIRST);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectBehaviour("String", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AngelScriptAPI<String>::Destruct), asCALL_CDECL_OBJFIRST);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void ToLower()", asMETHODPR(String, ToLower, (), void), asCALL_THISCALL);																			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void ToUpper()", asMETHODPR(String, ToUpper, (), void), asCALL_THISCALL);																			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void Trim()", asMETHODPR(String, Trim, (), void), asCALL_THISCALL);																				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void Insert(uint64, const String &in)", asMETHODPR(String, Insert, (size_t, const String&), void), asCALL_THISCALL);								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void RemoveFrom(uint64, uint64 numChars = -1)", asMETHODPR(String, RemoveFrom, (size_t, size_t), void), asCALL_THISCALL);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void RemoveFromEnd(uint64)", asMETHODPR(String, RemoveFromEnd, (size_t), void), asCALL_THISCALL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "uint64 Find(const String &in, uint64 start = 0) const", asMETHODPR(String, Find, (const String&, size_t) const, size_t), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "uint64 FindLast(const String &in) const", asMETHODPR(String, FindLast, (const String&) const, size_t), asCALL_THISCALL);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void Replace(const String &in, const String &in)", asMETHODPR(String, Replace, (const String&, const String&), void), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "void ReplaceAll(const String &in, const String &in)", asMETHODPR(String, ReplaceAll, (const String&, const String&), void), asCALL_THISCALL);		ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String SubString(uint64, uint64 numChars = -1) const", asMETHODPR(String, SubString, (size_t, size_t) const, String), asCALL_THISCALL);			ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "uint64 Length() const", asMETHODPR(String, Length, () const, size_t), asCALL_THISCALL);															ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "uint64 LengthUTF8() const", asMETHODPR(String, LengthUTF8, () const, size_t), asCALL_THISCALL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "int IntValue() const", asMETHODPR(String, IntValue, () const, int), asCALL_THISCALL);																ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "float FloatValue() const", asMETHODPR(String, FloatValue, () const, float), asCALL_THISCALL);														ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAssign(const String &in)", asMETHODPR(String, operator =, (const String&), String&), asCALL_THISCALL);									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd(const String &in) const", asMETHODPR(String, operator +, (const String&) const, String), asCALL_THISCALL);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(const String &in)", asMETHODPR(String, operator +=, (const String&), String&), asCALL_THISCALL);								ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(int)", asMETHODPR(String, operator +=, (int32_t), String&), asCALL_THISCALL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(uint)", asMETHODPR(String, operator +=, (uint32_t), String&), asCALL_THISCALL);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(int64)", asMETHODPR(String, operator +=, (int64_t), String&), asCALL_THISCALL);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(uint64)", asMETHODPR(String, operator +=, (uint64_t), String&), asCALL_THISCALL);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String& opAddAssign(float)", asMETHODPR(String, operator +=, (float), String&), asCALL_THISCALL);													ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "int opCmp(const String &in) const", asMETHODPR(String, Compare, (const String&) const, int), asCALL_THISCALL);										ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "int Compare(const String &in) const", asMETHODPR(String, Compare, (const String&) const, int), asCALL_THISCALL);									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "int CompareIgnoreCase(const String &in) const", asMETHODPR(String, CompareIgnoreCase, (const String&) const, int), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "bool Equals(const String &in) const", asMETHODPR(String, Equals, (const String&) const, bool), asCALL_THISCALL);									ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "bool EqualsIgnoreCase(const String &in) const", asMETHODPR(String, EqualsIgnoreCase, (const String&) const, bool), asCALL_THISCALL);				ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "bool StartsWith(const String &in) const", asMETHODPR(String, StartsWith, (const String&) const, bool), asCALL_THISCALL);							ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "bool EndsWith(const String &in) const", asMETHODPR(String, EndsWith, (const String&) const, bool), asCALL_THISCALL);								ASSERT(result >= asSUCCESS);

	// Add operators are not implemented in String class, but we add them to the script system for ease of use
	result = engine->RegisterObjectMethod("String", "String opAdd(int64) const", asFUNCTION((OpAddString<const String&, int64_t>)), asCALL_CDECL_OBJFIRST);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd_r(int64) const", asFUNCTION((OpAddString<int64_t, const String&>)), asCALL_CDECL_OBJLAST);											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd(uint64) const", asFUNCTION((OpAddString<const String&, uint64_t>)), asCALL_CDECL_OBJFIRST);											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd_r(uint64) const", asFUNCTION((OpAddString<uint64_t, const String&>)), asCALL_CDECL_OBJLAST);											ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd(float) const", asFUNCTION((OpAddString<const String&, float>)), asCALL_CDECL_OBJFIRST);												ASSERT(result >= asSUCCESS);
	result = engine->RegisterObjectMethod("String", "String opAdd_r(float) const", asFUNCTION((OpAddString<float, const String&>)), asCALL_CDECL_OBJLAST);												ASSERT(result >= asSUCCESS);
}
