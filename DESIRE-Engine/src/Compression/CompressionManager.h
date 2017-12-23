#pragma once

#include "Core/Singleton.h"
#include "Core/HashedStringMap.h"
#include "Compression/ICompression.h"

#include <memory>

class CompressionManager
{
	DESIRE_DECLARE_SINGLETON(CompressionManager)

public:
	std::unique_ptr<ICompression> CreateCompression(const char *name) const;

private:
	typedef ICompression*(*CompressionFactoryFunc_t)();

	template<class T>
	static ICompression* CompressionFactory()
	{
		return new T();
	}

	static const HashedStringMap<CompressionFactoryFunc_t> compressionFactories;
};
