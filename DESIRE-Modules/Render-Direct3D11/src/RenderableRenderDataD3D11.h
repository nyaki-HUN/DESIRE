#pragma once

class RenderableRenderDataD3D11
{
public:
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

	std::pair<uint64_t, uint64_t> m_inputLayoutKey;
	uint64_t m_depthStencilKey = 0;
};
