#pragma once

#include "Core/Singleton.h"

#include "../Externals/imgui/imgui.h"
#include <memory>

class IWindow;
class DynamicMesh;
class Material;
class Texture;

class ImGuiImpl
{
	DESIRE_SINGLETON(ImGuiImpl)

public:
	void Init();
	void Kill();

	void NewFrame(IWindow *window);
	void EndFrame();

private:
	void DoRender(ImDrawData *data);

	static void RenderDrawListsCallback(ImDrawData *data);

	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};
