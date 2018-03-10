#include "Engine/stdafx.h"
#include "Engine/Compression/CompressionManager.h"

std::unique_ptr<Compression> CompressionManager::CreateCompression(HashedString name)
{
	const Factory<Compression>::Func_t *factoryFunc = compressionFactories.Find(name);
	if(factoryFunc != nullptr)
	{
		return (*factoryFunc)();
	}

	return nullptr;
}
