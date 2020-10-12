#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

class ShaderRenderDataD3D11
{
public:
	struct ConstantBufferData
	{
		struct Variable
		{
			void* m_pMemory;
			uint32_t m_size;

			inline bool CheckAndUpdate(const void* pValue)
			{
				if(memcmp(m_pMemory, pValue, m_size) == 0)
				{
					return false;
				}

				memcpy(m_pMemory, pValue, m_size);
				return true;
			}
		};

		HashedStringMap<Variable> m_variables;
		MemoryBuffer m_data;
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
