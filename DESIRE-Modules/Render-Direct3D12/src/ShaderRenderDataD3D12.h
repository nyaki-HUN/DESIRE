#pragma once

#include "Engine/Core/Container/Array.h"
#include "Engine/Core/Container/HashedStringMap.h"
#include "Engine/Core/MemoryBuffer.h"

#include "Engine/Render/RenderData.h"

class ShaderRenderDataD3D12 : public RenderData
{
public:
	~ShaderRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pShaderCode);
	}

	struct ConstantBufferVariable
	{
		void* pMemory;
		uint32_t size;

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

	ID3DBlob* m_pShaderCode = nullptr;

	Array<ConstantBufferData> m_constantBuffersData;
};
