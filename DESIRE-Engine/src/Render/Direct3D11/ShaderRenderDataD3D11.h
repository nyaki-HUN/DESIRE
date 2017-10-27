#pragma once

#include <d3d11.h>

class ShaderRenderDataD3D11
{
public:
	ID3DBlob *vertexShaderCode = nullptr;
	ID3DBlob *pixelShaderCode = nullptr;

	ID3D11VertexShader *vertexShader = nullptr;
	ID3D11PixelShader *pixelShader = nullptr;
};
