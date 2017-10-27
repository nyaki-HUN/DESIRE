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
	void Kill() override;

	void UpdateRenderWindow(IWindow *window) override;

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
	void SetShader(Shader *shader) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	void DoRender() override;

	static bgfx::ProgramHandle CreateShaderProgram(const char *vertexShaderFilename, const char *fragmentShaderFilename);

	bgfx::UniformHandle samplerUniforms[8];

	bool initialized;
};
