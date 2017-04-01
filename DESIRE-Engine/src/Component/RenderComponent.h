#pragma once

#include "Component/IComponent.h"

class RenderComponent : public IComponent
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("REND");

public:
	RenderComponent();
	~RenderComponent();

	void SetInstanceAlpha(float a);
	float GetInstanceAlpha() const;

private:
	float alpha;
};
