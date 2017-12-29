#include "stdafx.h"
#include "Component/RenderComponent.h"

RenderComponent::RenderComponent(Object& object)
	: Component(object)
{
	AddToObject();
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
