#pragma once

#include "bgfx/bgfx.h"

class MeshRenderDataBgfx
{
public:
	bgfx::IndexBufferHandle indexBuffer;
	bgfx::VertexBufferHandle vertexBuffer;
};
