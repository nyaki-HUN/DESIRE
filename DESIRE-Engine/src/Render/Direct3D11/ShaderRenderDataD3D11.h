#pragma once

#include "Core/MemoryBuffer.h"

#include <d3d11.h>

class ShaderRenderDataD3D11
{
public:
	union
	{
		ID3D11VertexShader *vertexShader;
		ID3D11PixelShader *pixelShader;
		ID3D11ComputeShader *computeShader;

		ID3D11DeviceChild *ptr = nullptr;			// Generic base class pointer
	};

	ID3DBlob *shaderCode = nullptr;

	std::vector<ID3D11Buffer*> constantBuffers;
	std::vector<MemoryBuffer> constantBuffersData;
};
