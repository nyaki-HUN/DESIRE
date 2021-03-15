#pragma once

#include "Engine/Render/RenderData.h"

class RenderTargetRenderDataD3D12 : public RenderData
{
public:
	~RenderTargetRenderDataD3D12() override
	{
		DX_SAFE_RELEASE(m_pHeapForRTVs);
		DX_SAFE_RELEASE(m_pHeapForDSV);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	uint32_t m_numRTVs = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE m_DSV = {};

	ID3D12DescriptorHeap* m_pHeapForRTVs = nullptr;
	ID3D12DescriptorHeap* m_pHeapForDSV = nullptr;
};
