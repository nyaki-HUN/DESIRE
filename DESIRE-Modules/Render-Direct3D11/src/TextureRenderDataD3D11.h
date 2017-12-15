#pragma once

#include <d3d11.h>

class TextureRenderDataD3D11
{
public:
	ID3D11ShaderResourceView *textureSRV = nullptr;
	ID3D11SamplerState *samplerState = nullptr;
};
