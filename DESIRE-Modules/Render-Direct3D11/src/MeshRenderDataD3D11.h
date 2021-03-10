#pragma once

#include "Engine/Render/RenderData.h"

class MeshRenderDataD3D11 : public RenderData
{
public:
	~MeshRenderDataD3D11()
	{
		DX_SAFE_RELEASE(m_pIndexBuffer);
		DX_SAFE_RELEASE(m_pVertexBuffer);
	}

	ID3D11Buffer* m_pIndexBuffer = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
};
