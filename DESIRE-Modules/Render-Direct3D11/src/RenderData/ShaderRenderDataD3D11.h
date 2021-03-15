#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Container/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

#include "Engine/Render/RenderData.h"

class ShaderRenderDataD3D11 : public RenderData
{
public:
	~ShaderRenderDataD3D11() override
	{
		DX_SAFE_RELEASE(m_pPtr);
		DX_SAFE_RELEASE(m_pShaderCode);

		for(ID3D11Buffer* pBuffer : m_constantBuffers)
		{
			DX_SAFE_RELEASE(pBuffer);
		}
		m_constantBuffers.Clear();
	}

	struct ConstantBufferVariable
	{
		void* pMemory = nullptr;
		uint32_t size = 0;

		inline bool CheckAndUpdate(const void* pValue)
		{
			if(memcmp(pMemory, pValue, size) == 0)
			{
				return false;
			}

			memcpy(pMemory, pValue, size);
			return true;
		}
	};

	struct ConstantBufferData
	{
		HashedStringMap<ConstantBufferVariable> variables;
		MemoryBuffer data;
	};

	union
	{
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11ComputeShader* m_pComputeShader;

		ID3D11DeviceChild* m_pPtr = nullptr;			// Generic base class pointer
	};

	ID3DBlob* m_pShaderCode = nullptr;

	Array<ID3D11Buffer*> m_constantBuffers;
	Array<ConstantBufferData> m_constantBuffersData;
};
