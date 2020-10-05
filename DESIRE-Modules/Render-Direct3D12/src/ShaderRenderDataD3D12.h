#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/MemoryBuffer.h"
#include "Engine/Core/HashedStringMap.h"

class ShaderRenderDataD3D12
{
public:
	ID3DBlob* m_pShaderCode = nullptr;

	Array<ID3D12Resource*> m_constantBuffers;

	struct ConstantBufferData
	{
		MemoryBuffer m_data;
		HashedStringMap<std::pair<uint32_t, uint32_t>> m_variableOffsetSizePairs;
	};
	Array<ConstantBufferData> m_constantBuffersData;
};
