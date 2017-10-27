#pragma once

#include "Component/IComponent.h"

class Mesh;
class Material;

class RenderComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("REND");

public:
	RenderComponent();
	~RenderComponent();

	void SetLayer(int layer);
	int GetLayer() const;

	void SetMaterial(Material *material);
	Material* GetMaterial() const;

private:
	const Mesh *mesh;
	Material *material;

	int renderLayer;
};
