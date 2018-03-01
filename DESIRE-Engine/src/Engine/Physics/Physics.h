#pragma once

#include "Engine/Core/Singleton.h"

class PhysicsComponent;
class Object;
class Vector3;

enum class EPhysicsCollisionLayer
{
	DEFAULT,
	DYNAMIC,
	WHEEL,
	NO_COLLISION,

	NUM = 16
};

class Physics
{
	DESIRE_DECLARE_SINGLETON(Physics)

public:
	enum EPhysicsCollisionLayerMask
	{
		MASK_NONE				= 0,
		MASK_DEFAULT			= 1 << (int)EPhysicsCollisionLayer::DEFAULT,
		MASK_DYNAMIC			= 1 << (int)EPhysicsCollisionLayer::DYNAMIC,
		MASK_WHEEL				= 1 << (int)EPhysicsCollisionLayer::WHEEL,
		MASK_NO_COLLISION		= 1 << (int)EPhysicsCollisionLayer::NO_COLLISION,
		MASK_ALL				= 0xFFFFFFFF
	};

	virtual void Update() = 0;

	virtual PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) = 0;

	void SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled);

	virtual bool RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr = nullptr, Vector3 *o_collisionNormalPtr = nullptr, int layerMask = Physics::MASK_ALL) = 0;
	virtual bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;
	virtual int RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints = nullptr, Vector3 *o_collisionNormals = nullptr, int layerMask = Physics::MASK_ALL) = 0;

protected:
	int collisionMasks[(size_t)EPhysicsCollisionLayer::NUM];
};
