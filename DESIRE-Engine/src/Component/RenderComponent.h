#pragma once

#include "Component/IComponent.h"

class Mesh;

class RenderComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("REND");

public:
	RenderComponent();
	~RenderComponent();

	void SetMesh(const Mesh *mesh);

	void SetInstanceAlpha(float a);
	float GetInstanceAlpha() const;

private:
	const Mesh *mesh;

	float alpha;
};
