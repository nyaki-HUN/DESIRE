#pragma once

class MeshRenderDataD3D12
{
public:
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};

	ID3D12Resource* m_pIndexBuffer = nullptr;
	ID3D12Resource* m_pVertexBuffer = nullptr;
};
