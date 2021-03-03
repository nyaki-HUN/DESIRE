#include "Engine/stdafx.h"
#include "Engine/Compression/CompressionManager.h"

std::unique_ptr<Compression> CompressionManager::CreateCompression(HashedString name)
{
	const Factory<Compression>::Func_t* pFactoryFunc = s_compressionFactories.Find(name);
	if(pFactoryFunc)
	{
		return (*pFactoryFunc)();
	}

	return nullptr;
}
