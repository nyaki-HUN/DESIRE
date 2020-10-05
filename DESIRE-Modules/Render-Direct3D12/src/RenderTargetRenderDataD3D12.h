#pragma once

class RenderTargetRenderDataD3D12
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	uint32_t m_numRTVs = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE m_DSV = {};

	ID3D12DescriptorHeap* m_pHeapForRTVs = nullptr;
	ID3D12DescriptorHeap* m_pHeapForDSV = nullptr;
};
