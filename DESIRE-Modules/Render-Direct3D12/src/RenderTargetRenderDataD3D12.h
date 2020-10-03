#pragma once

class RenderTargetRenderDataD3D12
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE m_renderTargetDescriptors[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_depthStencilDescriptor = {};
	uint32_t m_numRenderTargetDescriptors = 0;
};
