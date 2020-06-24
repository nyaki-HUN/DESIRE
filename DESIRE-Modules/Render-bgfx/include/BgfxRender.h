#pragma once

#include "Engine/Render/Render.h"
#include "Engine/Utils/stl_utils.h"

#include "../Externals/bgfx/include/bgfx/bgfx.h"

#include <unordered_map>

class BgfxRender : public Render
{
public:
	BgfxRender();

	void Init(OSWindow* pMainWindow) override;
	void UpdateRenderWindow(OSWindow* pWindow) override;
	void Kill() override;

	void AppendShaderFilenameWithPath(WritableString& outString, const String& shaderFilename) const override;

	void BeginFrame(OSWindow* window) override;
	void EndFrame() override;

	void SetView(View* view) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;

	void SetScissor(uint16_t x = 0, uint16_t y = 0, uint16_t width = 0, uint16_t height = 0) override;
	void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
	void SetColorWriteEnabled(bool r, bool g, bool b, bool a) override;
	void SetDepthWriteEnabled(bool enabled) override;
	void SetDepthTest(EDepthTest depthTest) override;
	void SetCullMode(ECullMode cullMode) override;
	void SetBlendModeSeparated(EBlend srcBlendRGB, EBlend destBlendRGB, EBlendOp blendOpRGB, EBlend srcBlendAlpha, EBlend destBlendAlpha, EBlendOp blendOpAlpha) override;
	void SetBlendModeDisabled() override;

	void Bind(Mesh* pMesh) override;
	void Bind(Shader* pShader) override;
	void Bind(Texture* pTexture) override;
	void Bind(RenderTarget* pRenderTarget) override;

	void Unbind(Mesh* pMesh) override;
	void Unbind(Shader* pShader) override;
	void Unbind(Texture* pTexture) override;
	void Unbind(RenderTarget* pRenderTarget) override;

private:
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void SetMesh(Mesh* pMesh) override;
	void UpdateDynamicMesh(DynamicMesh& dynamicMesh) override;
	void SetVertexShader(Shader* pVertexShader) override;
	void SetFragmentShader(Shader* pFragmentShader) override;
	void SetTexture(uint8_t samplerIdx, Texture* pTexture, EFilterMode filterMode, EAddressMode addressMode) override;
	void UpdateShaderParams(const Material* pMaterial) override;

	void DoRender(uint32_t indexOffset, uint32_t vertexOffset, uint32_t numIndices, uint32_t numVertices) override;

	static bgfx::TextureFormat::Enum GetTextureFormat(const Texture* pTexture);
	static void BindEmbeddedShader(Shader* pShader);

	bgfx::UniformHandle samplerUniforms[8];
	bgfx::ViewId activeViewId = 0;

	std::unordered_map<std::pair<uint64_t, uint64_t>, bgfx::ProgramHandle, stl_utils::hash_pair<uint64_t, uint64_t>> shaderProgramCache;

	uint64_t renderState = 0;
	uint32_t blendFactor = 0;
	uint32_t clearColor = 0x000000FF;

	bgfx::VertexLayout screenSpaceQuadVertexLayout;

	bool initialized = false;
};
