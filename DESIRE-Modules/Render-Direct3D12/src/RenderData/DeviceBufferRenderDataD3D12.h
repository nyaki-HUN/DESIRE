#pragma once

#include "Engine/Render/RenderData.h"

class DeviceBufferRenderDataD3D12 : public RenderData
{
public:
	ID3D12Resource* m_pBuffer = nullptr;

protected:
	~DeviceBufferRenderDataD3D12()
	{
		DX_SAFE_RELEASE(m_pBuffer);
	}
};
