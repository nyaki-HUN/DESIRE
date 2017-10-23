#pragma once

#include "bgfx/bgfx.h"

class MeshRenderDataBgfx
{
public:
	union
	{
		bgfx::IndexBufferHandle indexBuffer;
		bgfx::DynamicIndexBufferHandle dynamicIndexBuffer;
		bgfx::TransientIndexBuffer transientIndexBuffer;
	};

	union
	{
		bgfx::VertexBufferHandle vertexBuffer;
		bgfx::DynamicVertexBufferHandle dynamicVertexBuffer;
		bgfx::TransientVertexBuffer transientVertexBuffer;
	};

	bgfx::VertexDecl vertexDecl;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
