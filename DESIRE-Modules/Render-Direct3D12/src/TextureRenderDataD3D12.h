#pragma once

class TextureRenderDataD3D12
{
public:
	ID3D12Resource* m_pTexture2D = nullptr;
	ID3D12DescriptorHeap* m_pHeapForSRV = nullptr;
};
