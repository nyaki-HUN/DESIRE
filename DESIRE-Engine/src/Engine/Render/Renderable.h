#pragma once

class Material;
class Mesh;

class Renderable
{
public:
	~Renderable();

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	// Render engine specific data set at bind
	void* pRenderData = nullptr;
};
