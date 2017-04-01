#pragma once

#include "Core/Singleton.h"

class PhysicsComponent;
class Vector3;

enum class EPhysicsCollisionGroup
{
	DEFAULT,
	DYNAMIC,
	WHEEL,
	NO_COLLISION,
};

enum EPhysicsCollisionGroupMask
{
	MASK_DEFAULT		= 1 << (int)EPhysicsCollisionGroup::DEFAULT,
	MASK_DYNAMIC		= 1 << (int)EPhysicsCollisionGroup::DYNAMIC,
	MASK_WHEEL			= 1 << (int)EPhysicsCollisionGroup::WHEEL,
	MASK_NO_COLLISION	= 1 << (int)EPhysicsCollisionGroup::NO_COLLISION,
};

class IPhysics
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IPhysics)

public:
	virtual void Update() = 0;

	virtual PhysicsComponent* CreatePhysicsComponent() = 0;

	virtual void SetCollisionEnabled(EPhysicsCollisionGroup a, EPhysicsCollisionGroup b, bool enabled) = 0;

	virtual bool RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint, PhysicsComponent **o_component = nullptr, int collisionGroupMask = 0xffffffff) = 0;
};
