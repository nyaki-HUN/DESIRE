#pragma once

#include "DeviceBufferRenderDataD3D12.h"

class IndexBufferRenderDataD3D12 : public DeviceBufferRenderDataD3D12
{
public:
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
};
