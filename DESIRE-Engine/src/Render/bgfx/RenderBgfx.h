#pragma once

#include "Render/IRender.h"

#include "bgfx/bgfx.h"

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

	void Bind(Mesh *mesh) override;
	void Unbind(Mesh *mesh) override;

	void Bind(Texture *texture) override;
	void Unbind(Texture *texture) override;
	void SetTexture(uint8_t samplerIdx, Texture *texture) override;

	static bgfx::ProgramHandle CreateShaderProgram(const char *vertexShaderFilename, const char *fragmentShaderFilename);

private:
	bool initialized;

	bgfx::UniformHandle samplerUniforms[8];
};
