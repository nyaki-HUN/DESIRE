#include "AngelScriptStringFactory.h"

#include "Engine/Core/assert.h"

AngelScriptStringFactory::AngelScriptStringFactory()
{

}

AngelScriptStringFactory::~AngelScriptStringFactory()
{
	// The script engine must release each string constant that it has requested
	ASSERT(stringCache.size() == 0);
}

const void* AngelScriptStringFactory::GetStringConstant(const char *data, asUINT length)
{
	String str(data, length);

	auto it = stringCache.find(str);
	if(it != stringCache.end())
	{
		it->second++;
	}
	else
	{
		it = stringCache.emplace(std::move(str), 1).first;
	}

	return reinterpret_cast<const void*>(&it->first);
}

int AngelScriptStringFactory::ReleaseStringConstant(const void *str)
{
	if(str == nullptr)
	{
		return asERROR;
	}

	auto it = stringCache.find(*reinterpret_cast<const String*>(str));
	if(it == stringCache.end())
	{
		return asERROR;
	}

	it->second--;
	if(it->second == 0)
	{
		stringCache.erase(it);
	}

	return asSUCCESS;
}

int AngelScriptStringFactory::GetRawStringData(const void *str, char *data, asUINT *length) const
{
	if(str == nullptr)
	{
		return asERROR;
	}

	const String& string = *reinterpret_cast<const String*>(str);

	if(data != nullptr)
	{
		memcpy(data, string.c_str(), string.Length());
	}

	if(length != nullptr)
	{
		*length = (asUINT)string.Length();
	}

	return asSUCCESS;
}
