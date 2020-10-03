#pragma once

#include "Engine/Core/Component.h"
#include "Engine/Core/Container/Array.h"

class AABB;
class Renderable;

class RenderComponent : public Component
{
	DECLARE_COMPONENT_FOURCC_TYPE_ID("REND");

public:
	RenderComponent(Object& object);
	~RenderComponent() override;

	void CloneTo(Object& otherObject) const override;

	void SetLayer(int layer);
	int GetLayer() const;

	void SetVisible(bool visible);
	bool IsVisible() const;

	const AABB& GetAABB() const;

private:
	Array<Renderable*> m_renderables;

	std::unique_ptr<AABB> m_aabb;

	int m_renderLayer = 0;
	bool m_isVisible = false;
};
