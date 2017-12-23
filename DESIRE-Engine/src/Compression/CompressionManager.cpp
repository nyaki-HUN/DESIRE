#include "stdafx.h"
#include "Compression/CompressionManager.h"

CompressionManager::CompressionManager()
{

}

CompressionManager::~CompressionManager()
{

}

std::unique_ptr<ICompression> CompressionManager::CreateCompression(const char *name) const
{
	ICompression *compression = nullptr;

	const CompressionFactoryFunc_t *factoryFunc = compressionFactories.Find(HashedString::CreateFromDynamicString(name));
	if(factoryFunc != nullptr)
	{
		compression = (*factoryFunc)();
	}

	return std::unique_ptr<ICompression>(compression);
}
