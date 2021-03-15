#pragma once

#include "Engine/Render/RenderData.h"

class TextureRenderDataD3D11 : public RenderData
{
public:
	~TextureRenderDataD3D11() override
	{
		DX_SAFE_RELEASE(m_pTexture);
		DX_SAFE_RELEASE(m_pSRV);
	}

	union
	{
		ID3D11Texture2D* m_pTexture2D;
		ID3D11Texture3D* m_pTexture3D;

		ID3D11Resource* m_pTexture = nullptr;			// Generic base class pointer
	};

	ID3D11ShaderResourceView* m_pSRV = nullptr;
};
