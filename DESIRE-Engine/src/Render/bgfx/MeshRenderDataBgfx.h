#pragma once

#include "bgfx/bgfx.h"

class MeshRenderDataBgfx
{
public:
	union
	{
		bgfx::IndexBufferHandle indexBuffer;
		bgfx::DynamicIndexBufferHandle dynamicIndexBuffer;
	};

	union
	{
		bgfx::VertexBufferHandle vertexBuffer;
		bgfx::DynamicVertexBufferHandle dynamicVertexBuffer;
	};

	bgfx::VertexDecl vertexDecl;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
