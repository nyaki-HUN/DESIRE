#include "Engine/stdafx.h"
#include "Engine/Physics/Physics.h"

#include "Engine/Physics/PhysicsComponent.h"

Physics::Physics()
{
	// Setup collision masks
	for(size_t i = 0; i < DESIRE_ASIZEOF(m_collisionMasks); i++)
	{
		m_collisionMasks[i] = MASK_ALL;

		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::Wheel, false);
		SetCollisionEnabled((EPhysicsCollisionLayer)i, EPhysicsCollisionLayer::NoCollision, false);
	}
	SetCollisionEnabled(EPhysicsCollisionLayer::Default, EPhysicsCollisionLayer::Wheel, true);
}

Physics::~Physics()
{
}

void Physics::OnPhysicsComponentCreated(PhysicsComponent* pPhysicsComponent)
{
	m_components.Add(pPhysicsComponent);
}

void Physics::OnPhysicsComponentDestroyed(PhysicsComponent* pPhysicsComponent)
{
	m_components.RemoveFast(pPhysicsComponent);
}

void Physics::SetFixedStepTime(float stepTime)
{
	m_fixedStepTime = stepTime;
}

float Physics::GetFixedStepTime() const
{
	return m_fixedStepTime;
}

void Physics::SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled)
{
	ASSERT(a < EPhysicsCollisionLayer::Num);
	ASSERT(b < EPhysicsCollisionLayer::Num);

	const int layerA = static_cast<int>(a);
	const int layerB = static_cast<int>(b);

	if(enabled)
	{
		m_collisionMasks[layerA] |= (1 << layerB);
		m_collisionMasks[layerB] |= (1 << layerA);
	}
	else
	{
		m_collisionMasks[layerA] &= ~(1 << layerB);
		m_collisionMasks[layerB] &= ~(1 << layerA);
	}

	// Force refresh collision layer to apply changes in collision matrix
	for(PhysicsComponent* pPhysicsComponent : m_components)
	{
		pPhysicsComponent->SetCollisionLayer(pPhysicsComponent->GetCollisionLayer());
	}
}

int Physics::GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const
{
	ASSERT(layer < EPhysicsCollisionLayer::Num);
	return m_collisionMasks[(size_t)layer];
}

void Physics::UpdateComponents()
{
	for(PhysicsComponent* pPhysicsComponent : m_components)
	{
		switch(pPhysicsComponent->GetBodyType())
		{
			case PhysicsComponent::EBodyType::Static:		break;
			case PhysicsComponent::EBodyType::Dynamic:		pPhysicsComponent->UpdateGameObjectTransform(); break;
			case PhysicsComponent::EBodyType::Kinematic:	pPhysicsComponent->SetTransformFromGameObject(); break;
		}
	}
}
