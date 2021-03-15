#pragma once

class IndexBuffer;
class Material;
class Mesh;
class RenderData;
class VertexBuffer;

class Renderable
{
public:
	~Renderable();

	std::shared_ptr<IndexBuffer> m_spIndexBuffer;
	std::shared_ptr<VertexBuffer> m_spVertexBuffer;
	std::shared_ptr<Material> m_spMaterial;

	// Render engine specific data set at bind
	RenderData* m_pRenderData = nullptr;
};
