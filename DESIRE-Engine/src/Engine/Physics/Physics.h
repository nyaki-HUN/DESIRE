#pragma once

#include "Engine/Physics/Collision.h"

#include <vector>

class Object;
class PhysicsComponent;
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

	Physics();
	virtual ~Physics();

	virtual void Update(float deltaTime) = 0;

	virtual PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) = 0;

	void OnPhysicsComponentCreated(PhysicsComponent *component);
	void OnPhysicsComponentDestroyed(PhysicsComponent *component);

	virtual void SetGravity(const Vector3& gravity) = 0;
	virtual Vector3 GetGravity() const = 0;

	void SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled);
	int GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	virtual Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;
	virtual bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;
	virtual std::vector<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;

protected:
	void UpdateComponents();

	std::vector<PhysicsComponent*> components;
	int collisionMasks[(size_t)EPhysicsCollisionLayer::NUM];
	float fixedStepTime = 1.0f / 60.0f;
};
