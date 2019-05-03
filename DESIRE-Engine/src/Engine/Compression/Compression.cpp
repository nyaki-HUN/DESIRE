#include "Engine/stdafx.h"
#include "Engine/Compression/Compression.h"

Compression::Compression()
{

}

Compression::~Compression()
{

}

void Compression::SetCompressionLevel(int level)
{
	const int minLevel = GetMinCompressionLevel();
	const int maxLevel = GetMaxCompressionLevel();

	compressionLevel = std::max(minLevel, std::min(level, maxLevel));
}

int Compression::GetMinCompressionLevel() const
{
	return 0;
}

int Compression::GetMaxCompressionLevel() const
{
	return 0;
}
