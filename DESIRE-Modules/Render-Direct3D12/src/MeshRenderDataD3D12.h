#pragma once

#include <d3d12.h>

class MeshRenderDataD3D12
{
public:
	ID3D12Resource* pIndexBuffer = nullptr;
	ID3D12Resource* pVertexBuffer = nullptr;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
