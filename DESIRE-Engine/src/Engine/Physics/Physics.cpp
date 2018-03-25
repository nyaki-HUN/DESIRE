#include "Engine/stdafx.h"
#include "Engine/Physics/Physics.h"

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
}

int Physics::GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const
{
	ASSERT(layer < EPhysicsCollisionLayer::NUM);
	return collisionMasks[(size_t)layer];
}
