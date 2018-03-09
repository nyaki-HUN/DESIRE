#include "Engine/stdafx.h"
#include "Engine/Compression/CompressionManager.h"

CompressionManager::CompressionManager()
{

}

CompressionManager::~CompressionManager()
{

}

std::unique_ptr<Compression> CompressionManager::CreateCompression(HashedString name) const
{
	const Factory<Compression>::Func_t *factoryFunc = compressionFactories.Find(name);
	if(factoryFunc != nullptr)
	{
		return (*factoryFunc)();
	}

	return nullptr;
}
