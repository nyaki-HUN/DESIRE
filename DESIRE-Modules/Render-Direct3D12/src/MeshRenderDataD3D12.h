#pragma once

class MeshRenderDataD3D12
{
public:
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
};
