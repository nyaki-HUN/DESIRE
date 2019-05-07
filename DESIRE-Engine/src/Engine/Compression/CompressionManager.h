#pragma once

#include "Engine/Compression/Compression.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/Factory.h"

#include <memory>

class CompressionManager
{
public:
	static std::unique_ptr<Compression> CreateCompression(HashedString name);

private:
	static const HashedStringMap<Factory<Compression>::Func_t> s_compressionFactories;
};
