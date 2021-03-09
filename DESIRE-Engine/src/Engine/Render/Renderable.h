#pragma once

class Material;
class Mesh;
class RenderData;

class Renderable
{
public:
	~Renderable();

	std::unique_ptr<Mesh> m_spMesh;
	std::shared_ptr<Material> m_spMaterial;

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;
};
