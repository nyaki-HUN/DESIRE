#pragma once

#include "Core/Singleton.h"

#include "UI-imgui/include/imgui.h"
#include <memory>

class IWindow;
class Texture;

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

	std::unique_ptr<Texture> fontTexture;
};
