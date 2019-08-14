#pragma once

#include <memory>

class DynamicMesh;
class Material;
class OSWindow;
class Texture;
struct ImDrawData;

class ImGuiImpl
{
public:
	ImGuiImpl();
	~ImGuiImpl();

	void Init();
	void Kill();

	void NewFrame(OSWindow* window);
	void EndFrame();

private:
	void DoRender(ImDrawData* data);

	std::unique_ptr<DynamicMesh> mesh;
	std::unique_ptr<Material> material;
	std::shared_ptr<Texture> fontTexture;
};
