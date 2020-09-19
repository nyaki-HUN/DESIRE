#pragma once

class MeshRenderDataBgfx
{
public:
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;

	bgfx::DynamicIndexBufferHandle dynamicIndexBuffer = BGFX_INVALID_HANDLE;
	bgfx::DynamicVertexBufferHandle dynamicVertexBuffer = BGFX_INVALID_HANDLE;

	bgfx::VertexLayout vertexLayout;
};
