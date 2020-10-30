#pragma once

#include "Engine/Physics/Collision.h"
#include "Engine/Core/Container/Array.h"

class Object;
class PhysicsComponent;
class Vector3;

enum class EPhysicsCollisionLayer
{
	Default,
	Dynamic,
	Wheel,
	NoCollision,

	Num = 16
};

class Physics
{
public:
	enum EPhysicsCollisionLayerMask
	{
		MASK_NONE				= 0,
		MASK_DEFAULT			= 1 << static_cast<int>(EPhysicsCollisionLayer::Default),
		MASK_DYNAMIC			= 1 << static_cast<int>(EPhysicsCollisionLayer::Dynamic),
		MASK_WHEEL				= 1 << static_cast<int>(EPhysicsCollisionLayer::Wheel),
		MASK_NO_COLLISION		= 1 << static_cast<int>(EPhysicsCollisionLayer::NoCollision),
		MASK_ALL				= 0xFFFFFFFF
	};

	Physics();
	virtual ~Physics();

	virtual void Update(float deltaTime) = 0;

	virtual PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) = 0;

	void OnPhysicsComponentCreated(PhysicsComponent* pPhysicsComponent);
	void OnPhysicsComponentDestroyed(PhysicsComponent* pPhysicsComponent);

	void SetFixedStepTime(float stepTime);
	float GetFixedStepTime() const;

	virtual void SetGravity(const Vector3& gravity) = 0;
	virtual Vector3 GetGravity() const = 0;

	void SetCollisionEnabled(EPhysicsCollisionLayer a, EPhysicsCollisionLayer b, bool enabled);
	int GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	virtual Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;
	virtual bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;
	virtual Array<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) = 0;

protected:
	void UpdateComponents();

	Array<PhysicsComponent*> m_components;
	int m_collisionMasks[(size_t)EPhysicsCollisionLayer::Num];
	float m_fixedStepTime = 1.0f / 60.0f;
};
