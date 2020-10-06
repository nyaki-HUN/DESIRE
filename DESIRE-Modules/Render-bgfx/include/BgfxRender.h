#pragma once

#include "Engine/Render/Render.h"
#include "Engine/Utils/stl_utils.h"

#include "../Externals/bgfx/include/bgfx/bgfx.h"

#include <unordered_map>

class BgfxRender : public Render
{
public:
	BgfxRender();

	bool Init(OSWindow& mainWindow) override;
	void UpdateRenderWindow(OSWindow& window) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void EndFrame() override;

	void Clear(uint32_t clearColorRGBA = 0x000000FF, float depth = 1.0f, uint8_t stencil = 0) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetColorWriteEnabled(bool r, bool g, bool b, bool a) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest depthTest) override;
	void SetCullMode(ECullMode cullMode) override;
	void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) override;
	void SetBlendModeDisabled() override;

private:
	void* CreateRenderableRenderData(const Renderable& renderable) override;
	void* CreateMeshRenderData(const Mesh& mesh) override;
	void* CreateShaderRenderData(const Shader& shader) override;
	void* CreateTextureRenderData(const Texture& texture) override;
	void* CreateRenderTargetRenderData(const RenderTarget& renderTarget) override;

	void DestroyRenderableRenderData(void* pRenderData) override;
	void DestroyMeshRenderData(void* pRenderData) override;
	void DestroyShaderRenderData(void* pRenderData) override;
	void DestroyTextureRenderData(void* pRenderData) override;
	void DestroyRenderTargetRenderData(void* pRenderData) override;

	void SetMesh(Mesh& mesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetTexture(uint8_t samplerIdx, const Texture& texture, EFilterMode filterMode, EAddressMode addressMode) override;
	void SetRenderTarget(RenderTarget* pRenderTarget) override;
	void UpdateShaderParams(const Material& material) override;

	void DoRender(Renderable& renderable, uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	static bgfx::TextureFormat::Enum GetTextureFormat(const Texture& texture);

	bgfx::UniformHandle m_samplerUniforms[8];
	bgfx::ViewId m_activeViewId = 0;

	std::unordered_map<std::pair<uint64_t, uint64_t>, bgfx::ProgramHandle, stl_utils::hash_pair<uint64_t, uint64_t>> m_shaderProgramCache;

	uint64_t m_renderState = 0;
	uint32_t m_blendFactor = 0;

	bgfx::VertexLayout m_screenSpaceQuadVertexLayout;
};
