#pragma once

#include "Engine/Core/Component.h"

class Mesh;
class Material;

class RenderComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("REND");

public:
	RenderComponent(Object& object);
	~RenderComponent() override;

	void SetLayer(int layer);
	int GetLayer() const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

private:
	const Mesh* mesh;
	Material* material;

	int renderLayer;
};
