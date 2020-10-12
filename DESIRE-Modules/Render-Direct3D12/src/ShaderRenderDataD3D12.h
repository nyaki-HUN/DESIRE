#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

class ShaderRenderDataD3D12
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

	ID3DBlob* m_pShaderCode = nullptr;

	Array<ID3D12Resource*> m_constantBuffers;
	Array<ConstantBufferData> m_constantBuffersData;
};
