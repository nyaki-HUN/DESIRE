#pragma once

#include "Engine/Render/RenderData.h"

class DeviceBufferRenderDataD3D11 : public RenderData
{
public:
	ID3D11Buffer* m_pBuffer = nullptr;

protected:
	~DeviceBufferRenderDataD3D11()
	{
		DX_SAFE_RELEASE(m_pBuffer);
	}
};
