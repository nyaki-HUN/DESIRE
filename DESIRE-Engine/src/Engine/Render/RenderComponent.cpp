#include "Engine/stdafx.h"
#include "Engine/Render/RenderComponent.h"

#include "Engine/Core/Math/AABB.h"
#include "Engine/Core/Object.h"

RenderComponent::RenderComponent(Object& object)
	: Component(object)
	, m_spAABB(std::make_unique<AABB>())
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::CloneTo(Object& otherObject) const
{
	RenderComponent& otherComponent = otherObject.AddComponent<RenderComponent>();
	DESIRE_TODO("Implement CloneTo");
//	otherComponent.m_mesh =
//	otherComponent.m_material =
//	otherComponent.m_aabb =
	otherComponent.m_renderLayer = m_renderLayer;
	otherComponent.m_isVisible = m_isVisible;
	otherComponent.SetEnabled(IsEnabled());
}

void RenderComponent::SetLayer(int layer)
{
	m_renderLayer = layer;
}

int RenderComponent::GetLayer() const
{
	return m_renderLayer;
}

void RenderComponent::SetVisible(bool visible)
{
	m_isVisible = visible;
}

bool RenderComponent::IsVisible() const
{
	return m_isVisible;
}

const AABB& RenderComponent::GetAABB() const
{
	return *m_spAABB;
}
