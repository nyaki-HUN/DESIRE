#pragma once

#include "Core/Singleton.h"
#include "Core/HashedStringMap.h"
#include "Compression/Compression.h"

#include <memory>

class CompressionManager
{
	DESIRE_SINGLETON(CompressionManager)

public:
	std::unique_ptr<Compression> CreateCompression(HashedString name) const;

private:
	typedef Compression*(*CompressionFactoryFunc_t)();

	template<class T>
	static Compression* CompressionFactory()
	{
		return new T();
	}

	static const HashedStringMap<CompressionFactoryFunc_t> compressionFactories;
};
