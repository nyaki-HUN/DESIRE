#pragma once

#include "Engine/Core/platform.h"	// for DESIRE_DISABLE_WARNINGS and DESIRE_ENABLE_WARNINGS
#include "Engine/Core/String/String.h"

// --------------------------------------------------------------------------------------------------------------------
//	HashedString is a helper class for creating compile-time hash values from string literals.
//	Use the HashedString::CreateFromDynamicString() function to create hashes run-time.
//	The implementation is using the 64-bit version of MurmurHash2 ( https://code.google.com/p/smhasher/ )
// --------------------------------------------------------------------------------------------------------------------

class HashedString
{
public:
	template<size_t LEN>
	constexpr HashedString(const char(&str)[LEN])
	{
		DESIRE_DISABLE_WARNINGS
#define _h	MurmurHash64<LEN - 1>((LEN - 1) * m, str, 0)
		// h ^= h >> r;
		// h *= m;
		// h ^= h >> r;
		m_hash = ((_h ^ (_h >> r)) * m) ^
				(((_h ^ (_h >> r)) * m) >> r);
#undef _h
		DESIRE_ENABLE_WARNINGS
	}

	explicit constexpr HashedString(uint64_t hash)
		: m_hash(hash)
	{
	}

	inline bool operator ==(HashedString other) const
	{
		return (m_hash == other.m_hash);
	}

	inline operator uint64_t() const
	{
		return m_hash;
	}

	static HashedString CreateFromString(const String& string)
	{
		const size_t len = string.Length();
		uint64_t h = len * m;

		const uint64_t* data = reinterpret_cast<const uint64_t*>(string.Str());
		const uint64_t* end = data + (len / 8);
		while(data != end)
		{
			uint64_t k = *data++;

			k *= m;
			k ^= k >> r;
			k *= m;

			h ^= k;
			h *= m;
		}

		const uint8_t* data2 = reinterpret_cast<const uint8_t*>(data);
		switch(len & 7)
		{
			case 7: h ^= uint64_t(data2[6]) << 48;
			case 6: h ^= uint64_t(data2[5]) << 40;
			case 5: h ^= uint64_t(data2[4]) << 32;
			case 4: h ^= uint64_t(data2[3]) << 24;
			case 3: h ^= uint64_t(data2[2]) << 16;
			case 2: h ^= uint64_t(data2[1]) << 8;
			case 1: h ^= uint64_t(data2[0]);
					h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return HashedString(h);
	}

private:
	static constexpr uint64_t m = 0xc6a4a7935bd1e995LLU;
	static constexpr int r = 47;

	template<size_t LEN>
	static constexpr uint64_t MurmurHash64(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<LEN - 8>(MurmurHash64<8>(h, str, idx), str, idx + 8);
	}

	template<>
	static constexpr uint64_t MurmurHash64<0>(uint64_t h, const char* /*str*/, int /*idx*/)
	{
		return h;
	}

	template<>
	static constexpr uint64_t MurmurHash64<1>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<0>((h ^ (uint64_t(str[idx + 0]))) * m, str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<2>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<1>(h ^ (uint64_t(str[idx + 1]) << 8), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<3>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<2>(h ^ (uint64_t(str[idx + 2]) << 16), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<4>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<3>(h ^ (uint64_t(str[idx + 3]) << 24), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<5>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<4>(h ^ (uint64_t(str[idx + 4]) << 32), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<6>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<5>(h ^ (uint64_t(str[idx + 5]) << 40), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<7>(uint64_t h, const char* str, int idx)
	{
		return MurmurHash64<6>(h ^ (uint64_t(str[idx + 6]) << 48), str, idx);
	}

	template<>
	static constexpr uint64_t MurmurHash64<8>(uint64_t h, const char* str, int idx)
	{
#define k	(uint64_t(str[idx + 0]) | uint64_t(str[idx + 1]) << 8 | uint64_t(str[idx + 2]) << 16 | uint64_t(str[idx + 3]) << 24 | uint64_t(str[idx + 4]) << 32 | uint64_t(str[idx + 5]) << 40 | uint64_t(str[idx + 6]) << 48 | uint64_t(str[idx + 7]) << 56)
		// k *= m;
		// k ^= k >> r;
		// k *= m;

		// h ^= k;
		// h *= m;
		return ((h ^ ((k * m) ^ ((k * m) >> r)) * m) * m);
#undef k
	}

	uint64_t m_hash = 0;
};
