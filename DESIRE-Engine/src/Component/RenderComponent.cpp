#include "stdafx.h"
#include "Component/RenderComponent.h"

RenderComponent::RenderComponent()
{

}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::SetMesh(const Mesh *i_mesh)
{
	mesh = i_mesh;
}

void RenderComponent::SetInstanceAlpha(float a)
{
	alpha = a;
}

float RenderComponent::GetInstanceAlpha() const
{
	return alpha;
}
