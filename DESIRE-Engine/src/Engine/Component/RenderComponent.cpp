#include "Engine/stdafx.h"
#include "Engine/Component/RenderComponent.h"

RenderComponent::RenderComponent(Object& object)
	: Component(object)
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

void RenderComponent::SetMaterial(Material *i_material)
{
	material = i_material;
}

Material* RenderComponent::GetMaterial() const
{
	return material;
}
