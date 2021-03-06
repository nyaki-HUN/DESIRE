#pragma once

#include "Engine/Compression/Compression.h"

#include "Engine/Core/Container/HashedStringMap.h"
#include "Engine/Core/Factory.h"

class CompressionManager
{
public:
	static std::unique_ptr<Compression> CreateCompression(HashedString name);

private:
	static const HashedStringMap<Factory<Compression>::Func_t> s_compressionFactories;
};
