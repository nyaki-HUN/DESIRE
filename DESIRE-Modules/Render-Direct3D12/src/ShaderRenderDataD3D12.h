#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

#include "Engine/Render/RenderData.h"

class ShaderRenderDataD3D12 : public RenderData
{
public:
	~ShaderRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pShaderCode);

		for(ID3D12Resource* pResource : m_constantBuffers)
		{
			DX_SAFE_RELEASE(pResource);
		}
		m_constantBuffers.Clear();
	}

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
