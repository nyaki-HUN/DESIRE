#pragma once

class RenderTargetRenderDataD3D12
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptors[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = {};
	uint32_t numRenderTargetDescriptors = 0;
};
