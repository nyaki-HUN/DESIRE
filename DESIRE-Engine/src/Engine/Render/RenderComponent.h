#pragma once

#include "Engine/Core/Component.h"

class Mesh;
class Material;
class AABB;

class RenderComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("REND");

public:
	RenderComponent(Object& object);
	~RenderComponent() override;

	void SetLayer(int layer);
	int GetLayer() const;

	void SetVisible(bool visible);
	bool IsVisible() const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

	const AABB& GetAABB() const;

private:
	const Mesh* mesh = nullptr;
	Material* material = nullptr;

	std::unique_ptr<AABB> aabb;

	int renderLayer = 0;
	bool isVisible = false;
};
