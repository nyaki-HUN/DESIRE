#pragma once

#include "Render/IRender.h"
#include "Resource/Texture.h"

#include "bgfx/bgfx.h"

class ShaderRenderData;

class RenderBgfx : public IRender
{
public:
	RenderBgfx();
	~RenderBgfx() override;

	void Init(IWindow *mainWindow) override;
	void UpdateRenderWindow(IWindow *window) override;
	void Kill() override;

	String GetShaderFilenameWithPath(const char *shaderFilename) const override;

	void BeginFrame(IWindow *window) override;
	void EndFrame() override;

	void SetView(View *view) override;

	void SetWorldMatrix(const Matrix4& matrix) override;
	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
	void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	void Bind(Mesh *mesh) override;
	void Bind(Shader *shader) override;
	void Bind(Texture *texture) override;
	void Bind(RenderTarget *renderTarget) override;

	void Unbind(Mesh *mesh) override;
	void Unbind(Shader *shader) override;
	void Unbind(Texture *texture) override;
	void Unbind(RenderTarget *renderTarget) override;

	void UpdateDynamicMesh(DynamicMesh *mesh) override;

private:
	void SetMesh(Mesh *mesh) override;
	void SetShadersFromMaterial(Material *material) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void DoRender() override;

	static bgfx::TextureFormat::Enum ConvertTextureFormat(Texture::EFormat textureFormat);

	bgfx::ProgramHandle activeShaderProgram = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle samplerUniforms[8];
	uint32_t clearColor = 0x000000FF;

	bgfx::ViewId activeViewId = 0;
	bool initialized = false;
};
