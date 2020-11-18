#include "Engine/stdafx.h"
#include "Engine/Core/String/HashedString.h"

#include "Engine/Core/String/String.h"

#define XXH_INLINE_ALL
#include "xxHash/xxhash.h"

HashedString::HashedString(const String& string)
	: HashedString(string.Str(), string.Length())
{
}

HashedString::HashedString(const void* pData, size_t size)
	: m_hash(XXH3_64bits(pData, size))
{
}
