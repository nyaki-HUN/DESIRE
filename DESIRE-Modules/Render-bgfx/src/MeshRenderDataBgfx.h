#pragma once

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

	bgfx::VertexLayout vertexLayout;

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
};
