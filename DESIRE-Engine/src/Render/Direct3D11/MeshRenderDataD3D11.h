#pragma once

#include <D3D11.h>

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer *indexBuffer = nullptr;
	ID3D11Buffer *vertexBuffer = nullptr;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
