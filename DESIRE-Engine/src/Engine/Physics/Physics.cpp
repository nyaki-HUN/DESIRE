#include "Engine/stdafx.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"

Physics::Physics()
{
	// Setup collision masks
	for(size_t i = 0; i < DESIRE_ASIZEOF(collisionMasks); i++)
	{
		collisionMasks[i] = MASK_ALL;

		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::Wheel, false);
		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::NoCollision, false);
	}
	SetCollisionEnabled(EPhysicsCollisionLayer::Default, EPhysicsCollisionLayer::Wheel, true);
}

Physics::~Physics()
{

}

void Physics::OnPhysicsComponentCreated(PhysicsComponent *component)
{
	components.Add(component);
}

void Physics::OnPhysicsComponentDestroyed(PhysicsComponent *component)
{
	components.Remove(component);
}

void Physics::SetFixedStepTime(float stepTime)
{
	fixedStepTime = stepTime;
}

float Physics::GetFixedStepTime() const
{
	return fixedStepTime;
}

void Physics::SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled)
{
	ASSERT(a < EPhysicsCollisionLayer::Num);
	ASSERT(b < EPhysicsCollisionLayer::Num);

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
	ASSERT(layer < EPhysicsCollisionLayer::Num);
	return collisionMasks[(size_t)layer];
}

void Physics::UpdateComponents()
{
	for(PhysicsComponent *component : components)
	{
		switch(component->GetBodyType())
		{
			case PhysicsComponent::EBodyType::Static:		break;
			case PhysicsComponent::EBodyType::Dynamic:		component->UpdateGameObjectTransform(); break;
			case PhysicsComponent::EBodyType::Kinematic:	component->SetTransformFromGameObject(); break;
		}
	}
}
