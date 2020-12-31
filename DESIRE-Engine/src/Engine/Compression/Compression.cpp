#include "Engine/stdafx.h"
#include "Engine/Compression/Compression.h"

Compression::Compression(int32_t defaultCompressionLevel)
	: m_compressionLevel(defaultCompressionLevel)
{
}

Compression::~Compression()
{
}

void Compression::SetCompressionLevel(int32_t level)
{
	const int32_t minLevel = GetMinCompressionLevel();
	const int32_t maxLevel = GetMaxCompressionLevel();

	m_compressionLevel = std::clamp(level, minLevel, maxLevel);
}

int32_t Compression::GetMinCompressionLevel() const
{
	return 0;
}

int32_t Compression::GetMaxCompressionLevel() const
{
	return 0;
}
