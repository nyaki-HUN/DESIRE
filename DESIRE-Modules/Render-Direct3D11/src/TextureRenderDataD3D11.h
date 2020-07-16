#pragma once

class TextureRenderDataD3D11
{
public:
	union
	{
		ID3D11Texture2D* pTexture2D;
		ID3D11Texture3D* pTexture3D;

		ID3D11Resource* pTexture = nullptr;			// Generic base class pointer
	};

	ID3D11ShaderResourceView* pTextureSRV = nullptr;
};
