#pragma once

#include "Engine/Core/Singleton.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/Factory.h"
#include "Engine/Compression/Compression.h"

#include <memory>

class CompressionManager
{
	DESIRE_SINGLETON(CompressionManager)

public:
	std::unique_ptr<Compression> CreateCompression(HashedString name) const;

private:
	static const HashedStringMap<Factory<Compression>::Func_t> compressionFactories;
};
