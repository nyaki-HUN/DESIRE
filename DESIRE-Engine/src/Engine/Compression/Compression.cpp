#include "stdafx.h"
#include "Compression/Compression.h"

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

void Compression::InitStreamForCompression()
{
	ASSERT(false && "Unimplemented");
}

void Compression::InitStreamForDecompression()
{
	ASSERT(false && "Unimplemented");
}

void Compression::ProcessStream()
{
	ASSERT(false && "Unimplemented");
}
