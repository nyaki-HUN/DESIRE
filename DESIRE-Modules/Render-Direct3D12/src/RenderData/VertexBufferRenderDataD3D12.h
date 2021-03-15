#pragma once

#include "DeviceBufferRenderDataD3D12.h"

class VertexBufferRenderDataD3D12 : public DeviceBufferRenderDataD3D12
{
public:
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
};
