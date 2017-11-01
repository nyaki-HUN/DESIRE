#pragma once

#include <d3d11.h>
#include <memory>

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer *indexBuffer = nullptr;
	ID3D11Buffer *vertexBuffer = nullptr;

	std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> vertexElementDesc;
	uint32_t vertexElementDescCount = 0;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
