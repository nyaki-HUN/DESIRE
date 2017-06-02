#pragma once

#include "Render/IRender.h"

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
	void UnBind(Mesh *mesh) override;

	void Bind(Texture *texture) override;
	void Unbind(Texture *texture) override;

private:
	bool initialized;
};
