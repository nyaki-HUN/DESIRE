#pragma once

#include "Engine/Render/RenderData.h"

class RenderableRenderDataD3D11 : public RenderData
{
public:
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11RasterizerState* m_pRasterizerState = nullptr;
	ID3D11BlendState* m_pBlendState = nullptr;
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

	uint64_t m_inputLayoutKey = 0;
	uint64_t m_rasterizerStateKey = 0;
	uint64_t m_blendStateKey = 0;
	uint64_t m_depthStencilStateKey = 0;
};
