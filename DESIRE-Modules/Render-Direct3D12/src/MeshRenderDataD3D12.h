#pragma once

#include "Engine/Render/RenderData.h"

class MeshRenderDataD3D12 : public RenderData
{
public:
	~MeshRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pIndexBuffer);
		DX_SAFE_RELEASE(m_pVertexBuffer);
	}

	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};

	ID3D12Resource* m_pIndexBuffer = nullptr;
	ID3D12Resource* m_pVertexBuffer = nullptr;
};
