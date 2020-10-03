#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

class ShaderRenderDataD3D11
{
public:
	union
	{
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11ComputeShader* m_pComputeShader;

		ID3D11DeviceChild* m_pPtr = nullptr;			// Generic base class pointer
	};

	ID3DBlob* m_pShaderCode = nullptr;

	Array<ID3D11Buffer*> m_constantBuffers;

	struct ConstantBufferData
	{
		MemoryBuffer m_data;
		HashedStringMap<std::pair<uint32_t, uint32_t>> m_variableOffsetSizePairs;
	};
	Array<ConstantBufferData> m_constantBuffersData;
};
