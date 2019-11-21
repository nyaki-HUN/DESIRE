#include "Engine/stdafx.h"
#include "Engine/Render/RenderComponent.h"
#include "Engine/Core/Object.h"
#include "Engine/Core/math/AABB.h"

RenderComponent::RenderComponent(Object& object)
	: Component(object)
	, aabb(std::make_unique<AABB>())
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::CloneTo(Object& otherObject) const
{
	RenderComponent& otherComponent = otherObject.AddComponent<RenderComponent>();
	DESIRE_TODO("Implement CloneTo");
//	otherComponent.mesh =
//	otherComponent.material =
//	otherComponent.aabb =
	otherComponent.renderLayer = renderLayer;
	otherComponent.isVisible = isVisible;
	otherComponent.SetEnabled(IsEnabled());
}

void RenderComponent::SetLayer(int layer)
{
	renderLayer = layer;
}

int RenderComponent::GetLayer() const
{
	return renderLayer;
}

void RenderComponent::SetVisible(bool visible)
{
	isVisible = visible;
}

bool RenderComponent::IsVisible() const
{
	return isVisible;
}

void RenderComponent::SetMaterial(Material* newMaterial)
{
	material = newMaterial;
}

Material* RenderComponent::GetMaterial() const
{
	return material;
}

const AABB& RenderComponent::GetAABB() const
{
	return *aabb;
}
