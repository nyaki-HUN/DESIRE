#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/MemoryBuffer.h"
#include "Engine/Core/HashedStringMap.h"

#include <d3d12.h>

class ShaderRenderDataD3D12
{
public:
	ID3DBlob *shaderCode = nullptr;

	Array<ID3D12Resource*> constantBuffers;

	struct ConstantBufferData
	{
		MemoryBuffer data;
		HashedStringMap<std::pair<uint32_t, uint32_t>> variableOffsetSizePairs;
	};
	Array<ConstantBufferData> constantBuffersData;
};
