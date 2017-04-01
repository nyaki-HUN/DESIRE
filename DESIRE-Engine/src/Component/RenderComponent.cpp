#include "stdafx.h"
#include "Component/RenderComponent.h"

RenderComponent::RenderComponent()
{

}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::SetInstanceAlpha(float a)
{
	alpha = a;
}

float RenderComponent::GetInstanceAlpha() const
{
	return alpha;
}
