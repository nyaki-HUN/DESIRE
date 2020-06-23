#pragma once

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;

	uint64_t vertexLayoutKey = 0;
};
