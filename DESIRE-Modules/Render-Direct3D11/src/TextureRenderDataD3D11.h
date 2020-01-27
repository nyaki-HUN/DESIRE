#pragma once

class TextureRenderDataD3D11
{
public:
	ID3D11ShaderResourceView* textureSRV = nullptr;
	ID3D11Texture2D* texture2D = nullptr;
};
