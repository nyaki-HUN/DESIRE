#pragma once

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer* pIndexBuffer = nullptr;
	ID3D11Buffer* pVertexBuffer = nullptr;

	uint64_t vertexLayoutKey = 0;
};
