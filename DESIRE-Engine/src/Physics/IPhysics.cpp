#include "stdafx.h"
#include "Physics/IPhysics.h"

IPhysics::IPhysics()
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

IPhysics::~IPhysics()
{

}

void IPhysics::SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled)
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
