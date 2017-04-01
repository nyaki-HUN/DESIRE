#pragma once

#include "Core/Singleton.h"

#include "UI-imgui/include/imgui.h"
#include "bgfx/bgfx.h"

class IWindow;

class ImGuiImpl
{
	DESIRE_DECLARE_SINGLETON(ImGuiImpl)

public:
	void Init();
	void Kill();

	void NewFrame(IWindow *window);
	void EndFrame();

private:
	void Render(ImDrawData *data);

	static void RenderDrawListsCallback(ImDrawData *data);

	bgfx::VertexDecl m_decl;
	bgfx::TextureHandle fontTexture;
	bgfx::ProgramHandle shaderProgram;
	bgfx::UniformHandle s_tex;
};
