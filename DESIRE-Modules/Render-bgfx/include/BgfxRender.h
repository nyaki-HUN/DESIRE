#pragma once

#include "Engine/Render/Render.h"

#include "../Externals/bgfx/include/bgfx/bgfx.h"

class BgfxRender : public Render
{
public:
	BgfxRender();

	bool Init(OSWindow& mainWindow) override;
	void UpdateRenderWindow(OSWindow& window) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void EndFrame() override;

	void Clear(uint32_t clearColorRGBA, float depth, uint8_t stencil) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

private:
	RenderData* CreateRenderableRenderData(const Renderable& renderable) override;
	RenderData* CreateMeshRenderData(const Mesh& mesh) override;
	RenderData* CreateShaderRenderData(const Shader& shader) override;
	RenderData* CreateTextureRenderData(const Texture& texture) override;
	RenderData* CreateRenderTargetRenderData(const RenderTarget& renderTarget) override;

	void SetMesh(Mesh& mesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	bgfx::UniformHandle m_samplerUniforms[8];
	bgfx::ViewId m_activeViewId = 0;

	uint32_t m_blendFactor = 0;
};
