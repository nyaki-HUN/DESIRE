#pragma once

#include "Render/IRender.h"

#include "bgfx/bgfx.h"

class ShaderRenderData;

class RenderBgfx : public IRender
{
public:
	RenderBgfx();
	~RenderBgfx();

	void Init(IWindow *mainWindow) override;
	void UpdateRenderWindow(IWindow *window) override;
	void Kill() override;

	String GetShaderFilenameWithPath(const char *shaderFilename) const override;

	void BeginFrame(IWindow *window) override;
	void EndFrame() override;

	void SetViewProjectionMatrices(const Matrix4& viewMatrix, const Matrix4& projMatrix) override;
	void SetViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
	void SetScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void Bind(Mesh *mesh) override;
	void Bind(Shader *shader) override;
	void Bind(Texture *texture) override;

	void Unbind(Mesh *mesh) override;
	void Unbind(Shader *shader) override;
	void Unbind(Texture *texture) override;

private:
	void SetMesh(Mesh *mesh) override;
	void SetShadersFromMaterial(Material *material) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	void DoRender() override;

	bgfx::ProgramHandle activeShaderProgram;
	bgfx::UniformHandle samplerUniforms[8];

	uint8_t activeViewId;
	bool initialized;
};
