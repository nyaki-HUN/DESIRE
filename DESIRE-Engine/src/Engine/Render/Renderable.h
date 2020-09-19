#pragma once

class Material;
class Mesh;

class Renderable
{
public:
	std::shared_ptr<Mesh> mesh;
	std::unique_ptr<Material> material;

	// Render engine specific data set at bind
	void* pRenderData = nullptr;
};
