#pragma once

#include "Engine/Render/RenderData.h"

class TextureRenderDataD3D12 : public RenderData
{
public:
	~TextureRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pTexture);
		DX_SAFE_RELEASE(m_pHeapForSRV);
	}

	ID3D12Resource* m_pTexture = nullptr;
	ID3D12DescriptorHeap* m_pHeapForSRV = nullptr;
};
