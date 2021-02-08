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

	bool operator <(HashedString other) const	{ return (m_hash < other.m_hash); }
	bool operator ==(HashedString other) const	{ return (m_hash == other.m_hash); }

private:
	HashedString(const void* pData, size_t size);

	uint64_t m_hash = 0;
};
