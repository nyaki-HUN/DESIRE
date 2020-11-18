#pragma once

class String;

class HashedString
{
public:
	HashedString(const String& string);

	template<size_t SIZE>
	HashedString(const char(&str)[SIZE])
		: HashedString(str, SIZE - 1)
	{
	}

	explicit HashedString(uint64_t hash)
		: m_hash(hash)
	{
	}

	inline operator uint64_t() const
	{
		return m_hash;
	}

private:
	HashedString(const void* pData, size_t size);

	uint64_t m_hash = 0;
};
