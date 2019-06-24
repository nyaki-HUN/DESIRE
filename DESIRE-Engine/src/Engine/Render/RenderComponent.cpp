#include "Engine/stdafx.h"
#include "Engine/Render/RenderComponent.h"
#include "Engine/Core/math/AABB.h"

RenderComponent::RenderComponent(Object& object)
	: Component(object)
	, aabb(std::make_unique<AABB>())
{

}

RenderComponent::~RenderComponent()
{

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

void RenderComponent::SetMaterial(Material* i_material)
{
	material = i_material;
}

Material* RenderComponent::GetMaterial() const
{
	return material;
}

const AABB& RenderComponent::GetAABB() const
{
	return *aabb;
}
