#pragma once

class MeshRenderDataD3D11
{
public:
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;

	uint64_t m_vertexLayoutKey = 0;
};
