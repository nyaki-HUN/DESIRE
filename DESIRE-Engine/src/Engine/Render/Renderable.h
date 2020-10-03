#pragma once

class Material;
class Mesh;

class Renderable
{
public:
	~Renderable();

	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Material> m_material;

	// Render engine specific data set at bind
	void* m_pRenderData = nullptr;
};
