#pragma once

#include <D3D11.h>

class TextureRenderDataD3D11
{
public:
	ID3D11ShaderResourceView *textureSRV = nullptr;
	ID3D11SamplerState *samplerState = nullptr;
};
