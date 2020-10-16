#pragma once

class Material;
class Mesh;
class RenderData;

class Renderable
{
public:
	~Renderable();

	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Material> m_material;

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;
};
