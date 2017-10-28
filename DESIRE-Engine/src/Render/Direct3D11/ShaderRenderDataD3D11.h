#pragma once

#include <d3d11.h>

class ShaderRenderDataD3D11
{
public:
	union
	{
		ID3D11VertexShader *vertexShader;
		ID3D11PixelShader *pixelShader;
		ID3D11ComputeShader *computeShader;

		ID3D11DeviceChild *ptr;			// Generic base class pointer
	};

	ID3D11Buffer *constantBuffer = nullptr;
	ID3DBlob *shaderCode = nullptr;
};
