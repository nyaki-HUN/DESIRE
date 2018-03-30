#include "Engine/stdafx.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Component/PhysicsComponent.h"

Physics::Physics()
{
	// Setup collision masks
	for(size_t i = 0; i < DESIRE_ASIZEOF(collisionMasks); i++)
	{
		collisionMasks[i] = MASK_ALL;

		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::WHEEL, false);
		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::NO_COLLISION, false);
	}
	SetCollisionEnabled(EPhysicsCollisionLayer::DEFAULT, EPhysicsCollisionLayer::WHEEL, true);
}

Physics::~Physics()
{

}

void Physics::OnPhysicsComponentCreated(PhysicsComponent *component)
{
	components.push_back(component);
}

void Physics::OnPhysicsComponentDestroyed(PhysicsComponent *component)
{
	auto it = std::find(components.begin(), components.end(), component);
	if(it != components.end())
	{
		components.erase(it);
	}
}

void Physics::SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled)
{
	ASSERT(a < EPhysicsCollisionLayer::NUM);
	ASSERT(b < EPhysicsCollisionLayer::NUM);

	const int layerA = (int)a;
	const int layerB = (int)b;

	if(enabled)
	{
		collisionMasks[layerA] |= (1 << layerB);
		collisionMasks[layerB] |= (1 << layerA);
	}
	else
	{
		collisionMasks[layerA] &= ~(1 << layerB);
		collisionMasks[layerB] &= ~(1 << layerA);
	}

	// Force refresh collision layer to apply changes in collision matrix
	for(PhysicsComponent *component : components)
	{
		component->SetCollisionLayer(component->GetCollisionLayer());
	}
}

int Physics::GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const
{
	ASSERT(layer < EPhysicsCollisionLayer::NUM);
	return collisionMasks[(size_t)layer];
}

void Physics::UpdateComponents()
{
	for(PhysicsComponent *component : components)
	{
		switch(component->GetBodyType())
		{
			case PhysicsComponent::EBodyType::DYNAMIC:		component->UpdateGameObjectTransform(); break;
			case PhysicsComponent::EBodyType::KINEMATIC:	component->SetTransformFromGameObject(); break;
		}
	}
}
